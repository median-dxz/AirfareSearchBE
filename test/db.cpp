#include <filesystem>
#include <iostream>
#include <spdlog/spdlog.h>
#include <sqlite_orm/sqlite_orm.h>

#include "database.h"

using namespace std;
namespace fs = std::filesystem;

const string DEFAULT_DB_FILE = fs::absolute(fs::path("/var") / "as_data" / "as.db");

int main() {
    auto &storage = Database::getStorage(DEFAULT_DB_FILE);
    auto &storage1 = Database::getStorage();

    assert(&storage == &storage1);

    auto flights = storage.select(object<Flight>(), where(is_equal(&Flight::arrival, "CGD")));
    cout << "find results: " << flights.size() << endl;
    for (const auto &flight : flights) {
        cout << flight.carrier << flight.flightNo << endl;
    }

    return 0;
}