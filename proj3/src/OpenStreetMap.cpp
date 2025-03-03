#include "OpenStreetMap.h" // Header file for COpenStreetMap class
#include "XMLReader.h" // Header file for XML parsing functionality
#include <memory> // For smart pointers like std::shared_ptr and std::unique_ptr
#include <vector> // For storing nodes and ways in dynamic arrays
#include <string> // For handling string attributes 
#include <unordered_map> // For storing and searching attributes

// Defining implementation structure first using COpenStreetMap
struct COpenStreetMap::SImplementation {
    // Forward declarations of implementation classes
    class NodeData;
    class WayData;

    // Storing ways and nodes here
    std::vector<std::shared_ptr<NodeData>> NodeList;
    std::vector<std::shared_ptr<WayData>> WayList;
};

// Now we define the implementation classes using CStreetMap::SNode
class COpenStreetMap::SImplementation::NodeData : public CStreetMap::SNode {
public:
    // ID of the node
    TNodeID IDValue;
    // Coordinates of the node or the location
    TLocation LocationValue;
    // Key and value attributes using an unordered map
    std::unordered_map<std::string, std::string> AttributeMap;

    // Getting node's ID
    TNodeID ID() const noexcept override {
        return IDValue;
    }

    // Getting node's location
    TLocation Location() const noexcept override {
        return LocationValue;
    }

    // Number of attributes node has
    std::size_t AttributeCount() const noexcept override {
        return AttributeMap.size();
    }

    // Retrieving key of attribute through index
    std::string GetAttributeKey(std::size_t index) const noexcept override {
        if (index < AttributeMap.size()) {
            auto it = AttributeMap.begin();
            // Iterate to the required position 
            std::advance(it, index);
            // Return the key at the index it's at
            return it->first;
        }
        // Return empty string if index is out of bounds
        return "";
    }

    // Checking to see if the node has an attribute using key
    bool HasAttribute(const std::string &key) const noexcept override {
        return AttributeMap.find(key) != AttributeMap.end();
    }

    // Retrieve the value of attribute if the node has an attribute
    std::string GetAttribute(const std::string &key) const noexcept override {
        auto it = AttributeMap.find(key);
        if (it != AttributeMap.end()) {
            return it->second;
        }
        // If out of bounds return ""
        return "";
    }
};

// Way class, when using CStreetMap::SWay
class COpenStreetMap::SImplementation::WayData : public CStreetMap::SWay {
public:
    // ID of way
    TWayID IDValue;
    // Nodes IDs
    std::vector<TNodeID> NodeIDList;
    // Attributes for key and value using unordered map
    std::unordered_map<std::string, std::string> AttributeMap;

    // Getting the way's ID
    TWayID ID() const noexcept override {
        return IDValue;
    }

    // Number of nodes in way
    std::size_t NodeCount() const noexcept override {
        return NodeIDList.size();
    }

    // Getting Node ID through index
    TNodeID GetNodeID(std::size_t index) const noexcept override {
        if (index < NodeIDList.size()) {
            return NodeIDList[index];
        }
        return CStreetMap::InvalidNodeID;
    }

    // Number of attributes way has
    std::size_t AttributeCount() const noexcept override {
        return AttributeMap.size();
    }

    // Retrieving key of attribute through index
    std::string GetAttributeKey(std::size_t index) const noexcept override {
        if (index < AttributeMap.size()) {
            auto it = AttributeMap.begin();
            // Iterate to the required position 
            std::advance(it, index);
            // Return the key at the index it's at
            return it->first;
        }
        // Return empty string if index is out of bounds
        return "";
    }

    // Checking to see if the way has an attribute using key
    bool HasAttribute(const std::string &key) const noexcept override {
        return AttributeMap.find(key) != AttributeMap.end();
    }

    // Retrieve the value of attribute if the way has an attribute
    std::string GetAttribute(const std::string &key) const noexcept override {
        auto it = AttributeMap.find(key);
        if (it != AttributeMap.end()) {
            return it->second;
        }
        // If out of bounds return ""
        return "";
    }
};

// Initialize the implementation
COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> src) {
    // std::make_unique ensures exclusive ownership of the SImplementation instance
    // Initializing it to DImplementation
    DImplementation = std::make_unique<SImplementation>();

    SXMLEntity entity;
    // std::shared_ptr is used here to allow multiple 
    // parts of the program to share ownership of nodes
    std::shared_ptr<SImplementation::NodeData> currentNode = nullptr;
    // std::shared_ptr for ways allows for multiple references to the same way 
    // without worrying about manual memory management
    std::shared_ptr<SImplementation::WayData> currentWay = nullptr;

    // Parsing the XML file
    while (src->ReadEntity(entity)) {
        if (entity.DType == SXMLEntity::EType::StartElement) {
            if (entity.DNameData == "node") {
                // Creating a new node instance
                // Using std::make_shared optimizes memory allocation and ensures exception safety
                currentNode = std::make_shared<SImplementation::NodeData>();
                currentWay = nullptr;

                // Process the node attributes through a for loop
                for (const auto& attr : entity.DAttributes) {
                    // In OSM XML file, id, lat, and lon are used as node attributes
                    if (attr.first == "id") {
                        currentNode->IDValue = std::stoull(attr.second);
                    } else if (attr.first == "lat") {
                        // std::stod is a simpler way of returning a double while
                        // trying to also check for errors such as
                        // std::invalid_argument and std::out_of_range
                        currentNode->LocationValue.first = std::stod(attr.second);
                    } else if (attr.first == "lon") {
                        currentNode->LocationValue.second = std::stod(attr.second);
                    } else {
                        // Store other attributes if needed
                        currentNode->AttributeMap[attr.first] = attr.second;
                    }
                }
            } else if (entity.DNameData == "way") {
                // Creating a new way instance
                currentWay = std::make_shared<SImplementation::WayData>();
                currentNode = nullptr;

                // Process way attributes
                for (const auto& attr : entity.DAttributes) {
                    if (attr.first == "id") {
                        currentWay->IDValue = std::stoull(attr.second);
                    } else {
                        // Store other attributes again
                        currentWay->AttributeMap[attr.first] = attr.second;
                    }
                }
            } else if (entity.DNameData == "nd" && currentWay) {
                // Process node reference inside a way
                // Using std::stoull since converting string to unsigned long long
                // when reading attribute
                for (const auto& attr : entity.DAttributes) {
                    if (attr.first == "ref") {
                        currentWay->NodeIDList.push_back(std::stoull(attr.second));
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
                // Pretty self-explanatory
                if (!key.empty()) {
                    if (currentNode) {
                        currentNode->AttributeMap[key] = value;
                    } else if (currentWay) {
                        currentWay->AttributeMap[key] = value;
                    }
                }
            }
        } else if (entity.DType == SXMLEntity::EType::EndElement) {
            if (entity.DNameData == "node" && currentNode) {
                // Store completed node
                DImplementation->NodeList.push_back(currentNode);
                currentNode = nullptr;
            } else if (entity.DNameData == "way" && currentWay) {
                // Store completed way
                DImplementation->WayList.push_back(currentWay);
                currentWay = nullptr;
            }
        }
    }
}

// Destructor
COpenStreetMap::~COpenStreetMap() = default;

// Return the total count of nodes
std::size_t COpenStreetMap::NodeCount() const noexcept {
    return DImplementation->NodeList.size();
}

// Return the total count of ways
std::size_t COpenStreetMap::WayCount() const noexcept {
    return DImplementation->WayList.size();
}

// Retrieve node by index
std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->NodeList.size()) {
        return DImplementation->NodeList[index];
    }
    return nullptr; // If index is out of bounds do this
}

// Retrieve node by ID
std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByID(TNodeID id) const noexcept {
    for (auto& node : DImplementation->NodeList) {
        if (node->ID() == id) {
            return node;
        }
    }
    return nullptr; // If no node with matching ID do this
}

// Retrieve way by index
std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->WayList.size()) {
        return DImplementation->WayList[index];
    }
    return nullptr; // If index is out of bounds
}

// Retrieve way by ID
std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByID(TWayID id) const noexcept {
    for (auto& way : DImplementation->WayList) {
        if (way->ID() == id) {
            return way;
        }
    }
    return nullptr; // If no way with matching ID do this
}