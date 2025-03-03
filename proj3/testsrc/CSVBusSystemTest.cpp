// CSVBusSystemTest.cpp

#include "CSVBusSystem.h"
#include "DSVReader.h"  // Assumed header for CDSVReader
#include "BusSystem.h"  // Assumed header for CBusSystem definitions
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

    bool ReadRow(std::vector<std::string>& row) override {
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
    // Expected format: [StopID, StopName, Latitude, Longitude]
    std::vector<std::vector<std::string>> stopData = {
        {"1", "Stop A", "0.0", "0.0"},
        {"2", "Stop B", "1.0", "1.0"},
        {"3", "Stop C", "2.0", "2.0"}
    };

    // Sample CSV data for routes.
    // Expected format: [RouteID, RouteName, "stopID1,stopID2,..."]
    std::vector<std::vector<std::string>> routeData = {
        {"1", "Route 1", "1,2,3"},
        {"2", "Route 2", "3,2,1"}
    };

    auto stopsrc = std::make_shared<MockDSVReader>(stopData);
    auto routesrc = std::make_shared<MockDSVReader>(routeData);

    CCSVBusSystem busSystem(stopsrc, routesrc);

    EXPECT_EQ(busSystem.StopCount(), 3);
}

// Test case: Verify that StopByID returns the correct stop.
TEST(CCSVBusSystemTest, StopByID) {
    std::vector<std::vector<std::string>> stopData = {
        {"1", "Stop A", "0.0", "0.0"},
        {"2", "Stop B", "1.0", "1.0"},
        {"3", "Stop C", "2.0", "2.0"}
    };

    std::vector<std::vector<std::string>> routeData = {
        {"1", "Route 1", "1,2,3"},
        {"2", "Route 2", "3,2,1"}
    };

    auto stopsrc = std::make_shared<MockDSVReader>(stopData);
    auto routesrc = std::make_shared<MockDSVReader>(routeData);

    CCSVBusSystem busSystem(stopsrc, routesrc);

    auto stop = busSystem.StopByID(2);
    ASSERT_NE(stop, nullptr);

    // Since the abstract interface does not expose the name,
    // we assume your concrete implementation (e.g. SStop in your private impl)
    // provides a way to access the name. In our implementation above, we did not add a getter.
    // For testing, if you have a concrete type, you can dynamic_cast it.
    // Here, we assume that the concrete type has a public member "DName".
    auto concreteStop = std::dynamic_pointer_cast<CCSVBusSystem::SImplementation::SStop>(stop);
    ASSERT_NE(concreteStop, nullptr);
    // Adjust the field name if necessary (here we use DName).
    EXPECT_EQ(concreteStop->DName, "Stop B");
}

// Test case: Verify that RouteByName returns the correct route.
TEST(CCSVBusSystemTest, RouteByName) {
    std::vector<std::vector<std::string>> stopData = {
        {"1", "Stop A", "0.0", "0.0"},
        {"2", "Stop B", "1.0", "1.0"},
        {"3", "Stop C", "2.0", "2.0"}
    };

    std::vector<std::vector<std::string>> routeData = {
        {"1", "Route 1", "1,2,3"},
        {"2", "Route 2", "3,2,1"}
    };

    auto stopsrc = std::make_shared<MockDSVReader>(stopData);
    auto routesrc = std::make_shared<MockDSVReader>(routeData);

    CCSVBusSystem busSystem(stopsrc, routesrc);

    auto route = busSystem.RouteByName("Route 1");
    ASSERT_NE(route, nullptr);
    EXPECT_EQ(route->Name(), "Route 1");
}

// Test case: Test the operator<< overload.
TEST(CCSVBusSystemTest, OutputOperator) {
    std::vector<std::vector<std::string>> stopData = {
        {"1", "Stop A", "0.0", "0.0"},
        {"2", "Stop B", "1.0", "1.0"}
    };

    std::vector<std::vector<std::string>> routeData = {
        {"1", "Route 1", "1,2"}
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
