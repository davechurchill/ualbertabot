#include <BWAPI.h>
#include "BulletImpl.h"

namespace AKBot
{
  BulletImpl::BulletImpl(int _index, BulletData originalData)
    : data(originalData)
	, self( &data )
	, index(_index)
  {
  }
}