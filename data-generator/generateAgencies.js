import { readFileSync, writeFileSync } from "node:fs";
import { getRand } from "./common.js";

/**
 * @typedef {Object} City
 * @property {string} name
 * @property {string} code
 */

/**
 * @param {number} maxCount
 */
export function generateAgencies(maxCount) {
    /** @type { Array<City> } */
    const cities = JSON.parse(readFileSync("./cities.json", "utf-8"));

    /** @type { Map<string, number> } */
    const agencies = new Map();

    for (let i = 0; i < maxCount; i++) {
        let r = getRand(0, cities.length - 1);
        if (agencies.has(cities[r].code)) {
            // @ts-ignore
            agencies.set(cities[r].code, agencies.get(cities[r].code) + 1);
        } else {
            agencies.set(cities[r].code, 1);
        }
    }

    const output = [];
    for (const [code, n] of agencies) {
        for (let i = 0; i < n; i++) {
            output.push(`${code}${(i + 1).toString().padStart(3, "0")}`);
        }
    }

    writeFileSync("agencies.json", JSON.stringify(output, null, 4), "utf-8");
}

const MAX_GENERATED_LIMIT = parseInt(process.argv[2]) ?? 40;

function main() {
    generateAgencies(MAX_GENERATED_LIMIT);
}

main();
