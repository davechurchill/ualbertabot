#include <BWAPI.h>
#include "BulletImpl.h"

namespace AKBot
{
  BulletImpl::BulletImpl(int _index, BulletData data)
    : data(data)
	, self( &data )
	, index(_index)
  {
  }
}