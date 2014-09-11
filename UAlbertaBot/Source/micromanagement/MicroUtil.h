#pragma once

#include <Common.h>
#include <BWAPI.h>

namespace UAlbertaBot
{
class MicroUtil
{
public:

  const static double PI;
  static void rotate(double &x, double &y, double angle);

  static void normalize(double &x, double &y);

};
}