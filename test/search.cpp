#include <chrono>
#include <filesystem>
#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <vector>

#include "service.h"
#include "str_utils.h"

#include <spdlog/spdlog.h>

using namespace std;
namespace fs = filesystem;

const string DEFAULT_DB_FILE = fs::absolute(fs::path("/var") / "as_data" / "as.db");

int main() {
    spdlog::set_level(spdlog::level::debug);

    auto &storage = Database::getStorage(DEFAULT_DB_FILE);
    auto &service = SearchServiceImpl::getInstance();

    SearchServiceImpl::Request req;

    req.people = 3;
    req.maxResults = 3;
    req.agencies = vector<string>({"HBR001", "PKX001"});
    req.routes = vector<SearchServiceImpl::Route>();

    SearchServiceImpl::City SHA = {"上海", "SHA"};
    SearchServiceImpl::City NKG = {"南京", "NKG"};
    SearchServiceImpl::City BJS = {"北京", "BJS"};

    req.routes.push_back(SearchServiceImpl::Route{1, SHA, NKG, "20240102"});
    req.routes.push_back(SearchServiceImpl::Route{2, BJS, SHA, "20240103"});
    req.routes.push_back(SearchServiceImpl::Route{3, NKG, SHA, "20240104"});

    auto res = service.search(req);
    const auto &result_set = res.data;

    for (int i = 0; i < result_set.size(); i++) {
        const auto &re = result_set[i];
        spdlog::info("result index: {}", i);
        spdlog::info("agencies: {} price: {}", fmt::join(re.agencies, ", "), re.price);
        for (const auto &f : re.flights) {
            spdlog::info("{} {} {} {} {} {}", f.carrier, f.flightNo, f.departure, f.arrival, f.departureDatetime,
                         f.arrivalDatetime);
        }
    }

    return 0;
}