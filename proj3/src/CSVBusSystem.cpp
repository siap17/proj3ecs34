#include "CSVBusSystem.h"
#include "BusSystem.h"
#include <vector>
#include <memory>
#include <unordered_map> 
#include <string> 
#include <iostream> 
#include <sstream> 

using TRouteID = unsigned int; 

// Private Implementation
class CCSVBusSystem::SStop : public CBusSystem::SStop {
    public: 
        TStopID DStopID; 
        CStreetMap::TNodeID NodeIDVal; 

        TStopID ID() const noexcept override {
            return DStopID; 
        } 

        CStreetMap::TNodeID NodeID() const noexcept override {
            return NodeIDVal; 
        }
    }; 

    class CCSVBusSystem::SRoute : public CBusSystem::SRoute {
    public: 
        std::string DName;                   //This establishes the name of the route essentially 
        std::vector<TStopID> DStopIDs;       //This lists the Stop IDs that form the route 

        // This returns the name of the route hence why we call on override 
        std::string Name() const noexcept override{ 
            return DName; 
        }

        //This returns the size "aka" number of stopping points int he route 
        std::size_t StopCount() const noexcept override{ 
            return DStopIDs.size(); 
        }

        //This returns the stop ID to the corresponding index 
        TStopID GetStopID(std::size_t index) const noexcept override{
            if (index < DStopIDs.size()){
                return DStopIDs[index]; 
            }
            return CBusSystem::InvalidStopID;    //Otherwise it would return an invalid ID 
        }
    }; 

    SImplementation(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader> routesrc){
        std::vector<SStop> DStops; 
        std::vector<SRoute> DRoutes; 
        std::unordered_map<TStopID, std::shared_ptr<SStop>> DStopByIDMap;
        std::unordered_map<std::string, std::shared_ptr<SRoute>> DRouteByNameMap;

    }; 

CCSVBusSystem::CCSVBusSystem(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader> routesrc){
    DImplementation = std::make_unique<SImplementation>(stopsrc, routesrc); 
    std:: vector<std::string> stopRow; 

    
    if (stopsrc){
        while (stopsrc->ReadRow(stopRow)){
            if (stopRow.size() >= 2){
                try {
                    auto stop = std::make_shared<SStop>(); 
                    stop->DStopID=std::stoul(stopRow[0]); 
                    stop->NodeIDVal = std::stoul(stopRow[1])
                    DImplementation->DStops[stop->DStopID] = stop; 
                    DImplementation->DStopbyIDMap.push_back(stop); 
                } catch (const std::exception& e){
                    std::cerr << "Caught an exception" << e.what() << "\n"; 
                }
            }
        }   
    }

    if (routesrc) {
    std::unordered_map<std::string, std::shared_ptr<SRoute>> routeRow; 
    while (routesrc->ReadRow(stopRow)) {
        try {
            if (stopRow.size() >= 2) {
                try{
                std::string rName = routeRow[0]; 
                TStopID stopID = std::stoul(routeRow[1]); 
                auto& route = routeRow[rName];  
                if (!route) {
                    route = std::make_shared<SRoute>(); 
                    route->DName = rName; 
                    }
                }
                route -> DStopIDs.push_back(stopID); 
            }
        } catch (const std::exception& e) {
            std::cerr << "Error processing route row: " << e.what() << "\n"; 
        }
    }

    for (const auto& p : routeRow){
        DImplementation->DRouteByNameMap[p.first] = p.second; 
        DImplmentation->DRoutes.push_back(p.second); 
    }
}

};



CCSVBusSystem::~CCSVBusSystem() = default;


std::size_t CCSVBusSystem::StopCount() const noexcept { 
    return DImplementation->DStops.size(); 
}

std::size_t CCSVBusSystem::RouteCount() const noexcept { 
    return DImplementation->DRoutes.size(); 
}

std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByIndex(std::size_t index) const noexcept { 
    if (index < DImplementation->DStops.size()){
        const auto& stop = DImplementation->DStops[index]; 
        return DImplementation->DStopByIDMap[stop.DStopID]; 
    }
    return nullptr; 
}


std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByID(TStopID id) const noexcept { 
    auto it = DImplementation->DStopByIDMap.find(id); 
    if (it != DImplementation ->DStopByIDMap.end()){
        return it->second; 
    }
    return nullptr; 
}

std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByIndex(std::size_t index) const noexcept { 
    if (index < DImplementation->DRoutes.size()){
        const auto& route = DImplementation->DRoutes[index]; 
        return DImplementation->DRouteByNameMap[route.DName];
    }
    return nullptr;
}

std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByName(const std::string &name) const noexcept { 
    
    auto it = DImplementation->DRouteByNameMap.find(name); 
    if (it != DImplementation->DRouteByNameMap.end()){
        return it->second; 
    }
    return nullptr; 
}

std::ostream& operator<<(std::ostream& os, const CCSVBusSystem& busSystem) {
    os << "Bus System Details:\n"; 
    os << "Stop Count: " << busSystem.StopCount() << "\n"; 
    for (std::size_t i = 0; i < busSystem.StopCount(); ++i) {
        auto stopPtr = busSystem.StopByIndex(i); 
        if (stopPtr) {
            os << "Stop " << i << ": ID = " << stopPtr->ID() 
               << ", NodeID = " << stopPtr->NodeID() << "\n"; 
        }
    }

    os << "Route Count: " << busSystem.RouteCount() << "\n"; 
    for (std::size_t i = 0; i < busSystem.RouteCount(); ++i) {
        auto routePtr = busSystem.RouteByIndex(i); 
        if (routePtr) {
            os << "Route " << i << ": Name = " << routePtr->Name()
               << ", StopCount = " << routePtr->StopCount() << "\n"; 
            os << "Stops: "; 

            for (std::size_t j = 0; j < routePtr->StopCount(); ++j) {
                os << routePtr->GetStopID(j); 
                if (j < routePtr->StopCount() - 1) {
                    os << ", "; 
                }
            }
            os << "\n"; 
        }
    }
    return os; 
}