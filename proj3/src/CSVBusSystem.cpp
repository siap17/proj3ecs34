#include "CSVBusSystem.h"
#include <vector>
#include <memory>
#include <unordered_map> 
#include <string> 

using TRouteID = unsigned int; 

// Private Implementation
struct CCSVBusSystem::SImplementation {
    struct SStop{
        TStopID DStopID; 
        std:: string DName; 
        double DLatitude; 
        double Dlongitude; 
    }; 

    struct SRoute {
        TRouteID DRouteID; 
        std::string DName; 
        std::vector<TStopID> DStropIDs; 
    }; 

    std::vector<SStop> DStops; 
    std::vector<SRoute> DRoutes; 
    std::unordered_map<TStopID, std::shared_ptr<CBusSystem::SStop>> DStopByIDMap; 
    std::unordered_map<TStopID, std::shared_ptr<CBusSystem::SRoute>> DRouteByNameMap;

    SImplmentation(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader> routesrc){
        std:: vector<std::string> stopRow; 
        while (stoprow->ReadRow(stopRow)){
            if (stopRow.size() >= 4){
                SStop stop; 
                stop.DStopID = std::stoul(stopRow[0]); 
                stop.DName = stopRow[1]; 
                stop.DLatitude = std::stod(stopRow[2]); 
                stop.DLongitude = std::stod(stopRow[3]); 
                DStops.push_back(stop);
            }

            stopRow.clear(); 
        }

        std::vector<std::string> routeRow; 
        while (routesrc ->ReadRow(routeRow)){
            if (routeRow.size() >= 3){
                SRoute route; 
                route.DRouteID = std::stoul(routeRow[0]); 
                route.DName = routeRow[1]; 
                std::stringstream ss(routeRow[2]); 
                TStopID stopID; 
                while (ss >> stopID){
                    route.DStopIDs.push_back(stopID); 
                    if (ss.peek() == ',') ss.ignore(); 
                }
                DRoutes.push_back(route); 
            }
            routeRow.clear(); 

        }

        for (const auto& stop : DStops){
            DStopByIDMap[stop.DStopID] = std::make_shared<CBusSystem::SStop>(stop.DStopID, stop.DName, stop.DLatitude, stop.DLongitude); 
        }

        for (const auto& route : DRoutes){
            DRouteByNameMap[route.DName] = std::make_shared<CBusSystem:: SRoute> (route.DRouteID, route.DName. route.DStopIDs); 
        }
    }
};

CCSVBusSystem::CCSVBusSystem(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader> routesrc) 
    : DImplementation(std::make_unique<SImplementation>(stopsrc, routesrc)) {}

CCSVBusSystem::~CCSVBusSystem() = default;

std::size_t CCSVBusSystem::StopCount() const noexcept { 
    return DImplementation->DStops.size(); }
std::size_t CCSVBusSystem::RouteCount() const noexcept { 
    return DImplementation->DRoutes.size(); }

std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByIndex(std::size_t index) const noexcept { 
    if (index < DImplementation->DStops.size()){
        const auto& stop = DImplementation->DStops[index]; 
        return std::make_shared<CBusSystem::SStop>(stop.DStopID, stop.DName, stop.DLatitude, stop.DLongitude); 
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
    if (index < DImplemenation->DRoutes.size()){
        const auto& route = DImplementation->DRoutes[index]; 
        return std::make_shared<CBusSystem::SRoute>(route.DRouteID, route.DName, route.DStopIDs); 
    }
    return nullptr;
}

std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByName(const std::string &name) const noexcept { 
    
    auto it = DImplementation->DRouteByNameMap.find(name); 
    if (it != DImplemnetation->DRouteByNameMap.end()){
        return it->second; 
    }
    return nullptr; 
}