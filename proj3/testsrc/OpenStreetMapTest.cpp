#include "OpenStreetMap.h"
#include "XMLReader.h"
#include "StringDataSource.h"
#include <gtest/gtest.h>
#include <memory>
#include <string>

// Helper function to create a sample OSM XML string
std::string CreateSampleOSMXML() {
    return R"(
        <osm version="0.6" generator="OpenStreetMap.org">
            <node id="1" lat="37.7749" lon="-122.4194">
                <tag k="name" v="San Francisco"/>
                <tag k="population" v="881549"/>
            </node>
            <node id="2" lat="34.0522" lon="-118.2437">
                <tag k="name" v="Los Angeles"/>
            </node>
            <way id="101">
                <nd ref="1"/>
                <nd ref="2"/>
                <tag k="highway" v="primary"/>
            </way>
        </osm>
    )";
}

// Test fixture for COpenStreetMap
class COpenStreetMapTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a sample OSM XML string
        std::string xmlData = CreateSampleOSMXML();

        // Create a StringDataSource with the sample XML data
        auto dataSource = std::make_shared<CStringDataSource>(xmlData);

        // Create an XMLReader with the data source
        auto xmlReader = std::make_shared<CXMLReader>(dataSource);

        // Create the COpenStreetMap object
        openStreetMap = std::make_unique<COpenStreetMap>(xmlReader);
    }

    std::unique_ptr<COpenStreetMap> openStreetMap;
};

// Test NodeCount and WayCount
TEST_F(COpenStreetMapTest, NodeAndWayCount) {
    EXPECT_EQ(openStreetMap->NodeCount(), 2); // 2 nodes in the sample XML
    EXPECT_EQ(openStreetMap->WayCount(), 1);  // 1 way in the sample XML
}

// Test NodeByID
TEST_F(COpenStreetMapTest, NodeByID) {
    auto node1 = openStreetMap->NodeByID(1);
    ASSERT_NE(node1, nullptr);
    EXPECT_EQ(node1->ID(), 1);
    EXPECT_EQ(node1->Location().first, 37.7749);
    EXPECT_EQ(node1->Location().second, -122.4194);

    auto node2 = openStreetMap->NodeByID(2);
    ASSERT_NE(node2, nullptr);
    EXPECT_EQ(node2->ID(), 2);
    EXPECT_EQ(node2->Location().first, 34.0522);
    EXPECT_EQ(node2->Location().second, -118.2437);

    auto invalidNode = openStreetMap->NodeByID(999); // Invalid ID
    EXPECT_EQ(invalidNode, nullptr);
}

// Test NodeByIndex
TEST_F(COpenStreetMapTest, NodeByIndex) {
    auto node0 = openStreetMap->NodeByIndex(0);
    ASSERT_NE(node0, nullptr);
    EXPECT_EQ(node0->ID(), 1); // First node has ID 1

    auto node1 = openStreetMap->NodeByIndex(1);
    ASSERT_NE(node1, nullptr);
    EXPECT_EQ(node1->ID(), 2); // Second node has ID 2

    auto invalidNode = openStreetMap->NodeByIndex(2); // Invalid index
    EXPECT_EQ(invalidNode, nullptr);
}

// Test Node Attributes
TEST_F(COpenStreetMapTest, NodeAttributes) {
    auto node1 = openStreetMap->NodeByID(1);
    ASSERT_NE(node1, nullptr);

    EXPECT_EQ(node1->AttributeCount(), 2); // 2 attributes for node 1
    EXPECT_TRUE(node1->HasAttribute("name"));
    EXPECT_EQ(node1->GetAttribute("name"), "San Francisco");
    EXPECT_TRUE(node1->HasAttribute("population"));
    EXPECT_EQ(node1->GetAttribute("population"), "881549");

    auto node2 = openStreetMap->NodeByID(2);
    ASSERT_NE(node2, nullptr);

    EXPECT_EQ(node2->AttributeCount(), 1); // 1 attribute for node 2
    EXPECT_TRUE(node2->HasAttribute("name"));
    EXPECT_EQ(node2->GetAttribute("name"), "Los Angeles");
}

// Test WayByID
TEST_F(COpenStreetMapTest, WayByID) {
    auto way101 = openStreetMap->WayByID(101);
    ASSERT_NE(way101, nullptr);
    EXPECT_EQ(way101->ID(), 101);

    auto invalidWay = openStreetMap->WayByID(999); // Invalid ID
    EXPECT_EQ(invalidWay, nullptr);
}

// Test WayByIndex
TEST_F(COpenStreetMapTest, WayByIndex) {
    auto way0 = openStreetMap->WayByIndex(0);
    ASSERT_NE(way0, nullptr);
    EXPECT_EQ(way0->ID(), 101); // Only way has ID 101

    auto invalidWay = openStreetMap->WayByIndex(1); // Invalid index
    EXPECT_EQ(invalidWay, nullptr);
}

// Test Way Node References
TEST_F(COpenStreetMapTest, WayNodeReferences) {
    auto way101 = openStreetMap->WayByID(101);
    ASSERT_NE(way101, nullptr);

    EXPECT_EQ(way101->NodeCount(), 2); // 2 nodes in the way
    EXPECT_EQ(way101->GetNodeID(0), 1); // First node has ID 1
    EXPECT_EQ(way101->GetNodeID(1), 2); // Second node has ID 2

    auto invalidNodeID = way101->GetNodeID(2); // Invalid index
    EXPECT_EQ(invalidNodeID, CStreetMap::InvalidNodeID);
}

// Test Way Attributes
TEST_F(COpenStreetMapTest, WayAttributes) {
    auto way101 = openStreetMap->WayByID(101);
    ASSERT_NE(way101, nullptr);

    EXPECT_EQ(way101->AttributeCount(), 1); // 1 attribute for way 101
    EXPECT_TRUE(way101->HasAttribute("highway"));
    EXPECT_EQ(way101->GetAttribute("highway"), "primary");
}

// Test Invalid XML
TEST(COpenStreetMapInvalidXMLTest, InvalidXML) {
    std::string invalidXML = "<invalid></invalid>";
    auto dataSource = std::make_shared<CStringDataSource>(invalidXML);
    auto xmlReader = std::make_shared<CXMLReader>(dataSource);

    EXPECT_THROW({
        COpenStreetMap openStreetMap(xmlReader);
    }, std::runtime_error);
}