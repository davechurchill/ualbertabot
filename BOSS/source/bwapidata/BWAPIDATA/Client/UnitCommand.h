#pragma once

#include <BWAPIDATA.h>

/**
 * UnitOrder contains a single whole order
 */

namespace BWAPIDATAC
{
  struct UnitCommand
  {
    BWAPIDATA::UnitCommandType type;
    int unitIndex;
    int targetIndex;
    int x;
    int y;
    int extra;
  };
}