#include <cassert>
#include <sstream>

#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

using namespace transport;
using namespace input;
using namespace output;

int main() {

    TransportCatalogue catalogue;
    FillTransportCatalogue(std::cin, catalogue);
    ProcessRequests(std::cin, std::cout, catalogue);
}