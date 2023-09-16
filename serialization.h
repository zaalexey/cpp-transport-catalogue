#pragma once



#include "json.h"
#include "transport_catalogue.h"
#include "request_handler.h"

#include <transport_catalogue.pb.h>
#include <svg.pb.h>
#include <map_renderer.pb.h>

namespace serialization {

void Serialize(const transport::TransportCatalogue& db, const renderer::MapRenderer& renderer, const transport::Router& router, std::ostream& out);
std::tuple<transport::TransportCatalogue, renderer::MapRenderer, transport::Router, graph::DirectedWeightedGraph<double>, std::map<std::string, graph::VertexId>> Deserialize(std::istream& input);


void SerializeStops(const transport::TransportCatalogue& db, serialization_transport::TransportCatalogue& proto_db);
void SerializeStopDistances(const transport::TransportCatalogue& db, serialization_transport::TransportCatalogue& proto_db);
void SerializeBuses(const transport::TransportCatalogue& db, serialization_transport::TransportCatalogue& proto_db);
void SerializeRenderSettings(const renderer::MapRenderer& renderer, serialization_transport::TransportCatalogue& proto_db);
serialization_map::Point SerializePoint(const svg::Point& point);
serialization_map::Color SerializeColor(const svg::Color& color);
serialization_map::Rgb SerializeRgb(const svg::Rgb& rgb);
serialization_map::Rgba SerializeRgba(const svg::Rgba& rgba);
void SerializeRouter(const transport::Router& router, serialization_transport::TransportCatalogue& proto_db);
serialization_transport::RouterSettings SerializeRouterSettings(const transport::Router& router, serialization_transport::TransportCatalogue& proto_db);
serialization_graph::Graph SerializeGraph(const transport::Router& router, serialization_transport::TransportCatalogue& proto_db);

void DeserializeStops(transport::TransportCatalogue& db, const serialization_transport::TransportCatalogue& proto_db);
void DeserializeStopDistances(transport::TransportCatalogue& db, const serialization_transport::TransportCatalogue& proto_db);
void DeserializeBuses(transport::TransportCatalogue& db, const serialization_transport::TransportCatalogue& proto_db);
renderer::MapRenderer DeserializeRenderSettings(renderer::RenderSettings& render_settings, const serialization_transport::TransportCatalogue& proto_db);
svg::Point DeserializePoint(const serialization_map::Point& proto_point);
svg::Color DeserializeColor(const serialization_map::Color& proto_color);
transport::Router DeserializeRouterSettings(const serialization_transport::TransportCatalogue& proto_db);
graph::DirectedWeightedGraph<double> DeserializeGraph(const serialization_transport::TransportCatalogue& proto_db);
std::map<std::string, graph::VertexId> DeserializeStopIds(const serialization_transport::TransportCatalogue& proto_db);

} // serialization