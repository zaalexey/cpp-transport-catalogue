#include "json_reader.h"
#include "json_builder.h"

#include <string>
#include <vector>
#include <cmath>


using namespace transport;
using namespace geo;
using namespace json;
using namespace renderer;

const Node& JsonReader::GetBaseRequests() const {
    if (!input_.GetRoot().AsDict().count("base_requests"))
        return nullptr_;
    return input_.GetRoot().AsDict().at("base_requests");
}

const Node& JsonReader::GetStatRequests() const {
    if (!input_.GetRoot().AsDict().count("stat_requests"))
        return nullptr_;
    return input_.GetRoot().AsDict().at("stat_requests");
}

const Node& JsonReader::GetRenderSettings() const {
    if (!input_.GetRoot().AsDict().count("render_settings"))
        return nullptr_;
    return input_.GetRoot().AsDict().at("render_settings");
}
const Node& JsonReader::GetRoutingSettings() const {
    if (!input_.GetRoot().AsDict().count("routing_settings"s))
        return nullptr_;
    return input_.GetRoot().AsDict().at("routing_settings"s);
}

void JsonReader::ProcessRequests(const Node& stat_requests, RequestHandler& rh) const {
    Array result;
    for (auto& stat_request : stat_requests.AsArray()) {
        const auto& request = stat_request.AsDict();
        const auto& type = request.at("type").AsString();
        if (type == "Stop") {
            result.push_back(PrintStopsInformation(request, rh).AsDict());
        }
        if (type == "Bus") {
            result.push_back(PrintRouteInformation(request, rh).AsDict());
        }
        if (type == "Map") {
            result.push_back(PrintMapInformation(request, rh).AsDict());
        }
        if (type == "Route") {
            result.push_back(PrintRoutingInformation(request, rh).AsDict());
        }
    }
    Print(json::Document{ result }, std::cout);
}


void JsonReader::LoadTransportCatalogue(TransportCatalogue& catalogue) {
    const Array& arr = GetBaseRequests().AsArray();
    for (auto& base_request : arr) {
        const auto& request = base_request.AsDict();
        const auto& type = request.at("type").AsString();
        if (type == "Stop") {
            catalogue.AddStop(GetStopFromRequest(request));
        }
    }

    for (auto& base_request : arr) {
        const auto& request = base_request.AsDict();
        const auto& type = request.at("type").AsString();
        if (type == "Stop") {
            ParseDistance(request, catalogue);
        }
    }

    for (auto& base_request : arr) {
        const auto& request = base_request.AsDict();
        const auto& type = request.at("type").AsString();
        if (type == "Bus") {
            catalogue.AddRoute(GetBusFromRequest(request, catalogue));
        }
    }
}

Stop JsonReader::GetStopFromRequest(const Dict& request) {
    Stop result;
    result.name = request.at("name").AsString();
    result.coordinates.lat = request.at("latitude").AsDouble();
    result.coordinates.lng = request.at("longitude").AsDouble();

    return result;
}

Bus JsonReader::GetBusFromRequest(const Dict& request, TransportCatalogue& catalogue) {
    Bus result;
    result.number = request.at("name").AsString();
    for (auto& stop : request.at("stops").AsArray()) {
         result.stops.push_back(catalogue.FindStop(stop.AsString()));
    }
    result.circular_route = request.at("is_roundtrip").AsBool();

    return result;
}

void JsonReader::ParseDistance(const Dict& request, TransportCatalogue& catalogue) {

    auto from = catalogue.FindStop(request.at("name").AsString());
    auto& distances = request.at("road_distances").AsDict();

    for (auto& [to_name, dist] : distances) {
        auto to = catalogue.FindStop(to_name);
        catalogue.SetDistance(from, to, dist.AsDouble());
        if (!catalogue.GetDistance(from, to)) {
            catalogue.SetDistance(to, from, dist.AsDouble());
        }
    }
}

MapRenderer JsonReader::LoadRenderSettings(const Dict& request_map) const {
    RenderSettings render_settings;
    render_settings.width = request_map.at("width").AsDouble();
    render_settings.height = request_map.at("height").AsDouble();
    render_settings.padding = request_map.at("padding").AsDouble();
    render_settings.stop_radius = request_map.at("stop_radius").AsDouble();
    render_settings.line_width = request_map.at("line_width").AsDouble();
    render_settings.bus_label_font_size = request_map.at("bus_label_font_size").AsInt();

    const Array& bus_label_offset = request_map.at("bus_label_offset").AsArray();
    render_settings.bus_label_offset = { bus_label_offset[0].AsDouble(), bus_label_offset[1].AsDouble() };

    render_settings.stop_label_font_size = request_map.at("stop_label_font_size").AsInt();

    const Array& stop_label_offset = request_map.at("stop_label_offset").AsArray();
    render_settings.stop_label_offset = { stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble() };


    if (request_map.at("underlayer_color").IsString()) 
        render_settings.underlayer_color = request_map.at("underlayer_color").AsString();
    else if (request_map.at("underlayer_color").IsArray()) {
        const Array& color = request_map.at("underlayer_color").AsArray();
        render_settings.underlayer_color = ReadColorAsArray(color);
    }
    else throw std::logic_error("wrong underlayer color");

    render_settings.underlayer_width = request_map.at("underlayer_width").AsDouble();

    const Array& color_palette = request_map.at("color_palette").AsArray();
    for (const auto& color_element : color_palette) {
        if (color_element.IsString())
            render_settings.color_palette.push_back(color_element.AsString());
        else if (color_element.IsArray()) {
            const Array& color_type = color_element.AsArray();
            render_settings.color_palette.push_back(ReadColorAsArray(color_type));
        }
        else throw std::logic_error("wrong color_palette");
    }

    return render_settings;
}

transport::Router JsonReader::LoadRoutingSettings(const json::Node& settings) const {
    transport::Router routing_settings;
    return transport::Router{ settings.AsDict().at("bus_wait_time"s).AsInt(), settings.AsDict().at("bus_velocity"s).AsDouble() };
}

const Node JsonReader::PrintRouteInformation(const Dict& request, RequestHandler& rh) const {
    Node result;
    const std::string& route_number = request.at("name").AsString();
    const int id = request.at("id").AsInt();

    if (!rh.IsBusNumber(route_number)) {
        result = Builder{}
            .StartDict()
            .Key("request_id").Value(id)
            .Key("error_message").Value(static_cast<std::string>("not found"))
            .EndDict()
            .Build();
    }
    else {
        const auto& route_info = rh.RouteInformation(route_number);
        result = Builder{}
            .StartDict()
            .Key("request_id").Value(id)
            .Key("curvature").Value(route_info.curvature)
            .Key("route_length").Value(route_info.route_length)
            .Key("stop_count").Value(static_cast<int>(route_info.stops_count))
            .Key("unique_stop_count").Value(static_cast<int>(route_info.unique_stops_count))
            .EndDict()
            .Build();
    }
    return result;
}

const Node JsonReader::PrintStopsInformation(const Dict& request, RequestHandler& rh) const {
    Node result;
    const std::string& stop_name = request.at("name").AsString();
    const int id = request.at("id").AsInt();

    if (!rh.IsStopName(stop_name)) {
        result = Builder{}
            .StartDict()
            .Key("request_id").Value(id)
            .Key("error_message").Value(static_cast<std::string>("not found"))
            .EndDict()
            .Build();
    }
    else {
        Array buses;
        for (const auto& bus : rh.GetBusesOnStop(stop_name)) {
            buses.push_back(std::string(bus));
        }
        result = Builder{}
            .StartDict()
            .Key("request_id").Value(id)
            .Key("buses").Value(buses)
            .EndDict()
            .Build();
    }
    return result;
}

const Node JsonReader::PrintMapInformation(const Dict& request, RequestHandler& rh) const {
    Node result;
    const int id = request.at("id").AsInt();
    std::ostringstream strm;
    svg::Document map = rh.RenderMap();
    map.Render(strm);

    result = Builder{}
        .StartDict()
        .Key("request_id").Value(id)
        .Key("map").Value(strm.str())
        .EndDict()
        .Build();

    return result;
}

const Node JsonReader::PrintRoutingInformation(const Dict& request_map, RequestHandler& rh) const {
    Node result;
    const int id = request_map.at("id"s).AsInt();
    const string_view stop_from = request_map.at("from"s).AsString();
    const string_view stop_to = request_map.at("to"s).AsString();
    const auto& routing = rh.GetOptimalRoute(stop_from, stop_to);

    if (!routing) {
        result = json::Builder{}
            .StartDict()
            .Key("request_id"s).Value(id)
            .Key("error_message"s).Value("not found"s)
            .EndDict()
            .Build();
    }
    else {
        Array items;
        double total_time = 0.0;
        items.reserve(routing.value().edges.size());
        for (auto& edge_id : routing.value().edges) {
            const graph::Edge<double> edge = rh.GetRouterGraph().GetEdge(edge_id);
            const auto& router_weight = rh.GetRouterWeight();
            
            if (router_weight[edge_id].span_count == 0) {
                items.emplace_back(json::Node(json::Builder{}
                    .StartDict()
                    .Key("stop_name"s).Value(router_weight[edge_id].name)
                    .Key("time"s).Value(edge.weight)
                    .Key("type"s).Value("Wait"s)
                    .EndDict()
                    .Build()));

                total_time += edge.weight;
            }
            else {
                items.emplace_back(json::Node(json::Builder{}
                    .StartDict()
                    .Key("bus"s).Value(router_weight[edge_id].name)
                    .Key("span_count"s).Value(static_cast<int>(router_weight[edge_id].span_count))
                    .Key("time"s).Value(edge.weight)
                    .Key("type"s).Value("Bus"s)
                    .EndDict()
                    .Build()));

                total_time += edge.weight;
            }
        }

        result = Builder{}
            .StartDict()
            .Key("request_id"s).Value(id)
            .Key("total_time"s).Value(total_time)
            .Key("items"s).Value(items)
            .EndDict()
            .Build();
    }

    return result;
}

const Color JsonReader::ReadColorAsArray(const Array& underlayer_color) const {
    if (underlayer_color.size() == 3) {
        return Rgb(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt());
    }
    else if (underlayer_color.size() == 4) {
        return Rgba(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt(), underlayer_color[3].AsDouble());
    }
    else {
        return svg::NoneColor;
    }
}
