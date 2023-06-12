#pragma once

#include "geo.h"

#include <string>
#include <vector>


namespace transport {
    using namespace geo;

    struct Stop {
        std::string name;
        Coordinates coordinates;
    };

    struct Bus {
        std::string number;
        std::vector<const Stop*> stops;
        bool circular_route;
    };

    struct RouteInfo {
        size_t stops_count;
        size_t unique_stops_count;
        double route_length;
        double curvature;
    };
}