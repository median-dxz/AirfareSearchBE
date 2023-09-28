#pragma once

#include <spdlog/spdlog.h>

#include "protos/SearchRequest.grpc.pb.h"
#include "service.h"

namespace DataBuilder {
    SearchServiceImpl::Request request(AirfareSearch::SearchRequest req);

    void bindResponse(AirfareSearch::SearchResponse &res_, SearchServiceImpl::Response res);
    void bindFlight(AirfareSearch::Flight *flight_, const SearchServiceImpl::Flight &flight);

    SearchServiceImpl::City getCity(const AirfareSearch::City &_r);
    SearchServiceImpl::Route getRoute(const AirfareSearch::SearchRoute &_r);

    AirfareSearch::Cabin cabin(const Database::Cabin &cabin);
}; // namespace DataBuilder
