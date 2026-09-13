# PuerTS TypeScript Engineering Template

This folder contains the lightweight ESLint and Prettier setup used to standardize business-side PuerTS TypeScript code.

## Usage

Copy all files in this folder to the root of a UE project that contains a `TypeScript` folder.

Then install dependencies and run the checks:

```sh
npm install
npm run check
```

For daily formatting and auto-fixes:

```sh
npm run fix
```

## Scripts

- `npm run lint`: Fix ESLint issues in `TypeScript`.
- `npm run lint:check`: Check ESLint issues without writing files.
- `npm run format`: Format TypeScript and root config files.
- `npm run format:check`: Check formatting without writing files.
- `npm run fix`: Run lint fixes and Prettier formatting.
- `npm run check`: Run lint and format checks.
