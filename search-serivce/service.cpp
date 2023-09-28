#include <chrono>
#include <map>
#include <queue>
#include <set>

#include <spdlog/spdlog.h>

#include "service.h"
#include "str_utils.h"

using namespace std::chrono_literals;

using FlightResult = SearchServiceImpl::FlightResult;
using RoutePrice = ServiceInternal::RoutePrice;

int seatToInt(const string &s);

SearchServiceImpl::Response SearchServiceImpl::search(SearchServiceImpl::Request request) {
    using Database::Cabin;
    using namespace std;
    using namespace ServiceInternal;

    Database::Storage storage = Database::getStorage();

    unordered_map<RoutePriceIdentifier, RoutePrice, RoutePriceIdentifier> routePriceMap; // 查询单段路程
    vector<vector<SearchServiceImpl::Flight>> allFlights(request.routes.size(),
                                                         vector<SearchServiceImpl::Flight>()); // 每段可用的航班

    const int &passenger = request.people;

    for (int cur_route = 0; cur_route < request.routes.size(); cur_route++) {
        using Database::Flight;
        using Database::Price;
        using Database::Seat;
        auto &[id, departure, arrival, departureDate] = request.routes[cur_route];
        auto departureDate_{departureDate}; // 无法在lambda内捕获解构赋值

        // 时间校验, 保证搜索到的航班是当天起飞的
        auto validateDate = [&departureDate_](const string &departureDatetimeLiteral) {
            auto departureDatetime = dateFromString(departureDatetimeLiteral, "%Y%m%d%H%M");
            return departureDatetime >= dateFromString(departureDate_ + "0000", "%Y%m%d%H%M") &&
                   departureDatetime <= dateFromString(departureDate_ + "0000", "%Y%m%d%H%M") + 24h;
        };

        spdlog::info("lookup: {} -> {}", departure.code, arrival.code);

        // 查找所有票价信息
        auto price = storage.select(
            columns(&Flight::carrier, &Flight::departure, &Flight::arrival, &Price::cabin, &Price::amount),
            from<Flight>(), natural_join<Price>(),
            where(c(&Flight::departure) == departure.code && c(&Flight::arrival) == arrival.code));

        for (const auto &[carrier, departure, arrival, cabin, amount] : price) {
            auto &routePrice = routePriceMap[RoutePriceIdentifier{carrier, departure, arrival}];
            if (cabin == Cabin::Y) {
                routePrice.seatY.amount = amount;
            } else if (cabin == Cabin::F) {
                routePrice.seatF.amount = amount;
            } else if (cabin == Cabin::C) {
                routePrice.seatC.amount = amount;
            }
        }
        spdlog::debug("preprocess: load all price info successfully");

        // 查找所有航班信息
        auto flights =
            storage.select(columns(&Flight::carrier, &Flight::flightNo, &Flight::departureDatetime,
                                   &Flight::arrivalDatetime, &Seat::seatC, &Seat::seatF, &Seat::seatY),
                           from<Flight>(), natural_join<Seat>(),
                           where(c(&Flight::departure) == departure.code && c(&Flight::arrival) == arrival.code));
        spdlog::debug("preprocess: load all flight info successfully");

        for (auto &[carrier, flightNo, departureDatetime, arrivalDatetime, seatC, seatF, seatY] : flights) {

            // 确认此行程可用的航班
            if (validateDate(departureDatetime) &&
                (seatToInt(seatY) + seatToInt(seatC) + seatToInt(seatF)) >= passenger) {

                // 设置余座信息
                auto &routePrice = routePriceMap[RoutePriceIdentifier{carrier, departure.code, arrival.code}];
                routePrice.seatC.remain = seatToInt(seatC);
                routePrice.seatF.remain = seatToInt(seatF);
                routePrice.seatY.remain = seatToInt(seatY);

                allFlights[cur_route].push_back(SearchServiceImpl::Flight{carrier, padLeft(to_string(flightNo), '0', 4),
                                                                          departure.code, arrival.code,
                                                                          departureDatetime, arrivalDatetime});
            }
        }
        spdlog::info("find {} available flights", allFlights[cur_route].size());
    }

    // 搜索可用航班
    queue<FlightResult> que;
    vector<FlightResult> result_set;

    for (const auto &flight : allFlights.front()) {
        que.push(FlightResult{0, vector{flight}, request.agencies});
    }
    spdlog::info("initial states set finished");

    while (!que.empty()) {
        auto flight_result = que.front();
        que.pop();

        // 取出当前航程
        auto flight = flight_result.flights.back();
        auto route_no = flight_result.flights.size() - 1;

        spdlog::debug("check route: {} {} -> {} at route: {}", flight.carrier + flight.flightNo, flight.departure,
                      flight.arrival, flight_result.flights.size());

        auto rules =
            storage.get_all_pointer<Database::PriceRule>(where(c(&Database::PriceRule::carrier) == flight.carrier),
                                                         order_by(&Database::PriceRule::sequenceNo).asc());

        struct MatchResult {
            int subcharge = 0;
            vector<string> next_agencies;
            string next_carrier;
        };
        MatchResult match_result; // 同时也是默认结果 (不匹配所有规则, 但是视为通过)

        Database::PriceRule debug_rule_no{0};

        for (const auto &rule : rules) {
            auto available_next_agencies = flight_result.agencies;
            // 匹配 departure
            if (rule->departure.has_value() && rule->departure != "" && rule->departure != flight.departure) {
                continue;
            }

            // 匹配 arrival
            if (rule->arrival.has_value() && rule->arrival != "" && rule->arrival != flight.arrival) {
                continue;
            }

            // 匹配 是否有可出票的代理人
            if (rule->agencies.has_value() && rule->agencies.value() != "") {
                std::set<string> allowed_next_agencies;

                for (const auto &a : splitString(rule->agencies.value(), ',')) {
                    allowed_next_agencies.emplace(a);
                }

                // 删除不在允许列表内的代理人
                // 对allowed_next_agencies 和 available_next_agencies 取交
                for (auto i = available_next_agencies.begin(); i != available_next_agencies.end();) {
                    if (!allowed_next_agencies.count(*i)) {
                        i = available_next_agencies.erase(i);
                    } else {
                        i++;
                    }
                }
            }

            // 没有可用的出票代理人
            if (available_next_agencies.size() == 0) {
                continue;
            }

            // 匹配成功, 构造下一个状态
            match_result.next_carrier = rule->nextCarrier.value_or("");
            match_result.next_agencies = available_next_agencies;
            match_result.subcharge = rule->subcharge;
            debug_rule_no = Database::PriceRule(*rule);
            break;
        }

        spdlog::debug("match rule: {}", debug_rule_no.sequenceNo);
        if (match_result.subcharge == -1) {
            // 无穷大不可出票
            continue;
        }

        int next_route_no = route_no + 1;
        auto &routePrice = routePriceMap.at(RoutePriceIdentifier{flight.carrier, flight.departure, flight.arrival});

        const auto &[price, cabins] = routePrice.countPrice(passenger, match_result.subcharge);

        flight_result.price += price;
        flight_result.agencies = match_result.next_agencies;
        flight_result.flights.back().cabins = cabins;

        // 判断是否是最后一段
        if (next_route_no == request.routes.size()) {
            // 加入结果集
            result_set.emplace_back(flight_result);
        } else {
            // 遍历寻找所有可能的下一个状态
            for (const auto &next_flight : allFlights[next_route_no]) {
                if (match_result.next_carrier != "" && next_flight.carrier != match_result.next_carrier) {
                    continue;
                }

                // 两段航班衔接时间要大于 120min
                auto prevArrivalDatetime = dateFromString(flight.arrivalDatetime, "%Y%m%d%H%M");
                auto nextDepartureDatetime = dateFromString(next_flight.departureDatetime, "%Y%m%d%H%M");
                if (nextDepartureDatetime - prevArrivalDatetime < 120min) {
                    continue;
                }

                // 拷贝之前状态
                auto next_state = flight_result;
                next_state.flights.emplace_back(next_flight);
                // 加入状态队列继续搜索
                que.emplace(next_state);
            }
        }
    }

    // 处理 result_set
    // 1. 按价格排序
    // 2. 筛选前maxLimit条结果
    sort(result_set.begin(), result_set.end(),
         [](const SearchServiceImpl::FlightResult &a, const SearchServiceImpl::FlightResult &b) {
             return a.price < b.price;
         });

    SearchServiceImpl::Response response;
    response.data = vector<SearchServiceImpl::FlightResult>(
        result_set.begin(),
        request.maxResults > result_set.size() ? result_set.end() : result_set.begin() + request.maxResults);

    return response;
}

int seatToInt(const string &s) {
    if (s.front() == 'A') {
        return 10;
    } else {
        return s.front() - '0';
    }
};

std::pair<int, vector<Database::Cabin>> RoutePrice::countPrice(int people, double subcharge) {
    vector<SeatPrice> seats{this->seatY, this->seatC, this->seatF};
    auto result = std::make_pair(0, vector<Database::Cabin>());
    for (const auto &s : seats) {
        int cnt = std::min(people, s.remain);
        result.first += cnt * s.amount;
        while (cnt--) {
            result.second.emplace_back(s.type);
        }
        if (people >= s.remain) {
            people -= s.remain;
        } else {
            people = 0;
        }
        if (people <= 0) {
            break;
        }
    }
    result.first = result.first * (1 + subcharge / 100);
    return result;
}