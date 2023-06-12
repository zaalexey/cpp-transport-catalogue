#pragma once


#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

#include <sstream>

using namespace transport;
using namespace geo;
using namespace json;
using namespace renderer;

class RequestHandler {
public:
    RequestHandler(TransportCatalogue& db, const MapRenderer& renderer)
        : db_(db)
        , renderer_(renderer)
    {}


    svg::Document RenderMap() const;

    const RouteInfo RouteInformation(const std::string_view route_number) const;
    const std::set<std::string_view>& GetBusesOnStop(std::string_view stop_name) const;
    bool IsBusNumber(const std::string_view bus_number) const;
    bool IsStopName(const std::string_view stop_name) const;
 

private:
    const TransportCatalogue& db_;
    const MapRenderer& renderer_;
};