#pragma once

#include <string>
#include <vector>

#include "database.h"
#include "models.h"

using std::string;
using std::vector;

namespace ServiceInternal {
    // 一种座位的价格相关信息
    struct SeatPrice {
        int remain = 0;
        int amount = 0;
        Database::Cabin type;
    };

    // 一段航程查询票价所需的相关信息
    struct RoutePrice {
        SeatPrice seatY;
        SeatPrice seatF;
        SeatPrice seatC;

        RoutePrice() {
            this->seatY.type = Database::Cabin::Y;
            this->seatF.type = Database::Cabin::F;
            this->seatC.type = Database::Cabin::C;
        }

        std::pair<int, vector<Database::Cabin>> countPrice(int people, double subcharge);
    };

    // 一段航程票价的Key
    struct RoutePriceIdentifier {
        string carrier;
        string departure;
        string arrival;

        size_t operator()(const RoutePriceIdentifier &key) const {
            return std::hash<string>()(key.carrier + key.departure + key.arrival);
        }

        bool operator==(const RoutePriceIdentifier &other) const {
            return carrier == other.carrier && departure == other.departure && arrival == other.arrival;
        }
    };
}; // namespace ServiceInternal

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
        string departure;
        string arrival;
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

    Response search(Request request);

    void update() {
        // not implmention
    }

  private:
    SearchServiceImpl() { auto &storage = Database::getStorage(); }
};