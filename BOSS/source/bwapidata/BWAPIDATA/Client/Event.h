#pragma once

#include <BWAPIDATA\Event.h>
#include <BWAPIDATA\EventType.h>

namespace BWAPIDATAC
{ 
  struct Event
  {
    BWAPIDATA::EventType::Enum type;
    int v1;
    int v2;
  };
}