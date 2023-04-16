//////////////////////////////////////////////////////////////////////////
//
// This file is part of the BWEM Library.
// BWEM is free software, licensed under the MIT/X11 License. 
// A copy of the license is provided with the library in the LICENSE file.
// Copyright (c) 2015, 2017, Igor Dimitrijevic
//
//////////////////////////////////////////////////////////////////////////


#include "examples.h"
#include "map.h"
#include "base.h"
#include "neutral.h"
#include "mapPrinter.h"
#include "mapDrawer.h"
#include "gridMap.h"
#include "bwapiExt.h"

using namespace BWAPI;
using namespace BWAPI::UnitTypes::Enum;
namespace { auto & bw = Broodwar; }

using namespace std;


namespace BWEM {

using namespace utils;
using namespace BWAPI_ext;

namespace utils
{




void drawMap(const Map & theMap)
{
	if (MapDrawer::showFrontier)
		for (auto f : theMap.RawFrontier())
			bw->drawBoxMap(Position(f.second), Position(f.second + 1), MapDrawer::Color::frontier, bool("isSolid"));

	for (int y = 0 ; y < theMap.Size().y ; ++y)
	for (int x = 0 ; x < theMap.Size().x ; ++x)
	{
		TilePosition t(x, y);
		const Tile & tile = theMap.GetTile(t, check_t::no_check);
		
		if (MapDrawer::showUnbuildable && !tile.Buildable())
			drawDiagonalCrossMap(Position(t), Position(t + 1), MapDrawer::Color::unbuildable);
		
		if (MapDrawer::showGroundHeight && (tile.GroundHeight() > 0))
		{
			auto col = tile.GroundHeight() == 1 ? MapDrawer::Color::highGround : MapDrawer::Color::veryHighGround;
			bw->drawBoxMap(Position(t), Position(t)+6, col, bool("isSolid"));
			if (tile.Doodad()) bw->drawTriangleMap(center(t)+Position(0, 5), center(t)+Position(-3, 2), center(t)+Position(+3, 2), Colors::White);
		}
	}

	for (int y = 0 ; y < theMap.WalkSize().y ; ++y)
	for (int x = 0 ; x < theMap.WalkSize().x ; ++x)
	{
		WalkPosition w(x, y);
		const MiniTile & miniTile = theMap.GetMiniTile(w, check_t::no_check);
			
		if (MapDrawer::showSeas && miniTile.Sea())
			drawDiagonalCrossMap(Position(w), Position(w + 1), MapDrawer::Color::sea);
		
		if (MapDrawer::showLakes && miniTile.Lake())
			drawDiagonalCrossMap(Position(w), Position(w + 1), MapDrawer::Color::lakes);
	}

	if (MapDrawer::showCP)
		for (const Area & area : theMap.Areas())
			for (const ChokePoint * cp : area.ChokePoints())
				for (ChokePoint::node end : {ChokePoint::end1, ChokePoint::end2})
					bw->drawLineMap(Position(cp->Pos(ChokePoint::middle)), Position(cp->Pos(end)), MapDrawer::Color::cp);

	if (MapDrawer::showMinerals)
		for (auto & m : theMap.Minerals())
		{
			bw->drawBoxMap(Position(m->TopLeft()), Position(m->TopLeft() + m->Size()) , MapDrawer::Color::minerals);
			if (m->Blocking())
				drawDiagonalCrossMap(Position(m->TopLeft()), Position(m->TopLeft() + m->Size()), MapDrawer::Color::minerals);
		}

	if (MapDrawer::showGeysers)
		for (auto & g : theMap.Geysers())
			bw->drawBoxMap(Position(g->TopLeft()), Position(g->TopLeft() + g->Size()) , MapDrawer::Color::geysers);

	if (MapDrawer::showStaticBuildings)
		for (auto & s : theMap.StaticBuildings())
		{
			bw->drawBoxMap(Position(s->TopLeft()), Position(s->TopLeft() + s->Size()) , MapDrawer::Color::staticBuildings);
			if (s->Blocking())
				drawDiagonalCrossMap(Position(s->TopLeft()), Position(s->TopLeft() + s->Size()), MapDrawer::Color::staticBuildings);
		}

	for (const Area & area : theMap.Areas())
		for (const Base & base : area.Bases())
		{
			if (MapDrawer::showBases)
				bw->drawBoxMap(Position(base.Location()), Position(base.Location() + UnitType(Terran_Command_Center).tileSize()) , MapDrawer::Color::bases);

			if (MapDrawer::showAssignedRessources)
			{
				vector<Ressource *> AssignedRessources(base.Minerals().begin(), base.Minerals().end());
				AssignedRessources.insert(AssignedRessources.end(), base.Geysers().begin(), base.Geysers().end());

				for (const Ressource * r : AssignedRessources)
					bw->drawLineMap(base.Center(), r->Pos(), MapDrawer::Color::assignedRessources);
			}
		}
}

#if BWEM_USE_MAP_PRINTER

static void printNeutral(const Map & theMap, const Neutral * n, MapPrinter::Color col)
{
	const WalkPosition delta(n->Pos().x < theMap.Center().x ? +1 : -1, n->Pos().y < theMap.Center().y ? +1 : -1);
	const int stackSize = MapPrinter::showStackedNeutrals ? theMap.GetTile(n->TopLeft()).StackedNeutrals() : 1;

	for (int i = 0 ; i < stackSize ; ++i)
	{
		auto origin = WalkPosition(n->TopLeft()) + delta * i;
		auto size = WalkPosition (n->Size());
		if (!theMap.Valid(origin) || !theMap.Valid(origin + size - 1)) break;

		MapPrinter::Get().Rectangle(origin, origin + size - 1, col, MapPrinter::fill);

		if (MapPrinter::showBlockingBuildings && n->Blocking())
			if (i < stackSize-1)
			{
				MapPrinter::Get().Point(origin, MapPrinter::Color::blockingNeutrals);
				MapPrinter::Get().Point(origin + size - 1, MapPrinter::Color::blockingNeutrals);
				MapPrinter::Get().Point(WalkPosition(origin.x, (origin + size - 1).y), MapPrinter::Color::blockingNeutrals);
				MapPrinter::Get().Point(WalkPosition((origin + size - 1).x, origin.y), MapPrinter::Color::blockingNeutrals);
			}
			else
				MapPrinter::Get().Rectangle(origin, origin + size - 1, MapPrinter::Color::blockingNeutrals);
	}
}


static MapPrinter::Color getZoneColor(const Area * area, map<int, MapPrinter::Color> & map_Zone_Color)
{
	const int zoneId = MapPrinter::showAreas ? area->Id() : area->GroupId();
	auto it = map_Zone_Color.emplace(zoneId, MapPrinter::Color());
	MapPrinter::Color & color = it.first->second;
	if (it.second) // zoneId was not find --> insertion did occur --> we have do define the new color:
	{
		int tries = 0;
		do
		{
			color.red = rand() % 256;
			color.green = rand() % 256;
			color.blue = 0;					// blue unused for Terrain so that Water can be easily distinguished.
			if (++tries > 100) break;
		} while (
				// 1) color should not be too dark
				(color.red + color.green < 150) ||

				// 2) color should differ enough from the colors of the neighbouring Areas
				 (	MapPrinter::showAreas &&
					any_of(area->ChokePointsByArea().begin(), area->ChokePointsByArea().end(),
						[&map_Zone_Color, &color](const pair<const Area *, const std::vector<ChokePoint> *> & neighbour)
						{
							auto it2 = map_Zone_Color.find(neighbour.first->Id());
							if (it2 == map_Zone_Color.end()) return false;
							MapPrinter::Color neighbouringColor = it2->second;
							return abs(color.red   - (int)neighbouringColor.red) +
								   abs(color.green - (int)neighbouringColor.green) < 150;
						})
				));
	}

	return color;
}


void printMap(const Map & theMap)
{
	map<int, MapPrinter::Color> map_Zone_Color;		// a "Zone" is either an Area or a continent

	for (int y = 0 ; y < theMap.WalkSize().y ; ++y)
	for (int x = 0 ; x < theMap.WalkSize().x ; ++x)
	{
		WalkPosition p(x, y);
		const auto & miniTile = theMap.GetMiniTile(p, check_t::no_check);

		MapPrinter::Color col;
		if (miniTile.Sea())
		{
			if (MapPrinter::showSeaSide && seaSide(p, &theMap))	col = MapPrinter::Color::seaSide;
			else												col = MapPrinter::Color::sea;
		}
		else
		{
			if (MapPrinter::showLakes && miniTile.Lake())
				col = MapPrinter::Color::lake;
			else
			{
				if (MapPrinter::showAltitude)
				{
					uint8_t c = uint8_t(255 - miniTile.Altitude() * 255 / theMap.MaxAltitude());
					col = MapPrinter::Color{c, c, c};
				}
				else col = MapPrinter::Color::terrain;

				if (MapPrinter::showAreas || MapPrinter::showContinents)
					if (miniTile.AreaId() > 0)
					{
						const Area * area = theMap.GetArea(miniTile.AreaId());
						MapPrinter::Color zoneColor = getZoneColor(area, map_Zone_Color);
						col.red = zoneColor.red * col.red / 255;
						col.green = zoneColor.green * col.green / 255;
						col.blue = 0;
					}
					else col = MapPrinter::Color::tinyArea;
			}
		}

		MapPrinter::Get().Point(p, col);
	}

	if (MapPrinter::showData)
		for (int y = 0 ; y < theMap.Size().y ; ++y)
		for (int x = 0 ; x < theMap.Size().x ; ++x)
		{
			int data = theMap.GetTile(TilePosition(x, y)).Data();
			uint8_t c = uint8_t(((data/1)*1) % 256);
			MapPrinter::Color col(c, c, c);
			WalkPosition origin(TilePosition(x, y));
			MapPrinter::Get().Rectangle(origin, origin + 3, col, MapPrinter::fill);
		}

	if (MapPrinter::showUnbuildable)
		for (int y = 0 ; y < theMap.Size().y ; ++y)
		for (int x = 0 ; x < theMap.Size().x ; ++x)
			if (!theMap.GetTile(TilePosition(x, y)).Buildable())
			{
				WalkPosition origin(TilePosition(x, y));
				MapPrinter::Get().Rectangle(origin+1, origin + 2, MapPrinter::Color::unbuildable);
			}

	if (MapPrinter::showGroundHeight)
		for (int y = 0 ; y < theMap.Size().y ; ++y)
		for (int x = 0 ; x < theMap.Size().x ; ++x)
		{
			int groundHeight = theMap.GetTile(TilePosition(x, y)).GroundHeight();
			if (groundHeight >= 1)
				for (int dy = 0 ; dy < 4 ; ++dy)
				for (int dx = 0 ; dx < 4 ; ++dx)
				{
					WalkPosition p = WalkPosition(TilePosition(x, y)) + WalkPosition(dx, dy);
					if (theMap.GetMiniTile(p, check_t::no_check).Walkable())		// groundHeight is usefull only for walkable miniTiles
						if ((dx + dy) & (groundHeight == 1 ? 1 : 3))
							MapPrinter::Get().Point(p, MapPrinter::Color::higherGround);
				}

//			if (theMap.GetTile(TilePosition(x, y)).Doodad())
//				MapPrinter::Get().Circle(WalkPosition(TilePosition(x, y)) + 2, 4, MapPrinter::Color(255, 255, 255));
		}

	if (MapPrinter::showAssignedRessources)
		for (const Area & area : theMap.Areas())
			for (const Base & base : area.Bases())
			{
				for (const Mineral * m : base.Minerals())
					MapPrinter::Get().Line(WalkPosition(base.Center()), WalkPosition(m->Pos()), MapPrinter::Color::bases);
				for (const Geyser * g : base.Geysers())
					MapPrinter::Get().Line(WalkPosition(base.Center()), WalkPosition(g->Pos()), MapPrinter::Color::bases);
			}

	if (MapPrinter::showGeysers)
		for (auto & g : theMap.Geysers())
			printNeutral(theMap, g.get(), MapPrinter::Color::geysers);

	if (MapPrinter::showMinerals)
		for (auto & m : theMap.Minerals())
			printNeutral(theMap, m.get(), MapPrinter::Color::minerals);

	if (MapPrinter::showStaticBuildings)
		for (auto & s : theMap.StaticBuildings())
			printNeutral(theMap, s.get(), MapPrinter::Color::staticBuildings);

	if (MapPrinter::showStartingLocations)
		for (TilePosition t : theMap.StartingLocations())
		{
			WalkPosition origin(t);
			WalkPosition size(UnitType(Terran_Command_Center).tileSize());	// same size for other races
			MapPrinter::Get().Rectangle(origin, origin + size - 1, MapPrinter::Color::startingLocations, MapPrinter::fill);
		}

	if (MapPrinter::showBases)
		for (const Area & area : theMap.Areas())
		{
			for (const Base & base : area.Bases())
			{
				WalkPosition origin(base.Location());
				WalkPosition size(UnitType(Terran_Command_Center).tileSize());	// same size for other races
				auto dashMode = base.BlockingMinerals().empty() ? MapPrinter::not_dashed : MapPrinter::dashed;
				MapPrinter::Get().Rectangle(origin, origin + size - 1, MapPrinter::Color::bases, MapPrinter::do_not_fill, dashMode);
			}

//			if (area.LowGroundPercentage() > 66)		MapPrinter::Get().Circle(area.Top(), 15, MapPrinter::Color(0, 0, 0), MapPrinter::fill);
//			if (area.HighGroundPercentage() > 66)		MapPrinter::Get().Circle(area.Top(), 15, MapPrinter::Color(128, 128, 128), MapPrinter::fill);
//			if (area.VeryHighGroundPercentage() > 66)	MapPrinter::Get().Circle(area.Top(), 15, MapPrinter::Color(255, 255, 255), MapPrinter::fill);
		}

	if (MapPrinter::showChokePoints)
	{
		for (auto f : theMap.RawFrontier())
			MapPrinter::Get().Point(f.second, MapPrinter::Color::chokePoints);

		for (const Area & area : theMap.Areas())
			for (const ChokePoint * cp : area.ChokePoints())
			{
				for (ChokePoint::node n : {ChokePoint::end1, ChokePoint::end2})
					MapPrinter::Get().Square(cp->Pos(n), 1, MapPrinter::Color(255, 0, 255), MapPrinter::fill);
				MapPrinter::Get().Square(cp->Center(), 1, MapPrinter::Color(0, 0, 255), MapPrinter::fill);
			}
	}
/*
	for (int y = 0 ; y < theMap.Size().y ; ++y)
	for (int x = 0 ; x < theMap.Size().x ; ++x)
		if (Area::id id = theMap.GetTile(TilePosition(x, y)).AreaId())
		{
			WalkPosition origin(TilePosition(x, y));
			MapPrinter::Color col = (id > 0) ? MapPrinter::Color(255, 255, 255) :
									(id == -1) ? MapPrinter::Color(128, 128, 128) :MapPrinter::Color(0, 0, 0);
			MapPrinter::Get().Rectangle(origin, origin + 3, col);
		}
*/
}



void pathExample(const Map & theMap)
{
	if (theMap.StartingLocations().size() < 2) return;

	const MapPrinter::Color col(255, 255, 255);

	WalkPosition a = WalkPosition(random_element(theMap.StartingLocations()));
	
	WalkPosition b = a;
	while (b == a) b = WalkPosition(random_element(theMap.StartingLocations()));

//	Uncomment this to use random positions for a and b:
//	a = WalkPosition(theMap.RandomPosition());
//	b = WalkPosition(theMap.RandomPosition());

	MapPrinter::Get().Circle(a, 6, col, MapPrinter::fill);
	MapPrinter::Get().Circle(b, 6, col, MapPrinter::fill);

	int length;
	const CPPath & Path = theMap.GetPath(Position(a), Position(b), &length);

	if (length < 0) return;		// cannot reach b from a

	if (Path.empty())						// no ChokePoint between a and b: 
	{
		// let's verify that a and b are in the same Area:
		bwem_assert(theMap.GetNearestArea(a) == theMap.GetNearestArea(b));

		// just draw a single line between them:
		MapPrinter::Get().Line(a, b, col, MapPrinter::dashed);
	}
	else									// at least one ChokePoint between a and b: 
	{
		// let's verify that a and b are not in the same Area:
		bwem_assert(theMap.GetNearestArea(a) != theMap.GetNearestArea(b));

		// draw a line between each ChokePoint in Path:
		const ChokePoint * cpPrevious = nullptr;
		for (const ChokePoint * cp : Path)
		{
			if (cpPrevious)	MapPrinter::Get().Line(cpPrevious->Center(), cp->Center(), col, MapPrinter::dashed);
			MapPrinter::Get().Circle(cp->Center(), 6, col);
			cpPrevious = cp;
		}

		MapPrinter::Get().Line(a, Path.front()->Center(), col, MapPrinter::dashed);
		MapPrinter::Get().Line(b, Path.back()->Center(), col, MapPrinter::dashed);
	}
}
#endif // BWEM_USE_MAP_PRINTER





struct SimpleGridMapCell
{
	vector<Unit>		Units;
};



class SimpleGridMap : public GridMap<SimpleGridMapCell, 8>
{
public:
						SimpleGridMap(const Map * pMap) : GridMap(pMap) {}

	void				Add(Unit unit);
	void				Remove(Unit unit);

	vector<Unit>		GetUnits(TilePosition topLeft, TilePosition bottomRight, Player player) const;
};


void SimpleGridMap::Add(Unit unit)
{
	auto & List = GetCell(TilePosition(unit->getPosition())).Units;

	if (!contains(List, unit)) List.push_back(unit);
}


void SimpleGridMap::Remove(Unit unit)
{
	auto & List = GetCell(TilePosition(unit->getPosition())).Units;

	really_remove(List, unit);
}


vector<Unit> SimpleGridMap::GetUnits(TilePosition topLeft, TilePosition bottomRight, Player player) const
{
	vector<Unit> Res;

	int i1, j1, i2, j2;
	tie(i1, j1) = GetCellCoords(topLeft);
	tie(i2, j2) = GetCellCoords(bottomRight);

	for (int j = j1 ; j <= j2 ; ++j)
	for (int i = i1 ; i <= i2 ; ++i)
		for (Unit unit : GetCell(i, j).Units)
			if (unit->getPlayer() == player)
				if (BWAPI_ext::inBoundingBox(TilePosition(unit->getPosition()), topLeft, bottomRight))
					Res.push_back(unit);

	return Res;
}


void gridMapExample(const Map & theMap)
{

	// 1) Initialization
	SimpleGridMap Grid(&theMap);

	//  Note: generally, you will create one instance of GridMap, after calling Map::Instance().Initialize().
	
	
	// 2) Update (in AIModule::onFrame)
	for (int j = 0 ; j < Grid.Height() ; ++j)
	for (int i = 0 ; i < Grid.Width() ; ++i)
		Grid.GetCell(i, j).Units.clear();

	for (Unit unit : Broodwar->getAllUnits())
		Grid.Add(unit);

	//  Note: alternatively, you could use the Remove and Add methods only, in the relevant BWAPI::AIModule methods.


	// 3) Use
	TilePosition centerTile(theMap.Center());
	for (Unit unit : Grid.GetUnits(centerTile-10, centerTile+10, Broodwar->self()))
		Broodwar << "My " << unit->getType().getName() << " #" << unit->getID() << " is near the center of the map." << endl;
}



}} // namespace BWEM::utils



