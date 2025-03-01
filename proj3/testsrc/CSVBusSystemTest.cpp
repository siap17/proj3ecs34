#include "CSVBusSystem.h"
#include <iostream>
#include <gtest/gtest.h> 
#include <vector> 
#include <string> 

class MockDSVReader{

public: 
    MockDSVReader(const std::vector<std::vector<std::string>>& data) : DData(data), DCurrentRow(0){}

    bool ReadRow(std:: vector<std::string> & row) override {
        if (DCurrentRow < DData.size()){
            row = DData[DCurrentRow++]; 
            return true; 
        }
        return false; 
    }
private: 
    std::vector<std::vector<std::string>> DData; 
    size_t DCurrentRow; 

};



int main() {
    std::shared_ptr<CDSVReader> stopsrc, routesrc;
    CCSVBusSystem busSystem(stopsrc, routesrc);
    
    std::cout << "Stop Count: " << busSystem.StopCount() << std::endl;
    std::cout << "Route Count: " << busSystem.RouteCount() << std::endl;
    
    return 0;
}


TEST(CCSVBusSystem, StopCount){
    std::vector<std::vector<std::string>> stopData = {
        { "1", "Stop A",  "0.0", "0.0"}, 
        { "2", "Stop B",  "1.0", "1.0"}, 
        { "3", "Stop C",  "2.0", "2.0"} 
    }; 

    std:: vector<std::vector<std::string>> routeData = {
        {"1", "Route 1", "1,2,3"}, 
        {"2", "Route 2",  "3,2,1"}
    };

    auto stopsrc = std::make_shared<MockDSVReader>(stopData); 
    auto routesrc = std::make_shared<MockDSVReader>(routeData); 

    CCSVBusSystem busSystem(stopsrc, routesrc); 
    EXPECT_EQ(busSystem.StopCount(), 3); 

}

TEST(CCSVBusSystem, StopByID){
    std::vector<std::vector<std::string>> stopData = {
        { "1", "Stop A",  "0.0", "0.0"}, 
        { "2", "Stop B",  "1.0", "1.0"}, 
        { "3", "Stop C",  "2.0", "2.0"} 
    }; 

    std:: vector<std::vector<std::string>> routeData = {
        {"1", "Route 1", "1,2,3"}, 
        {"2", "Route 2",  "3,2,1"}
    };

    auto stopsrc = std::make_shared<MockDSVReader>(stopData); 
    auto routesrc = std::make_shared<MockDSVReader>(routeData); 

    CCSVBusSystem busSystem(stopsrc, routesrc); 

    auto stop = busSystem.StopByID(2); 
    EXPECT_NE(stop, nullptr); 
    EXPECT_EQ(stop->DName, "Stop B");    

}


TEST(CCSVBusSystemTest, RouteByName){
    std::vector<std::vector<std::string>> stopData = {
        { "1", "Stop A",  "0.0", "0.0"}, 
        { "2", "Stop B",  "1.0", "1.0"}, 
        { "3", "Stop C",  "2.0", "2.0"} 
    }; 

    std:: vector<std::vector<std::string>> routeData = {
        {"1", "Route 1", "1,2,3"}, 
        {"2", "Route 2",  "3,2,1"}
    };

    auto stopsrc = std::make_shared<MockDSVReader>(stopData); 
    auto routesrc = std::make_shared<MockDSVReader>(routeData); 

    CCSVBusSystem busSystem(stopsrc, routesrc); 

    auto route = busSystem.RouteByName("Route 1"); 
    EXPECT_NE(route, nullptr); 
    EXPECT_EQ(route->DName, "Route 1"); 


}