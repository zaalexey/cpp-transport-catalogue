#include "transport_catalogue.h"

namespace transport {
    using namespace geo;

    void TransportCatalogue::AddRoute(const Bus& bus) {
        buses_.push_back({ bus.number, bus.stops, bus.circular_route });
        for (const auto& bus_stops : buses_.back().stops) {
            stop_to_buses_[bus_stops->name].insert(buses_.back().number);
        }
        busname_to_bus_[buses_.back().number] = &buses_.back();
    }

    void TransportCatalogue::AddStop(const Stop& stop) {
        stops_.push_back({ stop.name, stop.coordinates });
        stop_to_buses_[stops_.back().name];
        stopname_to_stop_[stops_.back().name] = &stops_.back();
    }

    const Bus* TransportCatalogue::FindRoute(const std::string_view route_number) const {
        return busname_to_bus_.count(route_number) ? busname_to_bus_.at(route_number) : nullptr;
    }

    const Stop* TransportCatalogue::FindStop(const std::string_view stop_name) const {
        return stopname_to_stop_.count(stop_name) ? stopname_to_stop_.at(stop_name) : nullptr;
    }

    const RouteInfo TransportCatalogue::RouteInformation(const std::string_view route_number) const {
        RouteInfo route_info{};
        const Bus* bus = FindRoute(route_number);

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
                route_length += GetDistance(from, to);
                geographic_length += ComputeDistance(from->coordinates, to->coordinates);
            }
            else {
                route_length += GetDistance(from, to) + GetDistance(to, from);;
                geographic_length += ComputeDistance(from->coordinates, to->coordinates) * 2;
            }
        }

        route_info.unique_stops_count = UniqueStopsCount(route_number);
        route_info.route_length = route_length;
        route_info.curvature = route_length / geographic_length;

        return route_info;
    }

    const std::unordered_map<std::string_view, std::set<std::string_view>>& TransportCatalogue::GetBusesOnStop() const {
        return stop_to_buses_;
    }

    size_t TransportCatalogue::UniqueStopsCount(const std::string_view route_number) const {
        std::unordered_set<std::string_view> unique_stops;
        for (const auto& stop : busname_to_bus_.at(route_number)->stops) {
            unique_stops.insert(stop->name);
        }
        return unique_stops.size();
    }

    void TransportCatalogue::SetDistance(const Stop* from, const Stop* to, const double distance) {
        stop_distances_[{from, to}] = distance;
    }

    double TransportCatalogue::GetDistance(const Stop* from, const Stop* to) const {
        if (stop_distances_.count({ from, to })) {
            return stop_distances_.at({ from, to });
        }
        else if (stop_distances_.count({ to, from })) {
            return stop_distances_.at({ to, from });
        }
        else
            return 0;
    }
} //namespace transport