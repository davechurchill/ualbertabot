#pragma once

#include "BWAPI.h"
#include <fstream>
#include <iostream>
#include <vector>

class UnitFrameData
{
    BWAPI::UnitType _type;
    BWAPI::Position _pos;
    BWAPI::Order    _order;
    BWAPI::Position _orderPos;

    int             _id;
    int             _player;
    int             _hp;
    int             _shields;
    int             _energy;
    double          _angle;

public:

    UnitFrameData(BWAPI::Unit unit)
        : _type     (unit->getType())
        , _pos      (unit->getPosition())
        , _order    (unit->getOrder())
        , _orderPos (unit->getOrderTargetPosition())
        , _id       (unit->getID())
        , _player   (unit->getPlayer()->getID())
        , _hp       (unit->getHitPoints())
        , _shields  (unit->getShields())
        , _energy   (unit->getEnergy())
        , _angle    (unit->getAngle())
    {
        
    }
    
    void writeToFile(std::ofstream & fout) const
    {
        fout << _player << " ";         // 1 byte
        fout << _type.getID() << " ";   // 1 byte
        fout << _id << " ";             // 2 byte
        fout << _hp << " ";             // 2 byte
        fout << _shields << " ";        // 2 byte
        fout << _pos.x << " ";          // 2 byte
        fout << _pos.y << " ";          // 2 byte
        //fout << _orderPos.x << " ";     // 2 byte
        //fout << _orderPos.y;            // 2 byte
    }
};

class GameFrame
{
    int                         _frame;
	std::vector<UnitFrameData>  _units;

public:

	GameFrame()
        : _frame(BWAPI::Broodwar->getFrameCount())
    {
        for (const BWAPI::Unit & unit : BWAPI::Broodwar->getAllUnits())
        {
            if (unit->getPlayer()->getID() != -1)
            {
                _units.push_back(UnitFrameData(unit));
            }
        }
    }

    int getFrame() const
    {
        return _frame;
    }

    void writeToFile(std::ofstream & fout) const
    {
        fout << "f " << _frame << "\n";
        for (const auto & unit : _units)
        {
            unit.writeToFile(fout);
            fout << "\n";
        }
    }
};

class GameMap
{
    int                 _width;
    int                 _height;
    std::vector<int>    _walkable;
    
public:

    GameMap()
        : _width(BWAPI::Broodwar->mapWidth() * 4)
        , _height(BWAPI::Broodwar->mapHeight() * 4)
        , _walkable(BWAPI::Broodwar->mapHeight() * BWAPI::Broodwar->mapWidth() * 16, 0)
    {
        for (int h(0); h < _height; ++h)
        {
            for (int w(0); w < _width; ++w)
            {
                _walkable[h*w + w] = BWAPI::Broodwar->isWalkable(h, w);
            }        
        }
    }

    void writeToFile(std::ofstream & fout) const
    {
        
    }
};

class GameHistory
{
    GameMap                 _map;
    std::vector<GameFrame>  _frames;
    int                     _frameSkip;

public:

	GameHistory()
        : _frameSkip(0)
    {
    }

    void setFrameSkip(int frames)
    {
        _frameSkip = frames;
    }

    void onFrame()
    {
        if (_frames.empty() || (BWAPI::Broodwar->getFrameCount() - _frames.back().getFrame() >= _frameSkip))
        {
            _frames.push_back(GameFrame());    
        }
    }

    void writeToFile(const std::string & filename) const
    {
        std::ofstream fout(filename);

        for (const auto & frame : _frames)
        {
            frame.writeToFile(fout);    
        }

        fout.close();
    }
};
