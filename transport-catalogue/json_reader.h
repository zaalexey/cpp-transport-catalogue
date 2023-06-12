#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "request_handler.h"

#include <iostream>

using namespace transport;
using namespace geo;
using namespace json;
using namespace renderer;

class JsonReader {
public:
    JsonReader(std::istream& input)
        : input_(Load(input))
    {}

    const Node& GetBaseRequests() const;
    const Node& GetStatRequests() const;
    const Node& GetRenderSettings() const;

    void ProcessRequests(const Node& stat_requests, RequestHandler& rh) const;

    void LoadTransportCatalogue(TransportCatalogue& catalogue);
    MapRenderer LoadRenderSettings(const Dict& request_map) const;


    const Node PrintRouteInformation(const Dict& request, RequestHandler& rh) const;
    const Node PrintStopsInformation(const Dict& request, RequestHandler& rh) const;
    const Node PrintMapInformation(const Dict& request, RequestHandler& rh) const;


private:
    json::Document input_;
    Node nullptr_ = nullptr;

    Bus GetBusFromRequest(const Dict& request, TransportCatalogue& catalogue);
    Stop GetStopFromRequest(const Dict& request);
    void ParseDistance(const Dict& request, TransportCatalogue& catalogue);
    const Color ReadColorAsArray(const Array& underlayer_color) const;
};