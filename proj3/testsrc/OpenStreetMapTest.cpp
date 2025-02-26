#include "OpenStreetMap.h"
#include <iostream>

int main() {
    std::shared_ptr<CXMLReader> src;
    COpenStreetMap streetMap(src);
    
    std::cout << "Node Count: " << streetMap.NodeCount() << std::endl;
    std::cout << "Way Count: " << streetMap.WayCount() << std::endl;
    
    return 0;
}
