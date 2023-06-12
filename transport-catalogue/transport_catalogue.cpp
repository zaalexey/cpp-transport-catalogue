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

    const std::set<std::string_view>& TransportCatalogue::FindBusesOnStop(std::string_view stop_name) const {
        return stop_to_buses_.at(stop_name);
    }

    const std::map<std::string_view, const Bus*> TransportCatalogue::GetSortedBuses() const {
        std::map<std::string_view, const Bus*> result;
        for (const auto& bus : busname_to_bus_) {
            result.emplace(bus);
        }
        return result;
    }

} //namespace transport