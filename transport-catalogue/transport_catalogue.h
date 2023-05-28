#pragma once

#include "geo.h"

#include <deque>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <unordered_set>
#include <set>

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
        const RouteInfo RouteInformation(const std::string_view route_number) const;
        const std::unordered_map<std::string_view, std::set<std::string_view>>& GetBusesOnStop() const;
        size_t UniqueStopsCount(const std::string_view route_number) const;
        void SetDistance(const Stop* from, const Stop* to, const double distance);
        double GetDistance(const Stop* from, const Stop* to) const;

    private:
        std::deque<Bus> buses_;
        std::deque<Stop> stops_;
        std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
        std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;
        std::unordered_map<std::string_view, std::set<std::string_view>> stop_to_buses_;
        std::unordered_map<std::pair<const Stop*, const Stop*>, double, StopDistancesHasher> stop_distances_;
    };
} //namespace transport