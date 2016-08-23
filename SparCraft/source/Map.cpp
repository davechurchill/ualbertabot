#include "Map.h"

using namespace SparCraft;

Map::Map()
    : _walkTileWidth(0)
    , _walkTileHeight(0)
    , _buildTileWidth(0)
    , _buildTileHeight(0)
{
}

// constructor which sets a completely walkable map
Map::Map(const size_t & bottomRightBuildTileX, const size_t & bottomRightBuildTileY)
    : _walkTileWidth(bottomRightBuildTileX * 4)
    , _walkTileHeight(bottomRightBuildTileY * 4)
    , _buildTileWidth(bottomRightBuildTileX)
    , _buildTileHeight(bottomRightBuildTileY)
{
    resetVectors();
}

Map::Map(BWAPI::GameWrapper & game)
    : _walkTileWidth(game->mapWidth() * 4)
    , _walkTileHeight(game->mapHeight() * 4)
    , _buildTileWidth(game->mapWidth())
    , _buildTileHeight(game->mapHeight())
{
    resetVectors();

    for (size_t x(0); x < _walkTileWidth; ++x)
    {
        for (size_t y(0); y < _walkTileHeight; ++y)
        {
            setMapData(x, y, game->isWalkable(x, y));
        }
    }
}

const Position Map::getWalkPosition(const Position & pixelPosition) const
{
    return Position(pixelPosition.x() / 8, pixelPosition.y() / 8);
}

void Map::resetVectors()
{
    _mapData = bvv(_walkTileWidth, std::vector<bool>(_walkTileHeight, true));
    _unitData = bvv(_buildTileWidth, std::vector<bool>(_buildTileHeight, false));
    _buildingData = bvv(_buildTileWidth, std::vector<bool>(_buildTileHeight, false));
}

const size_t Map::getPixelWidth() const
{
    return getWalkTileWidth() * 8;
}

const size_t Map::getPixelHeight() const
{
    return getWalkTileHeight() * 8;
}

const size_t & Map::getWalkTileWidth() const
{
    return _walkTileWidth;
}

const size_t & Map::getWalkTileHeight() const
{
    return _walkTileHeight;
}

const size_t & Map::getBuildTileWidth() const
{
    return _buildTileWidth;
}

const size_t & Map::getBuildTileHeight() const
{
    return _buildTileHeight;
}

const bool Map::isWalkable(const SparCraft::Position & pixelPosition) const
{
    const Position & wp(getWalkPosition(pixelPosition));

    return	isWalkable(wp.x(), wp.y());
}

const bool Map::isFlyable(const SparCraft::Position & pixelPosition) const
{
    const Position & wp(getWalkPosition(pixelPosition));

    return isFlyable(wp.x(), wp.y());
}

const bool Map::isWalkable(const size_t & walkTileX, const size_t & walkTileY) const
{
    return	walkTileX >= 0 && walkTileX < (PositionType)getWalkTileWidth() &&
        walkTileY >= 0 && walkTileY < (PositionType)getWalkTileHeight() &&
        getMapData(walkTileX, walkTileY);
}

const bool Map::isFlyable(const size_t & walkTileX, const size_t & walkTileY) const
{
    return	walkTileX >= 0 && walkTileX < (PositionType)getWalkTileWidth() &&
        walkTileY >= 0 && walkTileY < (PositionType)getWalkTileHeight();
}

const bool Map::getMapData(const size_t & walkTileX, const size_t & walkTileY) const
{
    return _mapData[walkTileX][walkTileY];
}

const bool Map::getUnitData(const size_t & buildTileX, const size_t & buildTileY) const
{
    return _unitData[buildTileX][buildTileY];
}

void Map::setMapData(const size_t & walkTileX, const size_t & walkTileY, const bool val)
{
    _mapData[walkTileX][walkTileY] = val;
}

void Map::setUnitData(BWAPI::GameWrapper & game)
{
    _unitData = bvv(getBuildTileWidth(), std::vector<bool>(getBuildTileHeight(), true));

    for (BWAPI::UnitInterface * unit : game->getAllUnits())
    {
        if (!unit->getType().isBuilding())
        {
            addUnit(unit);
        }
    }
}

const bool Map::canBuildHere(BWAPI::TilePosition pos) const
{
    return _unitData[pos.x][pos.y] && _buildingData[pos.x][pos.y];
}

void Map::setBuildingData(BWAPI::GameWrapper & game)
{
    _buildingData = bvv(getBuildTileWidth(), std::vector<bool>(getBuildTileHeight(), true));

    for (BWAPI::UnitInterface * unit : game->getAllUnits())
    {
        if (unit->getType().isBuilding())
        {
            addUnit(unit);
        }
    }
}

void Map::addUnit(BWAPI::Unit & unit)
{

    if (unit->getType().isBuilding())
    {
        int tx = unit->getPosition().x / TILE_SIZE;
        int ty = unit->getPosition().y / TILE_SIZE;
        int sx = unit->getType().tileWidth();
        int sy = unit->getType().tileHeight();
        for (int x = tx; x < tx + sx && x < (int)getBuildTileWidth(); ++x)
        {
            for (int y = ty; y < ty + sy && y < (int)getBuildTileHeight(); ++y)
            {
                _buildingData[x][y] = true;
            }
        }
    }
    else
    {
        int startX = (unit->getPosition().x - unit->getType().dimensionLeft()) / TILE_SIZE;
        int endX = (unit->getPosition().x + unit->getType().dimensionRight() + TILE_SIZE - 1) / TILE_SIZE; // Division - round up
        int startY = (unit->getPosition().y - unit->getType().dimensionUp()) / TILE_SIZE;
        int endY = (unit->getPosition().y + unit->getType().dimensionDown() + TILE_SIZE - 1) / TILE_SIZE;
        for (int x = startX; x < endX && x < (int)getBuildTileWidth(); ++x)
        {
            for (int y = startY; y < endY && y < (int)getBuildTileHeight(); ++y)
            {
                _unitData[x][y] = true;
            }
        }
    }
}

unsigned int * Map::getRGBATexture()
{
    unsigned int * data = new unsigned int[getWalkTileWidth() * getWalkTileHeight()];
    for (size_t x(0); x < getWalkTileWidth(); ++x)
    {
        for (size_t y(0); y < getWalkTileHeight(); ++y)
        {
            if (!isWalkable(x, y))
            {
                data[y*getWalkTileWidth() + x] = 0xffffffff;
            }
            else
            {
                data[y*getWalkTileWidth() + x] = 0x00000000;
            }
        }
    }

    return data;
}

void Map::write(const std::string & filename)
{
    std::ofstream fout(filename.c_str());
    fout << getWalkTileWidth() << "\n" << getWalkTileHeight() << "\n";

    for (size_t y(0); y < getWalkTileHeight(); ++y)
    {
        for (size_t x(0); x < getWalkTileWidth(); ++x)
        {
            fout << (isWalkable(x, y) ? 1 : 0);
        }

        fout << "\n";
    }

    fout.close();
}

void Map::load(const std::string & filename)
{
    std::ifstream fin(filename.c_str());
    std::string line;

    getline(fin, line);
    _walkTileWidth = atoi(line.c_str());

    getline(fin, line);
    _walkTileHeight = atoi(line.c_str());

    _buildTileWidth = _walkTileWidth / 4;
    _buildTileHeight = _walkTileHeight / 4;

    resetVectors();

    for (size_t y(0); y < getWalkTileHeight(); ++y)
    {
        getline(fin, line);

        for (size_t x(0); x < getWalkTileWidth(); ++x)
        {
            _mapData[x][y] = line[x] == '1' ? true : false;
        }
    }

    fin.close();
}