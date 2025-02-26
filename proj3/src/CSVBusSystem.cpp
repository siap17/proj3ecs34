#include "CSVBusSystem.h"
#include <vector>
#include <memory>

// Private Implementation
struct CCSVBusSystem::SImplementation {
    // Data members for stops and routes
};

CCSVBusSystem::CCSVBusSystem(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader> routesrc) 
    : DImplementation(std::make_unique<SImplementation>()) {}

CCSVBusSystem::~CCSVBusSystem() = default;

std::size_t CCSVBusSystem::StopCount() const noexcept { return 0; }
std::size_t CCSVBusSystem::RouteCount() const noexcept { return 0; }
std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByIndex(std::size_t index) const noexcept { return nullptr; }
std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByID(TStopID id) const noexcept { return nullptr; }
std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByIndex(std::size_t index) const noexcept { return nullptr; }
std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByName(const std::string &name) const noexcept { return nullptr; }
