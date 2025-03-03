#include "OpenStreetMap.h"
#include "XMLReader.h"
#include <gtest/gtest.h>
#include <string>
#include <memory>
#include <sstream>

// Mock XMLReader for testing
class MockXMLReader : public CXMLReader {
public:
    MockXMLReader(const std::vector<SXMLEntity>& entities) 
        : DEntities(entities), DCurrentIndex(0), CXMLReader("") {}

    bool ReadEntity(SXMLEntity& entity, bool skipComments) override {
        if (DCurrentIndex < DEntities.size()) {
            entity = DEntities[DCurrentIndex++];
            return true;
        }
        return false;
    }

private:
    std::vector<SXMLEntity> DEntities;
    size_t DCurrentIndex;
};

// Helper function to create XML entities for testing
std::vector<SXMLEntity> CreateTestOSMData() {
    std::vector<SXMLEntity> entities;
    
    // OSM root element
    SXMLEntity osmStart;
    osmStart.DType = SXMLEntity::EType::StartElement;
    osmStart.DNameData = "osm";
    entities.push_back(osmStart);
    
    // Node 1
    SXMLEntity node1Start;
    node1Start.DType = SXMLEntity::EType::StartElement;
    node1Start.DNameData = "node";
    node1Start.DAttributes["id"] = "1001";
    node1Start.DAttributes["lat"] = "34.1050";
    node1Start.DAttributes["lon"] = "-118.2892";
    entities.push_back(node1Start);
    
    // Tag for Node 1
    SXMLEntity node1TagStart;
    node1TagStart.DType = SXMLEntity::EType::StartElement;
    node1TagStart.DNameData = "tag";
    node1TagStart.DAttributes["k"] = "name";
    node1TagStart.DAttributes["v"] = "Test Node 1";
    entities.push_back(node1TagStart);
    
    SXMLEntity node1TagEnd;
    node1TagEnd.DType = SXMLEntity::EType::EndElement;
    node1TagEnd.DNameData = "tag";
    entities.push_back(node1TagEnd);
    
    SXMLEntity node1End;
    node1End.DType = SXMLEntity::EType::EndElement;
    node1End.DNameData = "node";
    entities.push_back(node1End);
    
    // Node 2
    SXMLEntity node2Start;
    node2Start.DType = SXMLEntity::EType::StartElement;
    node2Start.DNameData = "node";
    node2Start.DAttributes["id"] = "1002";
    node2Start.DAttributes["lat"] = "34.1051";
    node2Start.DAttributes["lon"] = "-118.2893";
    entities.push_back(node2Start);
    
    SXMLEntity node2End;
    node2End.DType = SXMLEntity::EType::EndElement;
    node2End.DNameData = "node";
    entities.push_back(node2End);
    
    // Way 1
    SXMLEntity way1Start;
    way1Start.DType = SXMLEntity::EType::StartElement;
    way1Start.DNameData = "way";
    way1Start.DAttributes["id"] = "2001";
    entities.push_back(way1Start);
    
    // Node refs for Way 1
    SXMLEntity way1Ref1Start;
    way1Ref1Start.DType = SXMLEntity::EType::StartElement;
    way1Ref1Start.DNameData = "nd";
    way1Ref1Start.DAttributes["ref"] = "1001";
    entities.push_back(way1Ref1Start);
    
    SXMLEntity way1Ref1End;
    way1Ref1End.DType = SXMLEntity::EType::EndElement;
    way1Ref1End.DNameData = "nd";
    entities.push_back(way1Ref1End);
    
    SXMLEntity way1Ref2Start;
    way1Ref2Start.DType = SXMLEntity::EType::StartElement;
    way1Ref2Start.DNameData = "nd";
    way1Ref2Start.DAttributes["ref"] = "1002";
    entities.push_back(way1Ref2Start);
    
    SXMLEntity way1Ref2End;
    way1Ref2End.DType = SXMLEntity::EType::EndElement;
    way1Ref2End.DNameData = "nd";
    entities.push_back(way1Ref2End);
    
    // Tag for Way 1
    SXMLEntity way1TagStart;
    way1TagStart.DType = SXMLEntity::EType::StartElement;
    way1TagStart.DNameData = "tag";
    way1TagStart.DAttributes["k"] = "highway";
    way1TagStart.DAttributes["v"] = "residential";
    entities.push_back(way1TagStart);
    
    SXMLEntity way1TagEnd;
    way1TagEnd.DType = SXMLEntity::EType::EndElement;
    way1TagEnd.DNameData = "tag";
    entities.push_back(way1TagEnd);
    
    SXMLEntity way1End;
    way1End.DType = SXMLEntity::EType::EndElement;
    way1End.DNameData = "way";
    entities.push_back(way1End);
    
    // End OSM
    SXMLEntity osmEnd;
    osmEnd.DType = SXMLEntity::EType::EndElement;
    osmEnd.DNameData = "osm";
    entities.push_back(osmEnd);
    
    return entities;
}

// Test Suite for COpenStreetMap
class COpenStreetMapTest : public ::testing::Test {
protected:
    std::shared_ptr<COpenStreetMap> DStreetMap;
    
    void SetUp() override {
        auto entities = CreateTestOSMData();
        auto xmlReader = std::make_shared<MockXMLReader>(entities);
        DStreetMap = std::make_shared<COpenStreetMap>(xmlReader);
    }
};

// Test that the node count matches expectations
TEST_F(COpenStreetMapTest, NodeCount) {
    EXPECT_EQ(DStreetMap->NodeCount(), 2);
}

// Test that the way count matches expectations
TEST_F(COpenStreetMapTest, WayCount) {
    EXPECT_EQ(DStreetMap->WayCount(), 1);
}

// Test NodeByIndex
TEST_F(COpenStreetMapTest, NodeByIndex) {
    auto node = DStreetMap->NodeByIndex(0);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->ID(), 1001);
    
    auto location = node->Location();
    EXPECT_DOUBLE_EQ(location.first, 34.1050);
    EXPECT_DOUBLE_EQ(location.second, -118.2892);
    
    // Test out of bounds index
    EXPECT_EQ(DStreetMap->NodeByIndex(10), nullptr);
}

// Test NodeByID
TEST_F(COpenStreetMapTest, NodeByID) {
    auto node = DStreetMap->NodeByID(1002);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->ID(), 1002);
    
    // Test invalid ID
    EXPECT_EQ(DStreetMap->NodeByID(9999), nullptr);
}

// Test WayByIndex
TEST_F(COpenStreetMapTest, WayByIndex) {
    auto way = DStreetMap->WayByIndex(0);
    ASSERT_NE(way, nullptr);
    EXPECT_EQ(way->ID(), 2001);
    EXPECT_EQ(way->NodeCount(), 2);
    
    // Test out of bounds index
    EXPECT_EQ(DStreetMap->WayByIndex(10), nullptr);
}

// Test WayByID
TEST_F(COpenStreetMapTest, WayByID) {
    auto way = DStreetMap->WayByID(2001);
    ASSERT_NE(way, nullptr);
    EXPECT_EQ(way->ID(), 2001);
    
    // Test invalid ID
    EXPECT_EQ(DStreetMap->WayByID(9999), nullptr);
}

// Test node attributes
TEST_F(COpenStreetMapTest, NodeAttributes) {
    auto node = DStreetMap->NodeByID(1001);
    ASSERT_NE(node, nullptr);
    
    EXPECT_TRUE(node->HasAttribute("name"));
    EXPECT_EQ(node->GetAttribute("name"), "Test Node 1");
    EXPECT_FALSE(node->HasAttribute("nonexistent"));
    EXPECT_EQ(node->GetAttribute("nonexistent"), "");
    
    EXPECT_EQ(node->AttributeCount(), 1);
    EXPECT_EQ(node->GetAttributeKey(0), "name");
}

// Test way attributes and node references
TEST_F(COpenStreetMapTest, WayAttributesAndNodes) {
    auto way = DStreetMap->WayByID(2001);
    ASSERT_NE(way, nullptr);
    
    EXPECT_TRUE(way->HasAttribute("highway"));
    EXPECT_EQ(way->GetAttribute("highway"), "residential");
    
    EXPECT_EQ(way->NodeCount(), 2);
    EXPECT_EQ(way->GetNodeID(0), 1001);
    EXPECT_EQ(way->GetNodeID(1), 1002);
    EXPECT_EQ(way->GetNodeID(2), CStreetMap::InvalidNodeID);
}

// Test empty or invalid input
TEST(COpenStreetMapEmptyTest, HandleEmptyData) {
    std::vector<SXMLEntity> emptyEntities;
    auto xmlReader = std::make_shared<MockXMLReader>(emptyEntities);
    
    COpenStreetMap streetMap(xmlReader);
    EXPECT_EQ(streetMap.NodeCount(), 0);
    EXPECT_EQ(streetMap.WayCount(), 0);
}

// Test malformed input - missing attributes
TEST(COpenStreetMapMalformedTest, HandleMalformedData) {
    std::vector<SXMLEntity> entities;
    
    // OSM root element
    SXMLEntity osmStart;
    osmStart.DType = SXMLEntity::EType::StartElement;
    osmStart.DNameData = "osm";
    entities.push_back(osmStart);
    
    // Malformed node (missing lat/lon)
    SXMLEntity nodeStart;
    nodeStart.DType = SXMLEntity::EType::StartElement;
    nodeStart.DNameData = "node";
    nodeStart.DAttributes["id"] = "1001";
    // Missing lat/lon attributes
    entities.push_back(nodeStart);
    
    SXMLEntity nodeEnd;
    nodeEnd.DType = SXMLEntity::EType::EndElement;
    nodeEnd.DNameData = "node";
    entities.push_back(nodeEnd);
    
    // End OSM
    SXMLEntity osmEnd;
    osmEnd.DType = SXMLEntity::EType::EndElement;
    osmEnd.DNameData = "osm";
    entities.push_back(osmEnd);
    
    auto xmlReader = std::make_shared<MockXMLReader>(entities);
    
    // This should not crash
    COpenStreetMap streetMap(xmlReader);
    // The node should still be counted even if lat/lon are missing
    EXPECT_EQ(streetMap.NodeCount(), 1);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}