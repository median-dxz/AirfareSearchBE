#pragma once

#include <string>
#include <vector>

#include "database.h"
#include "models.h"

using std::string;
using std::vector;

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
        vector<Database::Cabin> cabins;
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

    Response search(Request req);

    void update() {
        // not implmention
    }

  private:
    SearchServiceImpl() {
        auto &storage = Database::getStorage();
        this->rules = storage.get_all<Database::PriceRule>();
        spdlog::info("[service]: load {} price rules.", this->rules.size());
    }

    vector<Database::PriceRule> rules;
};
