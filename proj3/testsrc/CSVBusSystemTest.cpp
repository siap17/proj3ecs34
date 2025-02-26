#include "CSVBusSystem.h"
#include <iostream>

int main() {
    std::shared_ptr<CDSVReader> stopsrc, routesrc;
    CCSVBusSystem busSystem(stopsrc, routesrc);
    
    std::cout << "Stop Count: " << busSystem.StopCount() << std::endl;
    std::cout << "Route Count: " << busSystem.RouteCount() << std::endl;
    
    return 0;
}