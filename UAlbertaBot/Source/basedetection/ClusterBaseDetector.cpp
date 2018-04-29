#include "ClusterBaseDetector.h"
#include <BWAPI/Unit.h>
#include <BWAPI/Unitset.h>
#include "../UnitUtil.h"
#include "../BaseLocation.h"
#include "../Distance.h"

using namespace UAlbertaBot;

void addNewResourceClusters(
	std::vector<std::vector<BWAPI::Unit>>& resourceClusters,
	const BWAPI::Unitset& resources,
	AKBot::DistanceFunction distanceFunction,
	const int resourceThreshold,
	const int clusterDistance)
{
	for (auto & mineral : resources)
	{
		// skip minerals that don't have more than 100 starting minerals
		// these are probably stupid map-blocking minerals to confuse us
		if (mineral->getResources() <= resourceThreshold)
		{
			continue;
		}

		bool foundCluster = false;
		for (std::vector<BWAPI::Unit> & cluster : resourceClusters)
		{
			int groundDist = distanceFunction(mineral->getPosition(), UnitUtil::GetUnitsetCenter(cluster));

			if (groundDist >= 0 && groundDist < clusterDistance)
			{
				cluster.push_back(mineral);
				foundCluster = true;
				break;
			}
		}

		if (!foundCluster)
		{
			resourceClusters.push_back(std::vector<BWAPI::Unit>());
			resourceClusters.back().push_back(mineral);
		}
	}
}

void addToExistingResourceClusters(
	std::vector<std::vector<BWAPI::Unit>>& resourceClusters,
	const BWAPI::Unitset& resources,
	AKBot::DistanceFunction distanceFunction,
	const int resourceThreshold,
	const int clusterDistance)
{
	for (auto & geyser : resources)
	{
		if (geyser->getResources() <= resourceThreshold)
		{
			continue;
		}

		for (std::vector<BWAPI::Unit> & cluster : resourceClusters)
		{
			int groundDist = distanceFunction(geyser->getPosition(), UnitUtil::GetUnitsetCenter(cluster));

			if (groundDist >= 0 && groundDist < clusterDistance)
			{
				cluster.push_back(geyser);
				break;
			}
		}
	}
}

AKBot::ClusterBaseDetector::ClusterBaseDetector(
	BWAPI::Game* game,
	std::shared_ptr<AKBot::OpponentView> opponentView,
	std::shared_ptr<UAlbertaBot::MapTools> map)
	: _map(map), _game(game), _opponentView(opponentView)
{
}

void AKBot::ClusterBaseDetector::detectBases(std::vector<UAlbertaBot::BaseLocation>& baseLocations) const
{// a BaseLocation will be anything where there are minerals to mine
    // so we will first look over all minerals and cluster them based on some distance
	const int clusterDistance = 20;
	const int mineralThreshold = 100;
	const int gasThreshold = 100;

	auto distanceFunction = [this](const BWAPI::Position & src, const BWAPI::Position & dest)
	{
		return _map->getGroundDistance(src, dest);
	};
	// stores each cluster of resources based on some ground distance
	std::vector<std::vector<BWAPI::Unit>> resourceClusters;
	addNewResourceClusters(resourceClusters, _game->getStaticMinerals(), distanceFunction, mineralThreshold, clusterDistance);
	addNewResourceClusters(resourceClusters, _game->getMinerals(), distanceFunction, mineralThreshold, clusterDistance);
	// add geysers only to existing resource clusters
	addToExistingResourceClusters(resourceClusters, _game->getStaticGeysers(), distanceFunction, gasThreshold, clusterDistance);
	addToExistingResourceClusters(resourceClusters, _game->getGeysers(), distanceFunction, gasThreshold, clusterDistance);

	// add the base locations if there are more than 4 resouces in the cluster
	int baseID = 0;
	for (auto & cluster : resourceClusters)
	{
		if (cluster.size() > 4)
		{
			baseLocations.push_back(BaseLocation(_opponentView, _map, baseID++, cluster));
		}
	}
}
