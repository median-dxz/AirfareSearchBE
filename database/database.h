#pragma once

#include <spdlog/spdlog.h>
#include <sqlite_orm/sqlite_orm.h>
#include <string>

#include "models.h"

using namespace sqlite_orm;

static auto createStroage(std::string db_path) {
    using namespace Database;
    auto flight_table =
        make_table("flight", make_column("carrier", &Flight::carrier), make_column("flightNo", &Flight::flightNo),
                   make_column("departureDatetime", &Flight::departureDatetime),
                   make_column("arrivalDatetime", &Flight::arrivalDatetime),
                   make_column("departure", &Flight::departure), make_column("arrival", &Flight::arrival),
                   primary_key(&Flight::carrier, &Flight::flightNo, &Flight::departure, &Flight::arrival,
                               &Flight::departureDatetime));

    auto price_table =
        make_table("price", make_column("carrier", &Price::carrier), make_column("departure", &Price::departure),
                   make_column("arrival", &Price::arrival), make_column("cabin", &Price::cabin),
                   make_column("amount", &Price::amount), check(in(&Price::cabin, {"Y", "C", "F"})));

    auto price_rule_table =
        make_table("price_rule", make_column("sequenceNo", &PriceRule::sequenceNo, unique()),
                   make_column("carrier", &PriceRule::carrier), make_column("departure", &PriceRule::departure),
                   make_column("arrival", &PriceRule::arrival), make_column("nextCarrier", &PriceRule::nextCarrier),
                   make_column("agencies", &PriceRule::agencies), make_column("subcharge", &PriceRule::subcharge),
                   check(c(&PriceRule::subcharge) >= -1 and c(&PriceRule::subcharge) <= 100));

    auto seat_table = make_table(
        "seat", make_column("carrier", &Seat::carrier), make_column("flightNo", &Seat::flightNo),
        make_column("departure", &Seat::departure), make_column("arrival", &Seat::arrival),
        make_column("departureDatetime", &Seat::departureDatetime), make_column("seatF", &Seat::seatF),
        make_column("seatC", &Seat::seatC), make_column("seatY", &Seat::seatY),
        primary_key(&Seat::carrier, &Seat::flightNo, &Seat::departure, &Seat::arrival, &Seat::departureDatetime),
        foreign_key(&Seat::carrier, &Seat::flightNo, &Seat::departure, &Seat::arrival, &Seat::departureDatetime)
            .references(&Flight::carrier, &Flight::flightNo, &Flight::departure, &Flight::arrival,
                        &Flight::departureDatetime));

    spdlog::info("[database]: load db file :{}", db_path);

    auto stroage = make_storage(db_path, make_index("flight_departrue_datetime_index", &Flight::departureDatetime),
                                make_index("flight_index", &Flight::departure, &Flight::arrival),
                                make_index("price_index", &Price::departure, &Price::arrival),
                                make_index("price_rule_index", &PriceRule::carrier),
                                make_index("seat_index", &Seat::departure, &Seat::arrival), flight_table, price_table,
                                price_rule_table, seat_table);

    stroage.sync_schema();

    spdlog::info("[database]: db file loaded succeed.");
    return stroage;
};

namespace Database {
    using Storage = decltype(createStroage(""));
    Storage &getStorage(std::optional<std::string> path = std::nullopt);
}; // namespace Database
