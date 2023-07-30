#include <cassert>
#include <sstream>

#include "request_handler.h"
#include "json_reader.h"

using namespace json;
using namespace renderer;
using namespace transport;


int main() {

    TransportCatalogue catalogue;
    JsonReader json_doc(std::cin);

    json_doc.LoadTransportCatalogue(catalogue);

    const auto& stat_requests = json_doc.GetStatRequests();
    const auto& render_settings = json_doc.GetRenderSettings().AsDict();
    const auto& renderer = json_doc.LoadRenderSettings(render_settings);

    const auto& routing_settings = json_doc.LoadRoutingSettings(json_doc.GetRoutingSettings());
    const transport::Router router = { routing_settings, catalogue };

    RequestHandler rh(catalogue, renderer, router);
    json_doc.ProcessRequests(stat_requests, rh);


}