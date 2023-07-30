#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "router.h"
#include "transport_router.h"

#include <sstream>

using namespace transport;
using namespace geo;
using namespace json;
using namespace renderer;
using namespace graph;

class RequestHandler {
public:
    RequestHandler(TransportCatalogue& db, const MapRenderer& renderer, const transport::Router& router)
        : db_(db)
        , renderer_(renderer)
        , router_(router)
    {}


    svg::Document RenderMap() const;

    const RouteInfo RouteInformation(const std::string_view route_number) const;
    const std::set<std::string_view>& GetBusesOnStop(std::string_view stop_name) const;
    bool IsBusNumber(const std::string_view bus_number) const;
    bool IsStopName(const std::string_view stop_name) const;

    const std::optional<graph::Router<double>::RouteInfo> GetOptimalRoute(const std::string_view stop_from, const std::string_view stop_to) const;
    const graph::DirectedWeightedGraph<double>& GetRouterGraph() const;

    const vector<RouteWeight>& GetRouterWeight() const;
 

private:
    const TransportCatalogue& db_;
    const MapRenderer& renderer_;
    const transport::Router& router_;
};