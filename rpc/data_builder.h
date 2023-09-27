#pragma once

#include <spdlog/spdlog.h>

#include "protos/SearchRequest.grpc.pb.h"
#include "service.h"

namespace AirfareSearch {
    namespace DataBuilder {
        SearchServiceImpl::Request request(SearchRequest req);

        void bindResponse(SearchResponse &res_, SearchServiceImpl::Response res);
        void bindFlight(AirfareSearch::Flight *flight_, const SearchServiceImpl::Flight &flight);

        SearchServiceImpl::City getCity(const AirfareSearch::City &_r);
        SearchServiceImpl::Route getRoute(const AirfareSearch::SearchRoute &_r);

        AirfareSearch::City *city(const SearchServiceImpl::City &city);
        AirfareSearch::Cabin cabin(const SearchServiceImpl::Cabin &cabin);
    }; // namespace DataBuilder

}; // namespace AirfareSearch