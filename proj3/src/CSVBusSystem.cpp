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

    class CCSVBusSystem:: SRoute : public CBusSystem::SRoute {
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
                SStop stop; 
                stop.DStopID = std::stoul(stopRow[0]); 
                stop.DName = stopRow[1]; 

                if (stopRow.size() >= 4){
                    stop.DLatitude=std::stod(stopRow[2]); 
                    stop.Dlongitude=std::stod(stopRow[3]); 
                }

                DStops.push_back(stop);
                DStopByIDMap[stop.DStopID] = std::make_shared<SStop>(stop); 
            }
        }   
    }

    if (routesrc){
        while (routesrc->ReadRow(stopRow)){
            if (stopRow.size() >= 2){
                std::string rName = routeRow[0]; 
                auto RoutingID = DRouteByNameMap.find(rName); 

                if (RoutingID == DRouteByNameMap.end()){
                    SRoute route; 
                    route.DRouteID = DRoutes.size(); 
                    route.DName = rName; 
                    DRoutes.push_back(route); 
                    DRouteByNameMap[rName] = std::make_shared<SRoute>(route);  
                }
               DRouteByNameMap[rName]->DStopIDs.push_back(std::stoul(routeRow[1])); 
            }
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


