#pragma once

#include <sqlite_orm/sqlite_orm.h>
#include <string>

#include "models.h"

const std::string DB_FILE = "./as.db";

using namespace sqlite_orm;

static auto createStroage() {
    auto stroage = make_storage(
        DB_FILE,
        make_table("flight", make_column("carrier", &Flight::carrier), make_column("flightNo", &Flight::flightNo),
                   make_column("departureDatetime", &Flight::departureDatetime),
                   make_column("arrivalDatetime", &Flight::arrivalDatetime),
                   make_column("departure", &Flight::departure), make_column("arrival", &Flight::arrival)),
        make_table("price", make_column("carrier", &Price::carrier), make_column("departure", &Price::departure),
                   make_column("arrival", &Price::arrival), make_column("cabin", &Price::cabin),
                   make_column("amount", &Price::amount)),
        make_table("price_rule", make_column("sequenceNo", &PriceRule::sequenceNo),
                   make_column("carrier", &PriceRule::carrier), make_column("departure", &PriceRule::departure),
                   make_column("arrival", &PriceRule::arrival), make_column("nextCarrier", &PriceRule::nextCarrier),
                   make_column("agencies", &PriceRule::agencies), make_column("subcharge", &PriceRule::subcharge)),
        make_table("seat", make_column("carrier", &Seat::carrier), make_column("flightNo", &Seat::flightNo),
                   make_column("departure", &Seat::departure), make_column("arrival", &Seat::arrival),
                   make_column("departureDatetime", &Seat::departureDatetime), make_column("seatF", &Seat::seatF),
                   make_column("seatC", &Seat::seatC), make_column("seatY", &Seat::seatY)));

    stroage.sync_schema();

    return stroage;
};

class Database {
  public:
    using Stroage = decltype(createStroage());
    Stroage &getStroage() { return Database::stroage; }

  private:
    static Stroage stroage;
};
