#pragma once

#include <Common.h>
#include "MicroManager.h"

namespace UAlbertaBot
{
class MicroManager;

class TransportManager : public MicroManager
{
    BWAPI::UnitInterface*           m_transportShip              = nullptr;
    BWAPI::Position					m_minCorner                  = {-1, -1};
    BWAPI::Position					m_maxCorner                  = {-1, -1};
    BWAPI::Position					m_to                         = {-1, -1};
    BWAPI::Position					m_from                       = {-1, -1};
    int                             m_currentRegionVertexIndex   = -1;
    std::vector<BWAPI::Position>	m_waypoints;
    std::vector<BWAPI::Position>    m_mapEdgeVertices;

    void calculateMapEdgeVertices();
    void drawTransportInformation(int x, int y);
    void moveTransport();
    void moveTroops();
    void followPerimeter(int clockwise=1);
    void followPerimeter(BWAPI::Position to, BWAPI::Position from);

    int getClosestVertexIndex(BWAPI::UnitInterface * unit);
    int getClosestVertexIndex(BWAPI::Position p);
    BWAPI::Position getFleePosition(int clockwise=1);
    std::pair<int, int> findSafePath(BWAPI::Position from, BWAPI::Position to);

public:

    TransportManager();

    void executeMicro(const BWAPI::Unitset & targets);
    void update();
    void setTransportShip(BWAPI::UnitInterface * unit);
    void setFrom(BWAPI::Position from);
    void setTo(BWAPI::Position to);
};
}