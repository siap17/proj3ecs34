#include "CSVBusSystem.h"
#include <vector>
#include <memory>
#include <unordered_map> 
#include <string> 
#include <sstream> 

using TRouteID = unsigned int; 

// Private Implementation
struct CCSVBusSystem::SImplementation {
    struct SStop : public CBusSystem::SStop{
        TStopID DStopID; 
        std:: string DName; 
        double DLatitude; 
        double Dlongitude; 

        TStopID ID() const noexcept override {
            return DStopID; 
        }

        CStreetMap::TNodeID NodeID() const noexcept override {
            return 0; 
        }
    }; 

    struct SRoute {
        TRouteID DRouteID; 
        std::string DName; 
        std::vector<TStopID> DStropIDs; 
    }; 

    std::vector<SStop> DStops; 
    std::vector<SRoute> DRoutes; 
    std::unordered_map<TStopID, std::shared_ptr<CBusSystem::SStop>> DStopByIDMap; 
    std::unordered_map<std::string, std::shared_ptr<CBusSystem::SRoute>> DRouteByNameMap;

    SImplementation(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader> routesrc){

        std:: vector<std::string> stopRow; 
        bool FirstRow = true; 
        while (stopsrc->ReadRow(stopRow)){
            if (FirstRow) {
                FirstRow = false; 
                continue; 
            }
            if (stopRow.size() >= 2){
                SStop stop; 
                stop.DStopID = std::stoul(stopRow[0]); 
                stop.DStopID = std::stoul(stopRow[1]); 
                DStops.push_back(stop);
                DStopByIDMap[stop.DStopID] = std::make_shared<SStop>(stop); 
            }
        }

        std::vector<std::string> routeRow; 
        FirstRow = true; 
        while (routesrc ->ReadRow(routeRow)){

            if (FirstRow){
                FirstRow = false; 
                continue; 
            }
            if (routeRow.size() >= 2){
                std::string rName = routeRow[0]; 
                auto RoutingID = DRouteByNameMap.find(rName); 

                if (RoutingID == DRouteByNameMap.end()){
                    SRoute route; 
                    route.DName = rName; 
                    DRoutes.push_back(route); 
                    DRouteByNameMap[rName] = std::make_shared<SRoute>(route); 
                }
                DRouteByNameMap[rName]->DStropIDs.push_back(std::stoul(routeRow[1])); 
            }

        }
    }
};

CCSVBusSystem::CCSVBusSystem(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader> routesrc) 
    : DImplementation(std::make_unique<SImplementation>(stopsrc, routesrc)) {}


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
        return std::make_shared<CBusSystem::SStop>(stop.DStopID, stop.DName, stop.DLatitude, stop.Dlongitude); 
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
        return std::make_shared<CBusSystem::SRoute>(route.DRouteID, route.DName, route.DStropIDs); 
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