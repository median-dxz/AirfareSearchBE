#pragma once

#include <spdlog/spdlog.h>

#include "protos/SearchRequest.grpc.pb.h"
#include "service.h"

namespace AirfareSearch {
    class DataBuilder final {
      public:
        // 单例模式
        static DataBuilder &getInstance() {
            static DataBuilder instance;
            return instance;
        }
        DataBuilder(const DataBuilder &) = delete;
        DataBuilder &operator=(const DataBuilder &) = delete;

        SearchService::Request request(SearchRequest req);
        void bindResponse(SearchResponse res_, SearchService::Response res);

      private:
        DataBuilder() {}

        void bindFlight(AirfareSearch::Flight *flight_, const SearchService::Flight &flight);

        SearchService::City getCity(const AirfareSearch::City &_r) const {
            return SearchService::City{_r.code(), _r.name()};
        }

        SearchService::Route getRoute(const AirfareSearch::SearchRoute &_r) const {
            return SearchService::Route{_r.id(), this->getCity(_r.departure()), this->getCity(_r.arrival()),
                                        _r.departuredate()};
        }

        AirfareSearch::City *city(const SearchService::City &city) {
            auto c = new AirfareSearch::City();
            c->set_code(city.code);
            c->set_name(city.name);
            return c;
        }

        AirfareSearch::Cabin cabin(const SearchService::Cabin &cabin) {
            switch (cabin) {
            case SearchService::Cabin::F:
                return AirfareSearch::Cabin::F;
                break;
            case SearchService::Cabin::Y:
                return AirfareSearch::Cabin::Y;
                break;
            case SearchService::Cabin::C:
                return AirfareSearch::Cabin::C;
                break;
            default:
                spdlog::error("Error Cabin Enum Type");
                throw new std::bad_cast();
            }
        }
    };
}; // namespace AirfareSearch