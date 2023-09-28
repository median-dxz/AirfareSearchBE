#include "data_builder.h"

using namespace AirfareSearch;

SearchServiceImpl::Request DataBuilder::request(SearchRequest req_) {
    SearchServiceImpl::Request req;
    req.people = req_.people();
    req.maxResults = req_.maxresults();
    for (const auto &i : req_.routes()) {
        req.routes.emplace_back(getRoute(i));
    }

    for (const auto &i : req_.agencies()) {
        req.agencies.emplace_back(i);
    }

    return req;
}

void DataBuilder::bindResponse(SearchResponse *res_, SearchServiceImpl::Response res) {
    res_->clear_data();
    for (const auto &i : res.data) {
        auto data = res_->add_data();
        data->set_price(i.price);
        for (const auto &agency : i.agencies) {
            data->add_agencies(agency);
        }
        for (const auto &flight : i.flights) {
            auto flight_ = data->add_flights();
            bindFlight(flight_, flight);
        }
    }
}

void DataBuilder::bindFlight(AirfareSearch::Flight *flight_, const SearchServiceImpl::Flight &flight) {
    flight_->set_carrier(flight.carrier);
    flight_->set_flightno(flight.flightNo);

    auto departure = new AirfareSearch::City();
    auto arrival = new AirfareSearch::City();
    departure->set_code(flight.departure);
    arrival->set_code(flight.arrival);

    flight_->set_allocated_departure(departure);
    flight_->set_allocated_arrival(arrival);
    flight_->set_departuredatetime(flight.departureDatetime);
    flight_->set_arrivaldatetime(flight.arrivalDatetime);

    for (auto c : flight.cabins) {
        flight_->add_cabins(cabin(c));
    }
}

SearchServiceImpl::City DataBuilder::getCity(const AirfareSearch::City &_r) {
    return SearchServiceImpl::City{_r.name(), _r.code()};
}

SearchServiceImpl::Route DataBuilder::getRoute(const AirfareSearch::SearchRoute &_r) {
    return SearchServiceImpl::Route{_r.id(), getCity(_r.departure()), getCity(_r.arrival()), _r.departuredate()};
}

AirfareSearch::Cabin DataBuilder::cabin(const Database::Cabin &cabin) {
    switch (cabin) {
    case Database::Cabin::F:
        return AirfareSearch::Cabin::F;
        break;
    case Database::Cabin::Y:
        return AirfareSearch::Cabin::Y;
        break;
    case Database::Cabin::C:
        return AirfareSearch::Cabin::C;
        break;
    default:
        spdlog::error("Error Cabin Enum Type");
        throw new std::bad_cast();
    }
}
