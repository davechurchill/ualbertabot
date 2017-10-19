#include "..\stdafx.h"
#include <BWAPI.h>
#include "BulletImpl.h"

namespace AKBot
{
  //------------------------------------------------- GET ID -------------------------------------------------
  int BulletImpl::getID() const
  {
    return self->id;
  }
  //------------------------------------------------ GET PLAYER ----------------------------------------------
  Player BulletImpl::getPlayer() const
  {
	  throw new std::exception("Bullet::getPlayer() does not supported as it is required to maintain global state");
   // return Broodwar->getPlayer(self->player);
  }
  //------------------------------------------------ GET TYPE ------------------------------------------------
  BulletType BulletImpl::getType() const
  {
    return BulletType(self->type);
  }
  //------------------------------------------------ GET SOURCE ----------------------------------------------
  Unit BulletImpl::getSource() const
  {
	  throw new std::exception("Bullet::getSource() does not supported as it is required to maintain global state");
    // return Broodwar->getUnit(self->source);
  }
  //---------------------------------------------- GET POSITION ----------------------------------------------
  Position BulletImpl::getPosition() const
  {
    return Position(self->positionX,self->positionY);
  }
  //----------------------------------------------- GET ANGLE ------------------------------------------------
  double BulletImpl::getAngle() const
  {
    return self->angle;
  }
  //--------------------------------------------- GET VELOCITY X ---------------------------------------------
  double BulletImpl::getVelocityX() const
  {
    return self->velocityX;
  }
  //--------------------------------------------- GET VELOCITY Y ---------------------------------------------
  double BulletImpl::getVelocityY() const
  {
    return self->velocityY;
  }
  //------------------------------------------------ GET TARGET ----------------------------------------------
  Unit BulletImpl::getTarget() const
  {
	  throw new std::exception("Bullet::getTarget() does not supported as it is required to maintain global state");
    //return Broodwar->getUnit(self->target);
  }
  //-------------------------------------------- GET TARGET POSITION -----------------------------------------
  Position BulletImpl::getTargetPosition() const
  {
    return Position(self->targetPositionX,self->targetPositionY);
  }
  //-------------------------------------------- GET REMOVE TIMER --------------------------------------------
  int BulletImpl::getRemoveTimer() const
  {
    return self->removeTimer;
  }
  //------------------------------------------------- EXISTS -------------------------------------------------
  bool BulletImpl::exists() const
  {
    return self->exists;
  }
  //----------------------------------------------- IS VISIBLE -----------------------------------------------
  bool BulletImpl::isVisible(BWAPI::Player player) const
  {
	  if (player == nullptr) {  // Default to self
		  throw new std::exception("Please explicitly use Broodwar->self() instead of nullptr since this test framework does not maintain self player");
	  }

    if ( player != nullptr ) // Only if player is valid
    {
      int id = player->getID();
      if ( id >= 0 && id < 9 ) // Check if ID is valid
        return self->isVisible[id]; // return visibility
    }
    else  // fallback for self also being null
    {
      for(int i = 0; i < 9; ++i) // check if the bullet is visible by ANY player
      {
        if ( self->isVisible[i] )
          return true;
      }
    }
    return false;
  }
}

