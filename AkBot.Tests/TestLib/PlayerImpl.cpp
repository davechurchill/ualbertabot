#include <BWAPI.h>
#include "GameImpl.h"
#include "ForceImpl.h"
#include "PlayerImpl.h"
#include "UnitImpl.h"

namespace AKBot
{
  PlayerImpl::PlayerImpl(int _id, PlayerData originalData)
    : id(_id)
	, data(originalData)
    , self( &data )
  {
    clear();
  }
  PlayerImpl::PlayerImpl(const PlayerImpl& that)
	  : id(that.id)
	  , data(that.data)
	  , self(&data)
  {
  }
  void PlayerImpl::clear()
  {
    units.clear();
  }
  std::string PlayerImpl::getName() const
  {
    return std::string(self->name);
  }
  Race PlayerImpl::getRace() const
  {
    return Race(self->race);
  }
  PlayerType PlayerImpl::getType() const
  {
    return PlayerType(self->type);
  }
  Force PlayerImpl::getForce() const
  {
    return Broodwar->getForce(self->force);
  }
  bool PlayerImpl::isAlly(const Player player) const
  {
    if ( !player )
      return false;
    return self->isAlly[player->getID()];
  }
  bool PlayerImpl::isEnemy(const Player player) const
  {
    if ( !player )
      return false;
    return self->isEnemy[player->getID()];
  }
  bool PlayerImpl::isNeutral() const
  {
    return self->isNeutral;
  }
  TilePosition PlayerImpl::getStartLocation() const
  {
    return TilePosition(self->startLocationX,self->startLocationY);
  }
  bool PlayerImpl::isVictorious() const
  {
    return self->isVictorious;
  }
  bool PlayerImpl::isDefeated() const
  {
    return self->isDefeated;
  }
}
