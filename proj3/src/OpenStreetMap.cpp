#include "OpenStreetMap.h"  
#include "XMLReader.h"
#include <expat.h>          
#include <vector>           
#include <unordered_map>    
#include <string>           
#include <memory>          
#include <cstring>          

struct COpenStreetMap::SImplementation {  
    class CNode : public CStreetMap::SNode {  // node class that implements the SNode interface
        private:
            TNodeID DID;  // node id
            TLocation DLocation;  // node location (latitude, longitude)
            std::unordered_map<std::string, std::string> DAttributes;  // map of node attributes
            std::vector<std::string> DAttributeKeys;  // list of attribute keys for ordering

        public:
            CNode(TNodeID id, double lat, double lon) : DID(id), DLocation(std::make_pair(lat, lon)) {}  // constructor for node

            TNodeID ID() const noexcept override {  // get node id
                return DID;
            }

            TLocation Location() const noexcept override {  // get node location
                return DLocation;
            }

            std::size_t AttributeCount() const noexcept override {  // getting the # of attributes
                return DAttributeKeys.size();
            }

            std::string GetAttributeKey(std::size_t index) const noexcept override {  // get attribute key at a specific index
                if(index < DAttributeKeys.size()) {
                    return DAttributeKeys[index];
                }
                return "";  // ret empty string if index is out of bounds
            }

            bool HasAttribute(const std::string &key) const noexcept override {  // check if node has a specific attribute
                return DAttributes.find(key) != DAttributes.end();
            }

            std::string GetAttribute(const std::string &key) const noexcept override {  // get the val of a specific attribute
                auto Search = DAttributes.find(key);
                if(Search != DAttributes.end()) {
                    return Search->second;
                }
                return "";  // ret empty string if attribute doesn't exist
            }

            void AddAttribute(const std::string &key, const std::string &value) {  // add an attribute to the node
                if(DAttributes.find(key) == DAttributes.end()) {  // if key not exist, add it to the keys list
                    DAttributeKeys.push_back(key);
                }
                DAttributes[key] = value;  // add or update the attribute
            }
    };


    class CWay : public CStreetMap::SWay {  // way class that implements the SWay interface
        private:
            TWayID DID;  // way id
            std::vector<TNodeID> DNodeIDs;  // list of node ids in the way
            std::unordered_map<std::string, std::string> DAttributes;  // map of way attributes
            std::vector<std::string> DAttributeKeys;  // list of attribute keys for ordering

        public:
            CWay(TWayID id) : DID(id) {}  // constructor for way

            TWayID ID() const noexcept override {  // get way id
                return DID;
            }

            std::size_t NodeCount() const noexcept override {  // get # of nodes in the way
                return DNodeIDs.size();
            }

            TNodeID GetNodeID(std::size_t index) const noexcept override {  // get node id at a specific index
                if(index < DNodeIDs.size()) {
                    return DNodeIDs[index];
                }
                return CStreetMap::InvalidNodeID;  // ret invalid id if index is out of bounds
            }

            std::size_t AttributeCount() const noexcept override {  // get # of attributes
                return DAttributeKeys.size();
            }

            std::string GetAttributeKey(std::size_t index) const noexcept override {  // get attribute key at specific index
                if(index < DAttributeKeys.size()) {
                    return DAttributeKeys[index];
                }
                return "";  // ret empty string if index is out of bounds
            }

            bool HasAttribute(const std::string &key) const noexcept override {  // check if the way has a specific attribute
                return DAttributes.find(key) != DAttributes.end();
            }

            std::string GetAttribute(const std::string &key) const noexcept override {  // get val of a specific attribute
                auto Search = DAttributes.find(key);
                if(Search != DAttributes.end()) {
                    return Search->second;
                }
                return "";  // ret empty string if attribute doesn't exist
            }

            void AddNodeID(TNodeID id) {  // add a node id to the way
                DNodeIDs.push_back(id);
            }

            void AddAttribute(const std::string &key, const std::string &value) {  // add an attribute to the way
                if(DAttributes.find(key) == DAttributes.end()) {  // if key not exist, add to the keys list
                    DAttributeKeys.push_back(key);
                }
                DAttributes[key] = value;  // add/update the attribute
            }
    };

    // storage for nodes , ways
    std::vector<std::shared_ptr<CNode>> DNodes;  // list of nodes
    std::vector<std::shared_ptr<CWay>> DWays;  // list of ways
    std::unordered_map<TNodeID, std::shared_ptr<CNode>> DNodeMap;  // map of node ids to nodes
    std::unordered_map<TWayID, std::shared_ptr<CWay>> DWayMap;  // map of way ids to ways

    // state variables for xml parsing
    XML_Parser DParser;  // expat xml parser
    bool DParsingNode = false;  //  to track if we parsing a node
    bool DParsingWay = false;  //  to track if parsing a way
    std::shared_ptr<CNode> DCurrentNode;  // curr node being parsed
    std::shared_ptr<CWay> DCurrentWay;  // curr way being parsed

    // constructor that takes an xml reader
    SImplementation(std::shared_ptr<CXMLReader> src) {
        DParser = XML_ParserCreate(NULL);  // create the expat parser
        if (!DParser) {
            throw std::runtime_error("Failed to create XML parser");  // throw error if parser creation fails
        }

        XML_SetUserData(DParser, this);  // set user data for the parser
        XML_SetElementHandler(DParser, StartElementHandler, EndElementHandler);  // set handlers for start and end elements

        // parse the xml document
        SXMLEntity entity;
        while (src->ReadEntity(entity, false)) {  // read entities from the xml reader
            if (entity.DType == SXMLEntity::EType::StartElement || entity.DType == SXMLEntity::EType::EndElement) {
                const char *name = entity.DNameData.c_str();  // get the element name
                const char **attrs = nullptr;  // initialize attributes

                // convert attributes to expat format
                if (!entity.DAttributes.empty()) {
                    std::vector<const char *> attrPtrs;  // vector to hold attribute pointers
                    for (const auto &attr : entity.DAttributes) {
                        attrPtrs.push_back(attr.first.c_str());  // add attribute key
                        attrPtrs.push_back(attr.second.c_str());  // add attribute value
                    }
                    attrPtrs.push_back(nullptr);  // null-terminate the array
                    attrs = attrPtrs.data();  // set attrs to the array
                }

                if (entity.DType == SXMLEntity::EType::StartElement) {  // handle start element
                    StartElementHandler(this, name, attrs);
                } else if (entity.DType == SXMLEntity::EType::EndElement) {  //  end element
                    EndElementHandler(this, name);
                }
            }
        }

        //  to indicate end of document
        if (XML_Parse(DParser, "", 0, 1) == XML_STATUS_ERROR) {  // parse the final chunk
            std::string error = XML_ErrorString(XML_GetErrorCode(DParser));  // get  error message
            XML_ParserFree(DParser);  // free the parser
            throw std::runtime_error("XML parsing error at end: " + error);  // throw error if parsing fails
        }

        XML_ParserFree(DParser);  // free the parser
    }

    // xml start element handler
    static void StartElementHandler(void *userData, const char *name, const char **attrs) {
        SImplementation *impl = static_cast<SImplementation*>(userData);  // cast user data to implementation
        
        if (strcmp(name, "node") == 0) {  // if the element is a node
            impl->DParsingNode = true;  // set parsing node flag
            
            // here we r getting the node attributes
            TNodeID id = CStreetMap::InvalidNodeID;  // initialize node id
            double lat = 0.0, lon = 0.0;  // initialize lat and long
            
            for (int i = 0; attrs[i]; i += 2) {  // loop through attributes
                if (strcmp(attrs[i], "id") == 0) {  // if attribute is id
                    id = std::stoull(attrs[i+1]);  // set node id
                } else if (strcmp(attrs[i], "lat") == 0) {  // if attribute is lat
                    lat = std::stod(attrs[i+1]);  // set latitude
                } else if (strcmp(attrs[i], "lon") == 0) {  // if attribute is lon
                    lon = std::stod(attrs[i+1]);  // set longitude
                }
            }
            
            if (id != CStreetMap::InvalidNodeID) {  // if node id is valid
                impl->DCurrentNode = std::make_shared<CNode>(id, lat, lon);  // create a new node
            }
        } else if (strcmp(name, "way") == 0) {  // if the element is a way
            impl->DParsingWay = true;  // set parsing way flag
            
            // extract way id
            TWayID id = CStreetMap::InvalidWayID;  // initialize way id
            
            for (int i = 0; attrs[i]; i += 2) {  // loop through attributes
                if (strcmp(attrs[i], "id") == 0) {  // if attribute is id
                    id = std::stoull(attrs[i+1]);  // set way id
                    break;
                }
            }
            
            if (id != CStreetMap::InvalidWayID) {  // if way id is valid
                impl->DCurrentWay = std::make_shared<CWay>(id);  // create a new way
            }
        } else if (strcmp(name, "nd") == 0 && impl->DParsingWay && impl->DCurrentWay) {  // if the element is a node reference in a way
            // extract node reference in way
            for (int i = 0; attrs[i]; i += 2) {  // loop through attributes
                if (strcmp(attrs[i], "ref") == 0) {  // if attribute is ref
                    TNodeID nodeId = std::stoull(attrs[i+1]);  // set node id
                    impl->DCurrentWay->AddNodeID(nodeId);  // add node id to the way
                    break;
                }
            }
        } else if (strcmp(name, "tag") == 0) {  // if the element is a tag
            // gey tag key-value pair
            std::string key, value;  // initialize key and value
            
            for (int i = 0; attrs[i]; i += 2) {  // loop through attributes
                if (strcmp(attrs[i], "k") == 0) {  // if attribute is k
                    key = attrs[i+1];  // set key
                } else if (strcmp(attrs[i], "v") == 0) {  // if attribute is v
                    value = attrs[i+1];  // set value
                }
            }
            
            if (!key.empty()) {  // if key is not empty
                if (impl->DParsingNode && impl->DCurrentNode) {  // if parsing a node
                    impl->DCurrentNode->AddAttribute(key, value);  // add attribute to the node
                } else if (impl->DParsingWay && impl->DCurrentWay) {  // if parsing a way
                    impl->DCurrentWay->AddAttribute(key, value);  // add attribute to the way
                }
            }
        }
    }

    // xml , element handler
    static void EndElementHandler(void *userData, const char *name) {
        SImplementation *impl = static_cast<SImplementation*>(userData);  // cast user data to implementation
        
        if (strcmp(name, "node") == 0) {  // if the element is a node
            impl->DParsingNode = false;  // reset parsing node flag
            
            if (impl->DCurrentNode) {  // if curr node exists
                TNodeID id = impl->DCurrentNode->ID();  // get node id
                impl->DNodes.push_back(impl->DCurrentNode);  // add node to the list
                impl->DNodeMap[id] = impl->DCurrentNode;  // add node to  map
                impl->DCurrentNode = nullptr;  // reset curr node
            }
        } else if (strcmp(name, "way") == 0) {  // if the element is a way
            impl->DParsingWay = false;  // reset parsing way flag
            
            if (impl->DCurrentWay) {  // if current way exists
                TWayID id = impl->DCurrentWay->ID();  // get way id
                impl->DWays.push_back(impl->DCurrentWay);  // add way to the list
                impl->DWayMap[id] = impl->DCurrentWay;  // add way to the map
                impl->DCurrentWay = nullptr;  // reset current way
            }
        }
    }
};

// constructor
COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> src) {
    DImplementation = std::make_unique<SImplementation>(src);  
}

// destructor
COpenStreetMap::~COpenStreetMap() = default; 

// returns the number of nodes in the map
std::size_t COpenStreetMap::NodeCount() const noexcept {
    return DImplementation->DNodes.size();  // ret the num of nodes
}

// returns the number of ways in the map
std::size_t COpenStreetMap::WayCount() const noexcept {
    return DImplementation->DWays.size();  // return the num of ways
}

// returns the SNode associated with index
std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->DNodes.size()) {  // if index is valid
        return DImplementation->DNodes[index];  // return the node at the index
    }
    return nullptr;  // return nullptr if index is out of bounds
}

// returns the SNode with the id
std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByID(TNodeID id) const noexcept {
    auto Search = DImplementation->DNodeMap.find(id);  // find the node in the map
    if (Search != DImplementation->DNodeMap.end()) {  // if node exists
        return Search->second;  // return the node
    }
    return nullptr;  // ret nullptr if node not exist
}

// ret the SWay associated with index
std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByIndex(std::size_t index) const noexcept {
    if (index < DImplementation->DWays.size()) {  // if index is valid
        return DImplementation->DWays[index];  // ret the way at the index
    }
    return nullptr;  // ret nullptr if index is out of bounds
}

// returns the SWay with the id
std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByID(TWayID id) const noexcept {
    auto Search = DImplementation->DWayMap.find(id);  // find the way in the map
    if (Search != DImplementation->DWayMap.end()) {  // if way exists
        return Search->second;  // return the way
    }
    return nullptr;  // return nullptr if way doesn't exist
}
