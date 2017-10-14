#pragma once
#include <string>
#include <BWAPI/BulletType.h>
#include <BWAPI/Position.h>
#include <BWAPI/Unit.h>
#include <BWAPI/Client/BulletData.h>

namespace AKBot
{
  class BWAPI::PlayerInterface;
  typedef BWAPI::PlayerInterface *Player;
  using BWAPI::BulletInterface;
  using BWAPI::BulletData;
  using BWAPI::BulletType;
  using BWAPI::Position;
  using BWAPI::Unit;

  class BulletImpl : public BulletInterface
  {
    private:
		BulletData data;
      const BulletData* self;
      int index;
    public:

      BulletImpl(int index, BulletData data);
      virtual int getID() const override;
      virtual bool exists() const override;
      virtual Player getPlayer() const override;
      virtual BulletType getType() const override;
      virtual Unit getSource() const override;
      virtual Position getPosition() const override;
      virtual double getAngle() const override;
      virtual double getVelocityX() const override;
      virtual double getVelocityY() const override;
      virtual Unit getTarget() const override;
      virtual Position getTargetPosition() const override;
      virtual int getRemoveTimer() const override;
      virtual bool isVisible(Player player = nullptr) const override;
  };
}
