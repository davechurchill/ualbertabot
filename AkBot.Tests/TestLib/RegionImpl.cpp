#include <BWAPI/Client/Client.h>
#include "RegionImpl.h"

namespace AKBot
{
  RegionImpl::RegionImpl(int index, BWAPI::RegionData data)
    : data(data), self ( &data )
  { }
  void RegionImpl::setNeighbors()
  {
    int accessibleBestDist    = 99999;
    int inaccessibleBestDist  = 99999;
    for ( int i = 0; i < self->neighborCount; ++i )
    {
      BWAPI::Region neighbor = Broodwar->getRegion(self->neighbors[i]);
      this->neighbors.insert(neighbor);

      // Obtain the closest accessible and inaccessible Regions from their Region center
      int dst = this->getCenter().getApproxDistance(neighbor->getCenter());
      if ( this->getRegionGroupID() == neighbor->getRegionGroupID() )
      {
        if ( dst < accessibleBestDist )
        {
          accessibleBestDist = dst;
          this->closestAccessibleRgn = neighbor;
        }
      }
      else if ( dst < inaccessibleBestDist )
      {
        inaccessibleBestDist = dst;
        this->closestInaccessibleRgn = neighbor;
      }
    }
  }
};
