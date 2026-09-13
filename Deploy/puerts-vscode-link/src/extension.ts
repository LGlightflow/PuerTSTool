import * as net from "net";
import * as vscode from "vscode";

const assetPathPattern = /\bconst\s+AssetPath\s*=\s*["']([^"']+)["']/g;

type AssetPathMatch = {
    assetPath: string;
    range: vscode.Range;
};

export function activate(context: vscode.ExtensionContext): void {
    const provider = new AssetPathCodeLensProvider();

    context.subscriptions.push(
        vscode.languages.registerCodeLensProvider(
            [
                { language: "typescript", scheme: "file" },
                { language: "typescriptreact", scheme: "file" },
            ],
            provider,
        ),
        vscode.commands.registerCommand("puertsTool.openBlueprint", (assetPath: string) =>
            sendOpenAssetCommand(assetPath, "open"),
        ),
        vscode.commands.registerCommand("puertsTool.revealBlueprint", (assetPath: string) =>
            sendOpenAssetCommand(assetPath, "reveal"),
        ),
    );
}

export function deactivate(): void {
}

class AssetPathCodeLensProvider implements vscode.CodeLensProvider {
    provideCodeLenses(document: vscode.TextDocument): vscode.CodeLens[] {
        return findAssetPaths(document).flatMap((match) => [
            new vscode.CodeLens(match.range, {
                title: "Open Blueprint",
                command: "puertsTool.openBlueprint",
                arguments: [match.assetPath],
            }),
            new vscode.CodeLens(match.range, {
                title: "Reveal In UE",
                command: "puertsTool.revealBlueprint",
                arguments: [match.assetPath],
            }),
        ]);
    }
}

function findAssetPaths(document: vscode.TextDocument): AssetPathMatch[] {
    const matches: AssetPathMatch[] = [];
    const text = document.getText();

    assetPathPattern.lastIndex = 0;
    for (let match = assetPathPattern.exec(text); match; match = assetPathPattern.exec(text)) {
        const assetPath = match[1];
        const assetPathStart = match.index + match[0].indexOf(assetPath);
        const start = document.positionAt(assetPathStart);
        const end = document.positionAt(assetPathStart + assetPath.length);

        matches.push({
            assetPath,
            range: new vscode.Range(start, end),
        });
    }

    return matches;
}

async function sendOpenAssetCommand(assetPath: string, mode: "open" | "reveal"): Promise<void> {
    const config = vscode.workspace.getConfiguration("puertsTool");
    const host = config.get<string>("bridgeHost", "127.0.0.1");
    const port = config.get<number>("bridgePort", 18777);

    try {
        const response = await sendBridgeCommand(host, port, {
            cmd: "openAsset",
            assetPath,
            mode,
        });

        if (!response.ok) {
            vscode.window.showErrorMessage(`PuerTS bridge failed: ${response.message}`);
        }
    } catch (error) {
        const message = error instanceof Error ? error.message : String(error);
        vscode.window.showErrorMessage(`Cannot connect to PuerTS bridge at ${host}:${port}. ${message}`);
    }
}

function sendBridgeCommand(
    host: string,
    port: number,
    payload: Record<string, unknown>,
): Promise<{ ok: boolean; message: string }> {
    return new Promise((resolve, reject) => {
        const socket = net.createConnection({ host, port });
        let response = "";

        socket.setEncoding("utf8");
        socket.setTimeout(3000);

        socket.on("connect", () => {
            socket.write(`${JSON.stringify(payload)}\n`);
        });

        socket.on("data", (chunk: string) => {
            response += chunk;
            if (response.includes("\n")) {
                socket.end();
            }
        });

        socket.on("timeout", () => {
            socket.destroy(new Error("request timed out"));
        });

        socket.on("error", reject);

        socket.on("close", () => {
            if (!response.trim()) {
                reject(new Error("empty bridge response"));
                return;
            }

            try {
                const parsed = JSON.parse(response.trim()) as { ok?: boolean; message?: string };
                resolve({
                    ok: Boolean(parsed.ok),
                    message: parsed.message ?? "",
                });
            } catch {
                reject(new Error(`invalid bridge response: ${response.trim()}`));
            }
        });
    });
}
