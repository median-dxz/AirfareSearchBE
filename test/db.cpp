#include <filesystem>
#include <iostream>
#include <spdlog/spdlog.h>
#include <sqlite_orm/sqlite_orm.h>

#include "database.h"

using namespace std;
namespace fs = std::filesystem;

const string DB_PATH = fs::absolute(fs::path(getenv("HOME")) / "as_data" / "as.db");

int main() {
    auto &storage = Database::getStroage(DB_PATH);
    auto &storage1 = Database::getStroage();

    assert(&storage == &storage1);

    auto flights = storage.select(object<Flight>(), where(is_equal(&Flight::arrival, "CGD")));
    cout << "find results: " << flights.size() << endl;
    for (const auto &flight : flights) {
        cout << flight.carrier << flight.flightNo << endl;
    }

    return 0;
}