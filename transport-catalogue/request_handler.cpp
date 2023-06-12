#include "request_handler.h"

const RouteInfo RequestHandler::RouteInformation(const std::string_view route_number) const {
    RouteInfo route_info{};
    const Bus* bus = db_.FindRoute(route_number);

    if (!bus) {
        throw std::invalid_argument("bus not found");
    }
    if (bus->circular_route) {
        route_info.stops_count = bus->stops.size();
    }
    else {
        route_info.stops_count = bus->stops.size() * 2 - 1;
    }

    double route_length = 0.0;
    double geographic_length = 0.0;

    for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
        const auto from = bus->stops[i];
        const auto to = bus->stops[i + 1];
        if (bus->circular_route) {
            route_length += db_.GetDistance(from, to);
            geographic_length += ComputeDistance(from->coordinates, to->coordinates);
        }
        else {
            route_length += db_.GetDistance(from, to) + db_.GetDistance(to, from);;
            geographic_length += ComputeDistance(from->coordinates, to->coordinates) * 2;
        }
    }

    route_info.unique_stops_count = db_.UniqueStopsCount(route_number);
    route_info.route_length = route_length;
    route_info.curvature = route_length / geographic_length;

    return route_info;
}

svg::Document RequestHandler::RenderMap() const {
    return renderer_.RenderMap(db_.GetSortedBuses());
}

bool RequestHandler::IsBusNumber(const std::string_view bus_number) const {
    return db_.FindRoute(bus_number);
}

bool RequestHandler::IsStopName(const std::string_view stop_name) const {
    return db_.FindStop(stop_name);
}
const std::set<std::string_view>& RequestHandler::GetBusesOnStop(std::string_view stop_name) const {
    return db_.FindBusesOnStop(stop_name);
}