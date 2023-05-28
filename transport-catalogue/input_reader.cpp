#include "input_reader.h"


#include <string>
#include <vector>
#include <cmath>

namespace input {
    using namespace transport;
    using namespace geo;

    void FillTransportCatalogue(std::istream& in, TransportCatalogue& catalogue) {
        size_t requests_count;
        std::vector<std::string> query_bus;
        std::vector<std::string> query_stop;
        std::vector<std::string> query_stop_to_distans;
        in >> requests_count;
        for (size_t i = 0; i < requests_count; ++i) {
            std::string query, request;
            in >> query;
            std::getline(in, request);
            if (query == "Stop") {
                query_stop.push_back(move(request));
            }
            else if (query == "Bus") {
                query_bus.push_back(move(request));
            }
        }
        query_stop_to_distans = query_stop;
        for (auto& query : query_stop) {
            catalogue.AddStop(GetStopFromRequest(move(query)));
        }

        for (auto& query : query_stop_to_distans) {
            if (query.find("m to ") != query.npos)
                ParseDistance(move(query), catalogue);
        }

        for (auto& query : query_bus) {
            catalogue.AddRoute(GetBusFromRequest(move(query), catalogue));
        }
    }

    Bus GetBusFromRequest(std::string_view request, TransportCatalogue& catalogue) {
        Bus result;
        auto sep = request.find_first_of(':');
        result.number = RemoveSpaces(request.substr(0, sep));
        request.remove_prefix(sep + 1);

        auto pos = request.find('>') != request.npos ? '>' : '-';
        while (request.find_first_of(pos) != request.npos) {
            auto stop = catalogue.FindStop(RemoveSpaces(request.substr(0, request.find_first_of(pos))));
            result.stops.push_back(stop);
            request.remove_prefix(request.find_first_of(pos) + 1);
        }
        auto stop = catalogue.FindStop(RemoveSpaces(request));
        result.stops.push_back(stop);
        if (pos == '>')
            result.circular_route = true;
        else
            result.circular_route = false;

        return result;
    }

    Stop GetStopFromRequest(std::string_view request) {
        Stop result;
        auto sep = request.find_first_of(':');
        result.name = RemoveSpaces(request.substr(0, sep));
        request.remove_prefix(sep);

        double lat = std::stod(std::string(request.substr(request.find_first_of(':') + 2, 9)));
        double lng = std::stod(std::string(request.substr(request.find_first_of(',') + 2, 9)));

        result.coordinates.lat = lat;
        result.coordinates.lng = lng;

        return result;
    }

    void ParseDistance(std::string_view request, TransportCatalogue& catalogue) {
        auto sep = request.find_first_of(':');
        const Stop* from = catalogue.FindStop(RemoveSpaces(request.substr(0, sep)));

        sep = request.find(',');
        request.remove_prefix(sep + 1);
        sep = request.find(',');
        request.remove_prefix(sep + 1);

        while (1) {
            auto sep = request.find_first_of('m');
            double distance = std::stod(std::string(request.substr(0, sep)));
            request.remove_prefix(sep + 5);
            sep = request.find(',');
            const Stop* to = catalogue.FindStop(request.substr(0, sep));
            catalogue.SetDistance(from, to, distance);
            if (!catalogue.GetDistance(from, to)) {
                catalogue.SetDistance(to, from, distance);
            }

            if (sep == request.npos) {
                break;
            }
            else {
                request.remove_prefix(sep + 1);
            }
        }

    }


    std::string RemoveSpaces(std::string_view str) {
        str.remove_prefix(str.find_first_not_of(' '));
        return std::string(str.substr(0, str.find_last_not_of(' ') + 1));
    }

} //namespace input