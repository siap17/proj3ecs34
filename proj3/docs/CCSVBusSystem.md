Through this docs file, we ultimately just wanted to explain my working and reasoning for how I organized all this data. 

For lines 35-42, we just watned to include the necessary headers within my work. 

In lines 46-58, we established the SStop Class, which represents a stop. SStop is a nested classs within the CCSVBusSsytem and it inherits from 
CBusSystemStop. The DStopID actually stores every stop ID and NodeIDVal just stores the value of the node. 
I had to override the ID() to return the StopID and I also had to override the NodeID() to return the Value of the Node. 

In lines 61-83, we established the SRoute class to represent the route of the bus. Similar to SStop, SRoute is also a nested class that contains DName, the route name and the DStopIDs, which represents 
the list of the stop IDs in the route. 
We had to override once again so that they don't remain as abstract classes. 

In SImplementation, we have a private implementation that simply stores the DStops, DRoutes, and creates maps for stops and routes 
(aka) DStopByIDMap and DRouteByNameMap. 

We also constructed CCSVBusSystem through DImplementation. Within the DImplementation, we had to "read" the stop data and this  
is within if (stopsrc) (line 101 - 111), we essentiatlly read through every row of stopsrc and conver each string data to std::stoul. 
This creates a SStop object and stores it in a list and allows it to be added to the map for DStopByIDMap. Reading the routes is similar to 
the stops in lines 117-136, we essentially read the route data. Through routeMap we create a temporary placeholder to read the routes 
that we have created. If the row size is less than or equal to 2 then it reads the data or else it catches an excpetion. However, we need to 
create a case for handling transfers so taht is why in lines 138-141, we group stops unders routes and transfer routes from routeMap to DImplmentation. 

We also set a default destructor in line 147. 

Lastly, we created an operator overloading in << to handle printing the bus system to std::ostream so none of our cases could 
overwrite the MakeFile. 






Here is the code: 

#include "CSVBusSystem.h"
#include "BusSystem.h"
#include <vector>
#include <memory>
#include <unordered_map> 
#include <string> 
#include <iostream> 
#include <sstream> 


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

   struct CCSVBusSystem::SImplementation{
        std::vector<std::shared_ptr<SStop>> DStops; 
        std::vector<std::shared_ptr<SRoute>> DRoutes; 
        std::unordered_map<TStopID, std::shared_ptr<SStop>> DStopByIDMap;
        std::unordered_map<std::string, std::shared_ptr<SRoute>> DRouteByNameMap;


        SImplementation(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader>routesrc){

        }
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
                    stop->NodeIDVal = std::stoul(stopRow[1]); 
                    DImplementation->DStops.push_back(stop); 
                    DImplementation->DStopByIDMap[stop->DStopID] = stop;
                } catch (const std::exception& e){
                    std::cerr << "Caught an exception" << e.what() << "\n"; 
                }
            }
        }   
    }

    if (routesrc) {
        std::unordered_map<std::string, std::shared_ptr<SRoute>> routeMap;

        while (routesrc->ReadRow(stopRow)) {
            if (stopRow.size() >= 2) {
                try {
                    std::string rName = stopRow[0];
                    TStopID stopID = std::stoul(stopRow[1]);
                    
                    auto& route = routeMap[rName];
                    if (!route) {
                        route = std::make_shared<SRoute>();
                        route->DName = rName;
                    }
                    route->DStopIDs.push_back(stopID);
                } catch (const std::exception& e) {
                    std::cerr << "Error processing route row: " << e.what() << "\n";
                }
            }
        }

        // Transfer routes to implementation
        for (const auto& pair : routeMap) {
            DImplementation->DRouteByNameMap[pair.first] = pair.second;
            DImplementation->DRoutes.push_back(pair.second);
        }
    }
}


CCSVBusSystem::~CCSVBusSystem() = default;


std::size_t CCSVBusSystem::StopCount() const noexcept { 
    return DImplementation->DStops.size(); 
}

std::size_t CCSVBusSystem::RouteCount() const noexcept { 
    return DImplementation->DRoutes.size(); 
}

std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByIndex(std::size_t index) const noexcept { 
    if (index < DImplementation->DStops.size()){
        return DImplementation->DStops[index]; 
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
        return DImplementation->DRoutes[index];
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


