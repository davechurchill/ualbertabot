#pragma once

#include "BWAPI.h"
#include <fstream>
#include <iostream>
#include <vector>

class UnitFrameData
{
    BWAPI::UnitType m_type;
    BWAPI::Position m_pos;
    BWAPI::Order    m_order;
    BWAPI::Position m_orderPos;

    int     m_id     = 0;
    int     m_player = 0;
    int     m_hp     = 0;
    int     m_shields= 0;
    int     m_energy = 0;
    double  m_angle  = 0;

public:

    UnitFrameData(BWAPI::Unit unit)
        : m_type     (unit->getType())
        , m_pos      (unit->getPosition())
        , m_order    (unit->getOrder())
        , m_orderPos (unit->getOrderTargetPosition())
        , m_id       (unit->getID())
        , m_player   (unit->getPlayer()->getID())
        , m_hp       (unit->getHitPoints())
        , m_shields  (unit->getShields())
        , m_energy   (unit->getEnergy())
        , m_angle    (unit->getAngle())
    {

    }

    void writeToFile(std::ofstream & fout) const
    {
        fout << m_player << " ";         // 1 byte
        fout << m_type.getID() << " ";   // 1 byte
        fout << m_id << " ";             // 2 byte
        fout << m_hp << " ";             // 2 byte
        fout << m_shields << " ";        // 2 byte
        fout << m_pos.x << " ";          // 2 byte
        fout << m_pos.y << " ";          // 2 byte
    }
};

class GameFrame
{
    int                         m_frame = 0;
    std::vector<UnitFrameData>  m_units;

public:

    GameFrame()
        : m_frame(BWAPI::Broodwar->getFrameCount())
    {
        for (const BWAPI::Unit & unit : BWAPI::Broodwar->getAllUnits())
        {
            if (unit->getPlayer()->getID() != -1)
            {
                m_units.push_back(UnitFrameData(unit));
            }
        }
    }

    int getFrame() const
    {
        return m_frame;
    }

    void writeToFile(std::ofstream & fout) const
    {
        fout << "f " << m_frame << "\n";
        for (const auto & unit : m_units)
        {
            unit.writeToFile(fout);
            fout << "\n";
        }
    }
};

class GameMap
{
    int                 m_width  = 0;
    int                 m_height = 0;
    std::vector<int>    m_walkable;

public:

    GameMap()
        : m_width(BWAPI::Broodwar->mapWidth() * 4)
        , m_height(BWAPI::Broodwar->mapHeight() * 4)
        , m_walkable(BWAPI::Broodwar->mapHeight() * BWAPI::Broodwar->mapWidth() * 16, 0)
    {
        for (int h(0); h < m_height; ++h)
        {
            for (int w(0); w < m_width; ++w)
            {
                m_walkable[h*w + w] = BWAPI::Broodwar->isWalkable(h, w);
            }
        }
    }

    void writeToFile(std::ofstream & fout) const
    {

    }
};

class GameHistory
{
    GameMap                 m_map;
    std::vector<GameFrame>  m_frames;
    int                     m_frameSkip = 0;

public:

    GameHistory()
    {
    }

    void setFrameSkip(int frames)
    {
        m_frameSkip = frames;
    }

    void onFrame()
    {
        if (m_frames.empty() || (BWAPI::Broodwar->getFrameCount() - m_frames.back().getFrame() >= m_frameSkip))
        {
            m_frames.push_back(GameFrame());
        }
    }

    void writeToFile(const std::string & filename) const
    {
        std::ofstream fout(filename);

        for (const auto & frame : m_frames)
        {
            frame.writeToFile(fout);
        }

        fout.close();
    }
};
