#include "CSVBusSystem.h"
#include "DSVReader.h"  
#include "BusSystem.h"  
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <memory>
#include <sstream>

// A concrete implementation of CDSVReader for testing purposes.
// This mock simply returns rows from a preloaded vector.
class MockDSVReader : public CDSVReader {
public:
    // The CSV data: each inner vector represents one row.
    MockDSVReader(const std::vector<std::vector<std::string>>& data)
        : DData(data), DCurrentRow(0)
        // Call the base class constructor with dummy values.
        , CDSVReader(nullptr, ',')
    {}

    // The signature must exactly match the one in CDSVReader
    bool ReadRow(std::vector<std::string>& row) {
        if (DCurrentRow < DData.size()) {
            row = DData[DCurrentRow++];
            return true;
        }
        return false;
    }

private:
    std::vector<std::vector<std::string>> DData;
    size_t DCurrentRow;
};

// Test case: Check the StopCount of the bus system.
TEST(CCSVBusSystemTest, StopCount) {
    // Sample CSV data for stops.
    std::vector<std::vector<std::string>> stopData = {
        {"1", "1001"},
        {"2", "1002"},
        {"3", "1003"}
    };

    // Sample CSV data for routes.
    std::vector<std::vector<std::string>> routeData = {
        {"Route 1", "1"},
        {"Route 1", "2"},
        {"Route 2", "3"}
    };

    auto stopsrc = std::make_shared<MockDSVReader>(stopData);
    auto routesrc = std::make_shared<MockDSVReader>(routeData);

    CCSVBusSystem busSystem(stopsrc, routesrc);

    EXPECT_EQ(busSystem.StopCount(), 3);
}

// Test case: Verify that StopByID returns the correct stop.
TEST(CCSVBusSystemTest, StopByID) {
    std::vector<std::vector<std::string>> stopData = {
        {"1", "1001"},
        {"2", "1002"},
        {"3", "1003"}
    };

    std::vector<std::vector<std::string>> routeData = {
        {"Route 1", "1"},
        {"Route 1", "2"},
        {"Route 2", "3"}
    };

    auto stopsrc = std::make_shared<MockDSVReader>(stopData);
    auto routesrc = std::make_shared<MockDSVReader>(routeData);

    CCSVBusSystem busSystem(stopsrc, routesrc);

    auto stop = busSystem.StopByID(2);
    ASSERT_NE(stop, nullptr);
    EXPECT_EQ(stop->ID(), 2);
    EXPECT_EQ(stop->NodeID(), 1002);
}

// Test case: Verify that RouteByName returns the correct route.
TEST(CCSVBusSystemTest, RouteByName) {
    std::vector<std::vector<std::string>> stopData = {
        {"1", "1001"},
        {"2", "1002"},
        {"3", "1003"}
    };

    std::vector<std::vector<std::string>> routeData = {
        {"Route 1", "1"},
        {"Route 1", "2"},
        {"Route 2", "3"}
    };

    auto stopsrc = std::make_shared<MockDSVReader>(stopData);
    auto routesrc = std::make_shared<MockDSVReader>(routeData);

    CCSVBusSystem busSystem(stopsrc, routesrc);

    auto route = busSystem.RouteByName("Route 1");
    ASSERT_NE(route, nullptr);
    EXPECT_EQ(route->Name(), "Route 1");
    EXPECT_EQ(route->StopCount(), 2);
    EXPECT_EQ(route->GetStopID(0), 1);
    EXPECT_EQ(route->GetStopID(1), 2);
}

// Test case: Test the operator<< overload.
TEST(CCSVBusSystemTest, OutputOperator) {
    std::vector<std::vector<std::string>> stopData = {
        {"1", "1001"},
        {"2", "1002"}
    };

    std::vector<std::vector<std::string>> routeData = {
        {"Route 1", "1"},
        {"Route 1", "2"}
    };

    auto stopsrc = std::make_shared<MockDSVReader>(stopData);
    auto routesrc = std::make_shared<MockDSVReader>(routeData);

    CCSVBusSystem busSystem(stopsrc, routesrc);

    std::ostringstream oss;
    oss << busSystem;
    std::string output = oss.str();

    // Check that the output contains expected strings.
    EXPECT_NE(output.find("Stop Count:"), std::string::npos);
    EXPECT_NE(output.find("Route Count:"), std::string::npos);
    EXPECT_NE(output.find("Stop 0:"), std::string::npos);
    EXPECT_NE(output.find("Route 0:"), std::string::npos);
}
