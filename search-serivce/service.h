#pragma once

#include <string>
#include <vector>

#include "models.h"
#include "database.h"

using std::string;
using std::vector;

namespace AirfareSearch {
    namespace SearchService {
        struct City {
            string name;
            string code;
        };

        struct Route {
            int id;
            City departure;
            City arrival;
            string departureDate;
        };

        struct Flight {
            string carrier;
            string flightNo;
            City departure;
            City arrival;
            string departureDatetime;
            string arrivalDatetime;
            vector<Cabin> cabins;
        };

        struct FlightResult {
            int price;
            vector<Flight> flights;
            vector<string> agencies;
        };

        struct Request {
            int people;
            int maxResults;
            vector<string> agencies;
            vector<Route> routes;
        };

        struct Response {
            vector<FlightResult> data;
        };

        using Cabin = Cabin;
    } // namespace SearchService

    SearchService::Response search(SearchService::Request req);

} // namespace AirfareSearch