import ModuleBase from "../../Framework/Module/ModuleBase";

class TestModuleTemplate extends ModuleBase {
    constructor() {
        super();
        // Additional initialization if needed
    }

    Initialize(): void {
        super.Initialize();
        // Implement your own initialization logic here
        console.log("TestModuleTemplate initialized");
    }

    Start(): void {
        super.Start();
        // Implement your own start logic here
        console.log("TestModuleTemplate started");

    }
}

export default TestModuleTemplate;