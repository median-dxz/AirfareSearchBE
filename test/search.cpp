#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "service.h"
#include "str_utils.h"

#include <spdlog/spdlog.h>

using namespace std;
using namespace AirfareSearch;
namespace fs = filesystem;

const string DEFAULT_DB_FILE = fs::absolute(fs::path("/var") / "as_data" / "as.db");

int main() {
    auto &storage = Database::getStroage(DEFAULT_DB_FILE);
    auto &service = SearchServiceImpl::getInstance();

    SearchServiceImpl::Request r;

    r.people = 2;
    r.maxResults = 3;
    r.agencies = vector<string>({"HBR001", "PKX001"});
    r.routes = vector<SearchServiceImpl::Route>();

    SearchServiceImpl::City SHA = {"上海", "SHA"};
    SearchServiceImpl::City NKG = {"南京", "NKG"};
    SearchServiceImpl::City BJS = {"北京", "BJS"};

    r.routes.push_back(SearchServiceImpl::Route{1, SHA, NKG, "20240105"});
    r.routes.push_back(SearchServiceImpl::Route{2, NKG, BJS, "20240105"});
    r.routes.push_back(SearchServiceImpl::Route{3, NKG, SHA, "20240108"});

    for (auto route : r.routes) {
        auto departureDatetime = route.departureDate;

        using namespace chrono_literals;

        auto datetime = dateFromString(departureDatetime, "%Y%m%d");
        datetime += 24h;
        string nextDepartureDatetime = dateToString(datetime, "{:%Y%m%d}");

        spdlog::info("lookup: {} -> {}", route.departure.code, route.arrival.code);

        auto flights = storage.select(
            columns(&Flight::carrier, &Flight::flightNo, &Flight::departureDatetime, &Flight::arrivalDatetime,
                    &Seat::seatC, &Seat::seatF, &Seat::seatY),
            from<Flight>(), natural_join<Seat>(),
            where(c(&Flight::departure) == route.departure.code && c(&Flight::arrival) == route.arrival.code));

        cout << "result size: " << flights.size() << endl;
        auto departureDateRange = make_pair(route.departureDate + "0000", nextDepartureDatetime + "0000");

        using DateRange = decltype(departureDateRange);

        auto vaildateDate = [](const string &departureDatetime, const string &arrivalDatetime, const DateRange &r) {
            return departureDatetime >= r.first && arrivalDatetime <= r.second;
        };

        auto seatToInt = [](const string &s) {
            if (s.front() == 'A') {
                return 10;
            } else {
                return s.front() - '0';
            }
        };

        for (auto &[carrier, flightNo, departureDatetime, arrivalDatetime, c_, f_, y_] : flights) {
            int seatC = seatToInt(c_);
            int seatF = seatToInt(f_);
            int seatY = seatToInt(y_);

            if (vaildateDate(departureDatetime, arrivalDatetime, departureDateRange) &&
                (seatC + seatY + seatF) >= r.people) {
                 
            }
        }
    }

    return 0;
}