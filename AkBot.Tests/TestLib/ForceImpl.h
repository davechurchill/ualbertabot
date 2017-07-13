#pragma once
#include <BWAPI.h>
#include <BWAPI\Client\ForceData.h>
#include <string>

namespace AKBot
{
	using namespace BWAPI;

  class ForceImpl : public ForceInterface
  {
    private:
		ForceData data;
      const ForceData* self;
      int id;
    public:
      ForceImpl(int id, ForceData data);
      virtual int getID() const override;
      virtual std::string getName() const override;
      virtual Playerset getPlayers() const override;
  };
}