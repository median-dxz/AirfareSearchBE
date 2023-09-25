import { getRand } from "./common.js";
import dayjs from "dayjs";
import { readFileSync, writeFileSync } from "fs";
import fast_stringify from "fast-json-stable-stringify";

const CARRIERS = ["CA", "MU", "CZ", "HU", "FM", "ZH", "3U", "MF"];
const MAX_FLIGHT_NO = 999;
const MAX_FLIGHT_ROUTE = 200;
const MAX_PRICE_RULE = 1000;

// const CARRIERS = ["CA", "MU"];
// const MAX_FLIGHT_NO = 20;
// const MAX_FLIGHT_ROUTE = 2;
// const MAX_PRICE_RULE = 100;

const sql_filename = process.argv[2];

const cities = JSON.parse(readFileSync("./cities.json", { encoding: "utf-8" }));
const agencies = JSON.parse(
    readFileSync("./agencies.json", { encoding: "utf-8" })
);

const nextSeat = () => {
    const n = getRand(0, 15);
    if (n <= 9) {
        return n.toString();
    } else {
        return "A";
    }
};

const nextCity = () => {
    return cities[getRand(0, cities.length - 1)].code;
};

const nextCarrier = () => {
    return CARRIERS[getRand(0, CARRIERS.length - 1)];
};

const nextAgency = () => {
    return agencies[getRand(0, agencies.length - 1)];
};

class Route {
    /** @type {string} */
    departure;
    /** @type {string} */
    arrival;
    /** @type {Date} */
    departureDatetime;
    /** @type {Date} */
    arrivalDatetime;
}

class Flight {
    /** @type {string} */
    carrier;
    /** @type {number} */
    flightNo;
    /** @type {Array<Route>} */
    routes = [];

    /**
     * @param {string} carrier
     * @param {number} flightNo
     */
    constructor(carrier, flightNo) {
        this.carrier = carrier;
        this.flightNo = flightNo;
    }

    next() {
        if (this.routes.length === 0) {
            const start = dayjs("2024-01-01 05:00:00").add(
                getRand(0, 100),
                "day"
            );
            const route = new Route();
            route.departure = nextCity();
            route.arrival = nextCity();
            route.departureDatetime = start.toDate();
            route.arrivalDatetime = start
                .add(getRand(60, 180), "minutes")
                .toDate();
            this.routes.push(route);
            return true;
        } else {
            const last = this.routes[this.routes.length - 1];
            const route = new Route();
            route.departure = last.arrival;
            route.arrival = nextCity();
            route.departureDatetime = dayjs(last.arrivalDatetime)
                .add(getRand(8, 24 * 5), "hour")
                .toDate();
            route.arrivalDatetime = dayjs(route.departureDatetime)
                .add(getRand(60, 180), "minutes")
                .toDate();
            this.routes.push(route);
            return dayjs(route.arrivalDatetime).isBefore("2025-01-01");
        }
    }
}

function generate() {
    let sql = "";

    const pricePrimaryKey = new Set();
    // 用到的城市集合
    const citiesUsed = new Set();

    console.time("generation");
    // 生成航班和配套余座
    for (const carrier of CARRIERS) {
        for (let i = 1; i < getRand(5, MAX_FLIGHT_NO / 3); i++) {
            const f = new Flight(carrier, i);

            for (let j = 1; j < getRand(2, MAX_FLIGHT_ROUTE); j++) {
                if (!f.next()) {
                    break;
                }
            }

            for (const {
                arrival,
                arrivalDatetime,
                departure,
                departureDatetime,
            } of f.routes) {
                citiesUsed.add(departure);
                citiesUsed.add(arrival);

                pricePrimaryKey.add(
                    fast_stringify({ carrier, departure, arrival })
                );
                sql += `INSERT INTO flight (carrier, flightNo, departure, arrival, departureDatetime, arrivalDatetime)
                    VALUES ('${carrier}', ${i}, '${departure}', '${arrival}', 
                        '${dayjs(departureDatetime).format("YYYYMMDDHHmm")}', 
                        '${dayjs(arrivalDatetime).format("YYYYMMDDHHmm")}'
                    );\n`;

                sql += `INSERT INTO seat (carrier, flightNo, departure, arrival, departureDatetime, seatF, seatC, seatY)
                    VALUES ('${carrier}', ${i}, '${departure}', '${arrival}', 
                        '${dayjs(departureDatetime).format("YYYYMMDDHHmm")}', 
                        '${nextSeat()}', '${nextSeat()}', '${nextSeat()}'
                    );\n`;
            }
        }
    }
    console.timeLog("generation", "flights and seat generated");

    // 生成运价
    for (const key of pricePrimaryKey) {
        const { carrier, departure, arrival } = JSON.parse(key);
        let amount = getRand(800, 3000);
        let cabin = "Y";
        sql += `INSERT INTO price (carrier, departure, arrival, cabin, amount)
                VALUES ('${carrier}', '${departure}', '${arrival}', '${cabin}', ${amount});\n`;

        cabin = "C";
        amount += getRand(800, 1000);
        sql += `INSERT INTO price (carrier, departure, arrival, cabin, amount)
                VALUES ('${carrier}', '${departure}', '${arrival}', '${cabin}', ${amount});\n`;

        cabin = "F";
        amount += getRand(800, 1000);
        sql += `INSERT INTO price (carrier, departure, arrival, cabin, amount)
                VALUES ('${carrier}', '${departure}', '${arrival}', '${cabin}', ${amount});\n`;
    }
    console.timeLog("generation", "prices generated");

    const _cityUsed = [...citiesUsed];
    const _nextCity = () => _cityUsed[getRand(0, _cityUsed.length - 1)];

    // 生成运价规则
    const price_rules_limit = getRand(MAX_PRICE_RULE / 2, MAX_PRICE_RULE);
    for (let sequenceNo = 1; sequenceNo <= price_rules_limit; sequenceNo++) {
        const carrier = nextCarrier();
        const departure = getRand(0, 2) ? "" : _nextCity();
        const arrival = getRand(0, 2) ? "" : _nextCity();
        const _nextCarrier = getRand(0, 2) ? "" : nextCarrier();
        const agencies = getRand(0, 1)
            ? ""
            : Array(getRand(0, 10)).fill("").map(nextAgency);
        const subcharge = getRand(0, 8) ? getRand(0, 100) : -1;
        sql += `INSERT INTO price_rule (sequenceNo, carrier, departure, arrival, nextCarrier, agencies, subcharge)
                VALUES (${sequenceNo}, '${carrier}', '${departure}', '${arrival}', '${_nextCarrier}', '${agencies}', ${subcharge});\n`;
    }
    console.timeLog("generation", "price rules generated");

    return sql;
}

try {
    const sql = generate();
    // console.log(sql);
    writeFileSync(sql_filename, sql);
    console.timeEnd("generation");
    console.log("data generation finished.");
} catch (error) {
    console.error(error);
}
