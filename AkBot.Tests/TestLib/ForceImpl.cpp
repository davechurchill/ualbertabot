#include <BWAPI.h>
#include "GameImpl.h"
#include "ForceImpl.h"
#include "PlayerImpl.h"
#include "UnitImpl.h"

using namespace BWAPI;

namespace AKBot
{
  ForceImpl::ForceImpl(int _id, ForceData data)
    : data(data)
	, self( &data )
    , id(_id)
  {
  }
  std::string ForceImpl::getName() const
  {
    return std::string(this->self->name);
  }
  Playerset ForceImpl::getPlayers() const
  {
	  throw new std::exception("Force::getPlayers() does not supported as it is required to maintain global state");
    /*Playerset players;
    for (int i = 0; i < std::extent<decltype(BWAPI::BWAPIClient.data->players)>::value; ++i)
      if ( BWAPI::BWAPIClient.data->players[i].force == id )
        players.insert( Broodwar->getPlayer(i) );
    return players;*/
  }
}
