#include "OpenStreetMap.h" // Header file for COpenStreetMap class
#include "XMLReader.h" // Header file for XML parsing functionality
#include <memory> // For smart pointers like std::shared_ptr and std::unique_ptr
#include <vector> // For storing nodes and ways in dynamic arrays
#include <string> // For handling string attributes 
#include <unordered_map> // For storing and searching attributes

// Defining implementation structure first using COpenStreetMap
struct COpenStreetMap::SImplementation {
    // Forward declarations of implementation classes
    class MapNode;
    class MapWay;

    // Storing ways and nodes here
    std::vector<std::shared_ptr<MapNode>> Nodes;
    std::vector<std::shared_ptr<MapWay>> Ways;

    // Helper method to handle attributes
    void ProcessAttributes(const std::vector<std::pair<std::string, std::string>>& attributes, 
                          std::unordered_map<std::string, std::string>& attributeMap) {
        for (const auto& attr : attributes) {
            attributeMap[attr.first] = attr.second;
        }
    }
};

// Now we define the implementation classes using CStreetMap::SNode
class COpenStreetMap::SImplementation::MapNode : public CStreetMap::SNode {
public:
    // ID of the node
    TNodeID NodeID; // Renamed to avoid conflict with ID()
    // Coordinates of the node or the location
    TLocation NodeLocation; // Renamed to avoid conflict with Location()
    // Key and value attributes using an unordered map
    std::unordered_map<std::string, std::string> Attributes;

    // Getting node's ID
    TNodeID ID() const noexcept override {
        return NodeID; // Use the renamed variable
    }

    // Getting node's location
    TLocation Location() const noexcept override {
        return NodeLocation; // Use the renamed variable
    }

    // Number of attributes node has
    std::size_t AttributeCount() const noexcept override {
        return Attributes.size();
    }

    // Retrieving key of attribute through index
    std::string GetAttributeKey(std::size_t index) const noexcept override {
        if (index < Attributes.size()) {
            auto it = Attributes.begin();
            std::advance(it, index);
            return it->first;
        }
        return "";
    }

    // Checking to see if the node has an attribute using key
    bool HasAttribute(const std::string &key) const noexcept override {
        return Attributes.find(key) != Attributes.end();
    }

    // Retrieve the value of attribute if the node has an attribute
    std::string GetAttribute(const std::string &key) const noexcept override {
        auto it = Attributes.find(key);
        if (it != Attributes.end()) {
            return it->second;
        }
        return "";
    }
};

// Way class, when using CStreetMap::SWay
class COpenStreetMap::SImplementation::MapWay : public CStreetMap::SWay {
public:
    // ID of way
    TWayID WayID; // Renamed to avoid conflict with ID()
    // Nodes IDs
    std::vector<TNodeID> NodeIDs;
    // Attributes for key and value using unordered map
    std::unordered_map<std::string, std::string> Attributes;

    // Getting the way's ID
    TWayID ID() const noexcept override {
        return WayID; // Use the renamed variable
    }

    // Number of nodes in way
    std::size_t NodeCount() const noexcept override {
        return NodeIDs.size();
    }

    // Getting Node ID through index
    TNodeID GetNodeID(std::size_t index) const noexcept override {
        if (index < NodeIDs.size()) {
            return NodeIDs[index];
        }
        return CStreetMap::InvalidNodeID;
    }

    // Number of attributes way has
    std::size_t AttributeCount() const noexcept override {
        return Attributes.size();
    }

    // Retrieving key of attribute through index
    std::string GetAttributeKey(std::size_t index) const noexcept override {
        if (index < Attributes.size()) {
            auto it = Attributes.begin();
            std::advance(it, index);
            return it->first;
        }
        return "";
    }

    // Checking to see if the way has an attribute using key
    bool HasAttribute(const std::string &key) const noexcept override {
        return Attributes.find(key) != Attributes.end();
    }

    // Retrieve the value of attribute if the way has an attribute
    std::string GetAttribute(const std::string &key) const noexcept override {
        auto it = Attributes.find(key);
        if (it != Attributes.end()) {
            return it->second;
        }
        return "";
    }
};

// Initialize the implementation
COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> src) {
    DImplementation = std::make_unique<SImplementation>();

    SXMLEntity entity;
    std::shared_ptr<SImplementation::MapNode> currentNode = nullptr;
    std::shared_ptr<SImplementation::MapWay> currentWay = nullptr;

    // Parsing the XML file
    while (src->ReadEntity(entity)) {
        if (entity.DType == SXMLEntity::EType::StartElement) {
            if (entity.DNameData == "node") {
                currentNode = std::make_shared<SImplementation::MapNode>();
                currentWay = nullptr;

                // Process node attributes
                for (const auto& attr : entity.DAttributes) {
                    if (attr.first == "id") {
                        currentNode->NodeID = std::stoull(attr.second); // Use the renamed variable
                    } else if (attr.first == "lat") {
                        currentNode->NodeLocation.first = std::stod(attr.second); // Use the renamed variable
                    } else if (attr.first == "lon") {
                        currentNode->NodeLocation.second = std::stod(attr.second); // Use the renamed variable
                    } else {
                        currentNode->Attributes[attr.first] = attr.second;
                    }
                }
            } else if (entity.DNameData == "way") {
                currentWay = std::make_shared<SImplementation::MapWay>();
                currentNode = nullptr;

                // Process way attributes
                for (const auto& attr : entity.DAttributes) {
                    if (attr.first == "id") {
                        currentWay->WayID = std::stoull(attr.second); // Use the renamed variable
                    } else {
                        currentWay->Attributes[attr.first] = attr.second;
                    }
                }
            } else if (entity.DNameData == "nd" && currentWay) {
                // Process node reference inside a way
                for (const auto& attr : entity.DAttributes) {
                    if (attr.first == "ref") {
                        currentWay->NodeIDs.push_back(std::stoull(attr.second));
                    }
                }
            } else if (entity.DNameData == "tag") {
                // Process tag element for both nodes and ways
                std::string key, value;
                for (const auto& attr : entity.DAttributes) {
                    if (attr.first == "k") {
                        key = attr.second;
                    } else if (attr.first == "v") {
                        value = attr.second;
                    }
                }
                if (!key.empty()) {
                    if (currentNode) {
                        currentNode->Attributes[key] = value;
                    } else if (currentWay) {
                        currentWay->Attributes[key] = value;
                    }
                }
            }
        } else if (entity.DType == SXMLEntity::EType::EndElement) {
            if (entity.DNameData == "node" && currentNode) {
                DImplementation->Nodes.push_back(currentNode);
                currentNode = nullptr;
            } else if (entity.DNameData == "way" && currentWay) {
                DImplementation->Ways.push_back(currentWay);
                currentWay = nullptr;
            }
        }
    }
}

// Destructor
COpenStreetMap::~COpenStreetMap() = default;

// Return the total count of nodes
std::size_t COpenStreetMap::NodeCount() const noexcept {
    return DImplementation->Nodes.size();
}

// Return the total count of ways
std::size_t COpenStreetMap::WayCount() const noexcept {
    return DImplementation->Ways.size();
}

// Retrieve node by index
std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->Nodes.size()) {
        return DImplementation->Nodes[index];
    }
    return nullptr;
}

// Retrieve node by ID
std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByID(TNodeID id) const noexcept {
    for (auto& node : DImplementation->Nodes) {
        if (node->ID() == id) {
            return node;
        }
    }
    return nullptr;
}

// Retrieve way by index
std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->Ways.size()) {
        return DImplementation->Ways[index];
    }
    return nullptr;
}

// Retrieve way by ID
std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByID(TWayID id) const noexcept {
    for (auto& way : DImplementation->Ways) {
        if (way->ID() == id) {
            return way;
        }
    }
    return nullptr;
}
