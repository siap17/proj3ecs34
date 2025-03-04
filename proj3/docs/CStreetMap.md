The CStreetMap class is an abstract interface for handling street map data, defining how nodes (the intersections) and ways (the streets) are represented. It introduces TNodeID and TWayID as unique identifiers and uses TLocation to store lat-longi coordinates. It also defines InvalidNodeID and InvalidWayID to represent uninitialized or invalid entries.

There were two key structures, SNode and SWay, that handled individual intersections and streets. SNode provides methods to get a node’s ID, location, and attributes, while SWay does the same for streets, allowing access to their nodes and metadata.

Since CStreetMap is an abstract class, it doesn’t store data itself but provides functions to retrieve nodes and ways by index or ID. These return std::shared_ptr objects for safer memory management. To actually use a map, a concrete implementation like COpenStreetMap is needed.

For example, with COpenStreetMap, we can load an .osm file, get the total number of nodes and ways, and look up specific intersections or streets by ID. This setup makes CStreetMap useful for things like navigation, route planning, and geographic analysis.
