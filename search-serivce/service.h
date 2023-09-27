#pragma once

#include <string>
#include <vector>

#include "database.h"
#include "models.h"

using std::string;
using std::vector;

namespace AirfareSearch {
    class SearchServiceImpl final {
      public:
        // 单例模式
        static SearchServiceImpl &getInstance() {
            static SearchServiceImpl instance;
            return instance;
        }
        SearchServiceImpl(const SearchServiceImpl &) = delete;
        SearchServiceImpl &operator=(const SearchServiceImpl &) = delete;

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

        Response search(Request req);

        void update() {
            // not implmention
        }

      private:
        SearchServiceImpl() {
            auto &stroage = Database::getStroage();
            this->rules = stroage.get_all<PriceRule>();
            spdlog::info("[service]: load {} price rules.", this->rules.size());
        }

        vector<PriceRule> rules;
    };

} // namespace AirfareSearch