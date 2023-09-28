// for invailated docker run instruction cache
// rand seed: 4874jjd

import { getRand } from "./common.js";
import dayjs from "dayjs";
import { readFileSync, writeFileSync } from "fs";
import fast_stringify from "fast-json-stable-stringify";

const CARRIERS = ["CA", "MU", "CZ", "HU", "FM", "ZH", "3U", "MF"];
const HOT_CITIES = [
    "BJS",
    "SHA",
    "CAN",
    "SZX",
    "CTU",
    "HGH",
    "WUH",
    "SIA",
    "CKG",
    "TAO",
    "CSX",
    "NKG",
    "XMN",
    "KMG",
    "DLC",
    "TSN",
    "CGO",
    "SYX",
    "TNA",
    "FOC",
];
const MAX_FLIGHT_NO = 2000;
const MAX_FLIGHT_ROUTE = 30;
const MAX_PRICE_RULE = 800; // for signle (carrier, departure)

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
    const n = getRand(0, 12);
    if (n <= 9) {
        return n.toString();
    } else {
        return "A";
    }
};

const nextCity = (/** @type {string | undefined} */ prev) => {
    let nxt;
    // if (getRand(0, 10) === 0) {
    //     do {
    //         nxt = cities[getRand(0, cities.length - 1)].code;
    //     } while (nxt === prev);
    // } else {
    do {
        nxt = HOT_CITIES[getRand(0, HOT_CITIES.length - 1)];
    } while (nxt === prev);
    // }
    return nxt;
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
                // getRand(0, 2),
                0,
                "day"
            );
            const route = new Route();
            route.departure = nextCity();
            route.arrival = nextCity(route.departure);
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
            route.arrival = nextCity(route.departure);
            route.departureDatetime = dayjs(last.arrivalDatetime)
                .add(getRand(2, 8), "hour")
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
        for (let i = 1; i < getRand(100, MAX_FLIGHT_NO); i++) {
            const f = new Flight(carrier, i);

            for (let j = 1; j < MAX_FLIGHT_ROUTE; j++) {
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
        amount += getRand(200, 800);
        sql += `INSERT INTO price (carrier, departure, arrival, cabin, amount)
                VALUES ('${carrier}', '${departure}', '${arrival}', '${cabin}', ${amount});\n`;

        cabin = "F";
        amount += getRand(200, 800);
        sql += `INSERT INTO price (carrier, departure, arrival, cabin, amount)
                VALUES ('${carrier}', '${departure}', '${arrival}', '${cabin}', ${amount});\n`;
    }
    console.timeLog("generation", "prices generated");

    const _cityUsed = [...citiesUsed];
    const _nextCity = () => _cityUsed[getRand(0, _cityUsed.length - 1)];

    // 生成运价规则

    let sequenceNo = 1;
    for (const carrier of CARRIERS) {
        const price_rules_limit = getRand(MAX_PRICE_RULE / 2, MAX_PRICE_RULE);
        for (let i = 1; i <= price_rules_limit; i++) {
            // const departure = getRand(0, 2) ? "" : _nextCity();
            // const arrival = getRand(0, 2) ? "" : _nextCity();
            const departure = "";
            const arrival =
                i >= (price_rules_limit / 10) * 9 ? "" : _nextCity();
            const _nextCarrier =
                i >= price_rules_limit / 3 && getRand(0, 6)
                    ? ""
                    : nextCarrier();
            const agencies = getRand(0, 1)
                ? ""
                : [...new Set(Array(getRand(0, 10)).fill("").map(nextAgency))];
            const subcharge = getRand(0, 8) ? getRand(0, 100) : -1;

            sql += `INSERT INTO price_rule (sequenceNo, carrier, departure, arrival, nextCarrier, agencies, subcharge)
            VALUES (${sequenceNo}, '${carrier}', '${departure}', '${arrival}', '${_nextCarrier}', '${agencies}', ${subcharge});\n`;
            sequenceNo++;
        }
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
