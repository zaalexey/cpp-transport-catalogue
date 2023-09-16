#pragma once

#include "geo.h"
#include "domain.h"

#include <deque>
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <stdexcept>
#include <unordered_set>
#include <set>
#include <iostream>

namespace transport {
    using namespace geo;

    class TransportCatalogue {
    public:
        struct StopDistancesHasher {
            size_t operator()(const std::pair<const Stop*, const Stop*>& points) const {
                size_t hash_first = std::hash<const void*>{}(points.first);
                size_t hash_second = std::hash<const void*>{}(points.second);
                return hash_first + hash_second * 37;
            }
        };
        void AddRoute(const Bus& bus);
        void AddStop(const Stop& stop);
        const Bus* FindRoute(const std::string_view route_number) const;
        const Stop* FindStop(const std::string_view stop_name) const;
        size_t UniqueStopsCount(const std::string_view route_number) const;
        void SetDistance(const Stop* from, const Stop* to, const double distance);
        double GetDistance(const Stop* from, const Stop* to) const;
        const std::set<std::string_view>& FindBusesOnStop(std::string_view stop_name) const;
        const std::map<std::string_view, const Bus*> GetSortedBuses() const;
        const std::map<std::string_view, const Stop*> GetSortedStops() const;
        const std::unordered_map<std::pair<const Stop*, const Stop*>, double, StopDistancesHasher> GetStopDistances() const;

    private:
        std::deque<Bus> buses_;
        std::deque<Stop> stops_;
        std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
        std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;
        std::unordered_map<std::string_view, std::set<std::string_view>> stop_to_buses_;
        std::unordered_map<std::pair<const Stop*, const Stop*>, double, StopDistancesHasher> stop_distances_;
    };
} //namespace transport