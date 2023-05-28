#include "stat_reader.h"


namespace output {
    void ProcessRequests(std::istream& in, std::ostream& out, TransportCatalogue& catalogue) {
        size_t requests_count;
        in >> requests_count;
        for (size_t i = 0; i < requests_count; ++i) {
            std::string keyword, line;
            in >> keyword;
            std::getline(in, line);
            if (keyword == "Bus") {
                std::string route_number = line.substr(1, line.npos);
                PrintRouteInformation(out, route_number, catalogue);
            }
            if (keyword == "Stop") {
                std::string stop_name = line.substr(1, line.npos);
                PrintStopsInformation(out, stop_name, catalogue);
            }
        }
    }

    void PrintStopsInformation(std::ostream& out, std::string_view stop_name, TransportCatalogue& catalogue) {
        const auto& stops = catalogue.GetBusesOnStop();

        out << "Stop " << stop_name << ": ";
        if (stops.find(stop_name) == stops.end()) {
            out << "not found\n";
            return;
        }

        const auto& stop = stops.at(stop_name);

        if (stop.size() == 0) {
            out << "no buses\n";
            return;
        }
        else {
            out << "buses ";
            for (const auto& bus : stop) {
                out << bus << " ";
            }
            out << "\n";
        }
    }

    void PrintRouteInformation(std::ostream& out, std::string_view route_number, TransportCatalogue& catalogue) {
        if (catalogue.FindRoute(route_number)) {
            const auto& result = catalogue.RouteInformation(route_number);
            out << "Bus " << route_number << ": " << result.stops_count << " stops on route, "
                << result.unique_stops_count << " unique stops, " << std::setprecision(6)
                << result.route_length << " route length, "
                << result.curvature << " curvature\n";
        }
        else {
            out << "Bus " << route_number << ": not found\n";
        }
    }
}