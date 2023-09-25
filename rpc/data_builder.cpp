#include "data_builder.h"

namespace AirfareSearch {

    SearchService::Request DataBuilder::request(SearchRequest req_) {
        SearchService::Request req;
        req.people = req_.people();
        req.maxResults = req_.maxresults();
        for (const auto &i : req_.routes()) {
            req.routes.emplace_back(this->getRoute(i));
        }

        for (const auto &i : req_.agencies()) {
            req.agencies.emplace_back(i);
        }

        return req;
    }

    void DataBuilder::bindResponse(SearchResponse res_, SearchService::Response res) {
        for (const auto &i : res.data) {
            auto data = res_.add_data();
            data->set_price(i.price);
            for (const auto &agency : i.agencies) {
                data->add_agencies(agency);
            }
            for (const auto &flight : i.flights) {
                auto flight_ = data->add_flights();
                this->bindFlight(flight_, flight);
            }
        }
    }

    void DataBuilder::bindFlight(AirfareSearch::Flight *flight_, const SearchService::Flight &flight) {
        flight_->set_carrier(flight.carrier);
        flight_->set_flightno(flight.flightNo);
        flight_->set_allocated_departure(this->city(flight.departure));
        flight_->set_allocated_arrival(this->city(flight.arrival));
        flight_->set_departuredatetime(flight.departureDatetime);
        flight_->set_arrivaldatetime(flight.arrivalDatetime);

        for (auto c : flight.cabins) {
            flight_->add_cabins(c);
        }
    }

} // namespace AirfareSearch
