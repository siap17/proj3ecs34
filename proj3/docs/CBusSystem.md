The CBusSystem is a class that CCSVBusSystem inherits from. SStop and SRoute inherit 
from CBusSystem::SStop and CBusSystem::SRoute. 

The CBusSystem essentially has pure virtual functions that's why everything equals 0 adn must have some derived class. 
It also contains nested classes like SStop and SRoute and supports polymorphism. 

My CCSVBusSystem inherits the StopCount(), RouteCount(), StopByIndex(), StopByID(), RouteByIndex() and RouteByName() 
from the CBusSystem.h. This ultimately reflects the polymorphism and implementation methods of CBusSystem although CCSVBusSystem overrides these methods 
in the first few lines of the code. 


Here is the code from CBusSystem.h: 
#ifndef BUSROUTE_H
#define BUSROUTE_H

#include "StreetMap.h"
#include <string> 
#include <memory> 
#include <limits> 

class CBusSystem{
    public:
        using TStopID = uint64_t;

        static const TStopID InvalidStopID = std::numeric_limits<TStopID>::max();

        struct SStop{
            virtual ~SStop(){};
            virtual TStopID ID() const noexcept = 0;
            virtual CStreetMap::TNodeID NodeID() const noexcept= 0;
        };

        struct SRoute{
            virtual ~SRoute(){};
            virtual std::string Name() const noexcept = 0;
            virtual std::size_t StopCount() const noexcept = 0;
            virtual TStopID GetStopID(std::size_t index) const noexcept = 0;
        };

        virtual ~CBusSystem(){};

        virtual std::size_t StopCount() const noexcept = 0;
        virtual std::size_t RouteCount() const noexcept = 0;
        virtual std::shared_ptr<SStop> StopByIndex(std::size_t index) const noexcept = 0;
        virtual std::shared_ptr<SStop> StopByID(TStopID id) const noexcept = 0;
        virtual std::shared_ptr<SRoute> RouteByIndex(std::size_t index) const noexcept = 0;
        virtual std::shared_ptr<SRoute> RouteByName(const std::string &name) const noexcept = 0;
};

#endif
