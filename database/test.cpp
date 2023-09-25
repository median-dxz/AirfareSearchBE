#include <iostream>
#include <spdlog/spdlog.h>
#include <sqlite_orm/sqlite_orm.h>

#include "database.h"

using namespace std;

int main() {
    Database db;
    auto &storage = db.getStroage();

    auto flights = storage.select(object<Flight>(), where(is_equal(&Flight::arrival, "SHA")));

    for (const auto &flight : flights) {
        cout << flight.carrier << flight.flightNo << endl;
    }

    spdlog::warn("{} {}", "gg", 11);
    
    return 0;
}