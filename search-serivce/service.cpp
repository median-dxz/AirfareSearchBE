#include "service.h"
#include "str_utils.h"

using AirfareSearch::SearchServiceImpl;

SearchServiceImpl::Response SearchServiceImpl::search(SearchServiceImpl::Request req) {
    Database::Storage stroage = Database::getStroage();

    SearchServiceImpl::Response res;
    SearchServiceImpl::FlightResult fr1, fr2, fr3;

    fr1.agencies = vector<string>{"BJS001", "CAN001"};
    fr2.agencies = vector<string>{"BJS001"};
    fr3.agencies = vector<string>{"CAN002"};

    fr1.price = 3200;
    fr2.price = 4000;
    fr3.price = 5000;

    SearchServiceImpl::City c1 = {"中国", "AAA"};
    SearchServiceImpl::City c2 = {"TW", "BBB"};

    SearchServiceImpl::Flight f1, f2, f3;

    f1 = {"AA", "1232", c1, c2, "201405051340", "201405051340", {Cabin::C, Cabin::Y}};
    f2 = {"BB", "1232", c1, c2, "201405051340", "201405051340", {Cabin::C, Cabin::Y}};
    f3 = {"CC", "1232", c1, c2, "201405051340", "201405051340", {Cabin::C, Cabin::Y}};

    fr1.flights = vector<SearchServiceImpl::Flight>{f1, f2};
    fr2.flights = vector<SearchServiceImpl::Flight>{f1, f3};
    fr3.flights = vector<SearchServiceImpl::Flight>{f3, f2};

    res.data = vector<SearchServiceImpl::FlightResult>{fr1, fr2, fr3};

    return res;
}
