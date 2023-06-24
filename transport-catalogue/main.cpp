#include <cassert>
#include <sstream>

#include "request_handler.h"
#include "json_reader.h"

using namespace json;
using namespace renderer;


int main() {

    TransportCatalogue catalogue;
    JsonReader json_doc(std::cin);

    json_doc.LoadTransportCatalogue(catalogue);

    const auto& stat_requests = json_doc.GetStatRequests();
    const auto& render_settings = json_doc.GetRenderSettings().AsDict();
    const auto& renderer = json_doc.LoadRenderSettings(render_settings);

    RequestHandler rh(catalogue, renderer);
    json_doc.ProcessRequests(stat_requests, rh);


}