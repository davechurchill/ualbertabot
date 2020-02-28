#pragma once

#include "Common.h"
#include "DMap.h"
#include <map>
#include <vector>

namespace UAlbertaBot
{

class BaseLocation
{
	DMap						m_distanceMap;

	BWAPI::TilePosition			m_depotPosition;
	BWAPI::Position				m_centerOfResources;
	std::vector<BWAPI::Unit>	m_geysers;
	std::vector<BWAPI::Unit>	m_minerals;

	std::vector<BWAPI::Position> m_mineralPositions;
	std::vector<BWAPI::Position> m_geyserPositions;

	std::map<BWAPI::Player, bool> m_isPlayerOccupying;
	std::map<BWAPI::Player, bool> m_isPlayerStartLocation;

	int                         m_baseID;
	int							m_left;
	int							m_right;
	int							m_top;
	int							m_bottom;
	bool                        m_isStartLocation;

public:

	BaseLocation(int baseID, const std::vector<BWAPI::Unit> & resources);

	int getGroundDistance(const BWAPI::Position & pos) const;
	int getGroundDistance(const BWAPI::TilePosition & pos) const;
	bool isStartLocation() const;
	bool isPlayerStartLocation(BWAPI::Player player) const;
	bool isMineralOnly() const;
	bool containsPosition(const BWAPI::Position & pos) const;
	const BWAPI::TilePosition & getDepotPosition() const;
	const BWAPI::Position & getPosition() const;
	const std::vector<BWAPI::Unit> & getGeysers() const;
	const std::vector<BWAPI::Unit> & getMinerals() const;
	bool isOccupiedByPlayer(BWAPI::Player player) const;
	bool isExplored() const;
	bool isInResourceBox(int x, int y) const;

	void setPlayerOccupying(BWAPI::Player player, bool occupying);

	const std::vector<BWAPI::TilePosition> & getClosestTiles() const;

	void draw();
};
}