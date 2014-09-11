#include "AllPlayers.h"
    
using namespace SparCraft;

Player * AllPlayers::getPlayer(const IDType & playerID, const IDType & type)
{
	if			(type == PlayerModels::AttackClosest)		{ return new Player_AttackClosest(playerID); }
	else if		(type == PlayerModels::AttackDPS)			{ return new Player_AttackDPS(playerID); }
	else if		(type == PlayerModels::AttackWeakest)		{ return new Player_AttackWeakest(playerID); }
	else if		(type == PlayerModels::Kiter)				{ return new Player_Kiter(playerID); }
	else if		(type == PlayerModels::KiterDPS)			{ return new Player_KiterDPS(playerID); }
    else if		(type == PlayerModels::Kiter_NOKDPS)		{ return new Player_Kiter_NOKDPS(playerID); }
    else if		(type == PlayerModels::Cluster)		    	{ return new Player_Cluster(playerID); }
	else if		(type == PlayerModels::NOKDPS)              { return new Player_NOKDPS(playerID); }
	else if		(type == PlayerModels::Random)				{ return new Player_Random(playerID); }
	else													{ return NULL; }
}

PlayerPtr AllPlayers::getPlayerPtr(const IDType & playerID, const IDType & type)
{
	if			(type == PlayerModels::AttackClosest)		{ return PlayerPtr(new Player_AttackClosest(playerID)); }
	else if		(type == PlayerModels::AttackDPS)			{ return PlayerPtr(new Player_AttackDPS(playerID)); }
	else if		(type == PlayerModels::AttackWeakest)		{ return PlayerPtr(new Player_AttackWeakest(playerID)); }
	else if		(type == PlayerModels::Kiter)				{ return PlayerPtr(new Player_Kiter(playerID)); }
	else if		(type == PlayerModels::KiterDPS)			{ return PlayerPtr(new Player_KiterDPS(playerID)); }
    else if		(type == PlayerModels::Kiter_NOKDPS)		{ return PlayerPtr(new Player_Kiter_NOKDPS(playerID)); }
    else if		(type == PlayerModels::Cluster)		    	{ return PlayerPtr(new Player_Cluster(playerID)); }
	else if		(type == PlayerModels::NOKDPS)              { return PlayerPtr(new Player_NOKDPS(playerID)); }
	else if		(type == PlayerModels::Random)				{ return PlayerPtr(new Player_Random(playerID)); }
	else													{ return PlayerPtr(new Player_NOKDPS(playerID)); }
}
