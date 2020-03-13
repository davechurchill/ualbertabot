#pragma once

#include "Grid2D.hpp"
#include <fstream>
#include <iostream>

namespace TileType
{
    enum 
    {
        Unwalkable  = 'U',
        Walk        = 'W',
        BuildAll    = 'B',
        NoDepot     = 'D',
        Mineral     = 'M',
        Gas         = 'G',
        Neutral     = 'N'
    };
}

struct Tile 
{ 
    int x = 0, y = 0; 
};

class StarDraftMap
{
    Grid2D<char>          m_buildTiles;
    Grid2D<char>          m_walkTiles;
    std::vector<Tile>   m_mineralTiles;
    std::vector<Tile>   m_gasTiles;
    std::vector<Tile>   m_resourceTiles;
    std::vector<Tile>   m_startTiles;

    inline bool isWalkable(char tile) const noexcept
    {
        return (tile == TileType::Walk) || (tile == TileType::BuildAll) || (tile == TileType::NoDepot);
    }

    inline bool canBuild(char tile) const noexcept
    {
        return tile == (TileType::BuildAll) || (tile == TileType::NoDepot);
    }

    inline bool canBuildDepot(char tile) const noexcept
    {
        return tile == TileType::BuildAll;
    }

public:

    StarDraftMap() {}

    StarDraftMap(const std::string & path)
    {
        load(path);
    }

    StarDraftMap(size_t width, size_t height)
        : m_buildTiles(width, height, 0)
        , m_walkTiles(width*4, height*4, false)
    {
        
    }

    inline bool isValid(int x, int y) const
    {
        return (x >= 0) && (x < (int)m_buildTiles.width()) && (y >= 0) && (y < (int)m_buildTiles.height());
    }

    inline void set(int x, int y, char val)
    {
        m_buildTiles.set(x, y, val);

        if (val == TileType::Mineral) 
        { 
            m_mineralTiles.push_back({x, y}); 
            m_resourceTiles.push_back({x, y}); 
        }
        else if (val == TileType::Gas) 
        { 
            m_gasTiles.push_back({x, y}); 
            m_resourceTiles.push_back({x, y}); 
        }
    }

    inline void setWalk(int x, int y, bool val)
    {
        m_walkTiles.set(x, y, val);
    }
        
    inline void addStartTile(int x, int y)
    {
        m_startTiles.push_back({x, y});
    }

    inline bool isMineral(int x, int y) const
    {
        return get(x, y) == 'M';
    }

    inline bool isGas(int x, int y) const
    {
        return get(x, y) == 'G';
    }

    inline bool isResource(int x, int y) const
    {
        return isMineral(x,y) || isGas(x,y);
    }
    
    inline size_t width() const
    {
        return m_buildTiles.width();
    }

    inline size_t height() const
    {
        return m_buildTiles.height();
    }

    inline char get(int x, int y) const
    {
        return m_buildTiles.get(x, y);
    }

    inline char getWalk(int x, int y) const
    {
        return m_walkTiles.get(x, y);
    }

    inline bool isWalkable(int x, int y) const
    {
        return isWalkable(get(x, y));
    }

    inline bool canBuild(int x, int y) const
    {
        return canBuild(get(x,y));
    }

    inline bool canBuildDepot(int x, int y) const
    {
        return canBuildDepot(get(x,y));
    }

    inline const std::vector<Tile> & startTiles() const
    {
        return m_startTiles;
    }

    inline const std::vector<Tile> & mineralTiles() const
    {
        return m_mineralTiles;
    }

    inline const std::vector<Tile> & gasTiles() const
    {
        return m_gasTiles;
    }

    inline const std::vector<Tile> & resourceTiles() const
    {
        return m_resourceTiles;
    }

    inline void load(const std::string & path)
    {
        std::ifstream fin(path);
        int w, h, n, sx, sy;
        char c;

        // first line is width height
        fin >> w >> h; 

        m_buildTiles = Grid2D<char>(w, h, 0);
        m_walkTiles  = Grid2D<char>(4*w, 4*h, 0);

        // next line is the start tile locations
        fin >> n;
        for (int i=0; i<n; i++)
        {
            fin >> sx >> sy;
            m_startTiles.push_back({sx, sy});
        }

        // followed by the int values in the grid
        for (size_t y=0; y < m_buildTiles.height(); y++)
        {
            for (size_t x=0; x < m_buildTiles.width(); x++)
            {
                fin >> c;
                set(x, y, c);
            }   
        }

        // followed by the walk tiles
        for (size_t y=0; y < m_walkTiles.height(); y++)
        {
            for (size_t x=0; x < m_walkTiles.width(); x++)
            {
                fin >> c;
                m_walkTiles.set(x, y, c);
            }   
        }
    }

    inline void save(const std::string & path) const
    {
        std::ofstream fout(path);

        // first line is width height
        fout << m_buildTiles.width() << " " << m_buildTiles.height() << "\n";
        
        // next line is the start tile locations
        fout << m_startTiles.size();
        for (auto & tile : m_startTiles)
        {
            fout << " " << tile.x << " " << tile.y;
        }   fout << "\n";

        // followed by the int values in the grid
        for (size_t y=0; y<m_buildTiles.height(); y++)
        {
            for (size_t x=0; x < m_buildTiles.width(); x++)
            {
                fout << m_buildTiles.get(x, y);
            }   fout << "\n";
        }

        // set the walk tiles
        for (size_t y=0; y<m_walkTiles.height(); y++)
        {
            for (size_t x=0; x<m_walkTiles.width(); x++)   
            {
                fout << (m_walkTiles.get(x,y) ? '1' : '0');
            }   fout << "\n";
        }
    }
};
