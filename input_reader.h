#pragma once

#include "geo.h"
#include "transport_catalogue.h"

#include <iostream>
namespace input {
	using namespace transport;
	using namespace geo;

	void FillTransportCatalogue(std::istream& in, TransportCatalogue& catalogue);
	Bus GetBusFromRequest(std::string_view request, TransportCatalogue& catalogue);
	Stop GetStopFromRequest(std::string_view request);
	void ParseDistance(std::string_view request, TransportCatalogue& catalogue);
	std::string RemoveSpaces(std::string_view str);
}