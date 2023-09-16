#pragma once

#include "transport_catalogue.h"

#include <iostream>
#include <iomanip>

namespace output {
	using namespace transport;
	void ProcessRequests(std::istream& in, std::ostream& out, TransportCatalogue& catalogue);
	void PrintStopsInformation(std::ostream& out, std::string_view stop_name, TransportCatalogue& catalogue);
	void PrintRouteInformation(std::ostream& out, std::string_view route_number, TransportCatalogue& catalogue);
}