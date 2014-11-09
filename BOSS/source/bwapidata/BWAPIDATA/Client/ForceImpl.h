#pragma once
#include <BWAPIDATA.h>
#include "ForceData.h"
#include <set>
#include <string>

namespace BWAPIDATA
{
  class Game;
  class Player;
  class Unit;
  class ForceImpl : public Force
  {
    private:
      const ForceData* self;
      int id;
    public:
      ForceImpl(int id);
      virtual int getID() const;
      virtual std::string getName() const;
      virtual std::set<Player*> getPlayers() const;
  };
}