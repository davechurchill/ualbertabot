//////////////////////////////////////////////////////////////////////////
//
// This file is part of the BWEM Library.
// BWEM is free software, licensed under the MIT/X11 License. 
// A copy of the license is provided with the library in the LICENSE file.
// Copyright (c) 2015, 2017, Igor Dimitrijevic
//
//////////////////////////////////////////////////////////////////////////


#include "mapPrinter.h"

#if BWEM_USE_MAP_PRINTER

#include "map.h"
#include "../EasyBMP_1.06/EasyBMP.h"


using namespace BWAPI;
using namespace BWAPI::UnitTypes::Enum;
namespace { auto & bw = Broodwar; }

using namespace std;


namespace BWEM {

using namespace BWAPI_ext;

namespace utils {


//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                  class MapPrinter
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

const string MapPrinter::m_fileName = "bwapi-data/map.bmp";

std::unique_ptr<BMP> MapPrinter::m_pBMP;
MapPrinter MapPrinter::m_Instance;
const Map * MapPrinter::m_pMap = nullptr;

const bool MapPrinter::showAltitude				=						true;
const bool MapPrinter::showAreas				=						true;
const bool MapPrinter::showContinents			= !showAreas &&			true;
const bool MapPrinter::showLakes				=						true;
const bool MapPrinter::showSeaSide				=						true;
const bool MapPrinter::showUnbuildable			=						false;
const bool MapPrinter::showGroundHeight			=						true;
const bool MapPrinter::showChokePoints			=						true;
const bool MapPrinter::showResources			=						true;
const bool MapPrinter::showMinerals				= showResources &&		true;
const bool MapPrinter::showGeysers				= showResources &&		true;
const bool MapPrinter::showStaticBuildings		=						true;
const bool MapPrinter::showBlockingBuildings	=						true;
const bool MapPrinter::showStackedNeutrals		=						true;
const bool MapPrinter::showStartingLocations	=						true;
const bool MapPrinter::showBases				=						true;
const bool MapPrinter::showAssignedRessources	= showBases &&			true;
const bool MapPrinter::showData					=						false;

const MapPrinter::Color MapPrinter::Color::higherGround			= Color(0, 0, 0);
const MapPrinter::Color MapPrinter::Color::unbuildable			= Color(0, 0, 0);
const MapPrinter::Color MapPrinter::Color::terrain				= Color(164, 164, 164);
const MapPrinter::Color MapPrinter::Color::sea					= Color(0, 0, 192);
const MapPrinter::Color MapPrinter::Color::seaSide				= Color(0, 0, 100);
const MapPrinter::Color MapPrinter::Color::lake					= Color(0, 100, 200);
const MapPrinter::Color MapPrinter::Color::tinyArea				= Color(50, 50, 255);
const MapPrinter::Color MapPrinter::Color::chokePoints			= (showAreas||showContinents) ? Color(255, 255, 255) : Color(0, 0, 0);
const MapPrinter::Color MapPrinter::Color::minerals				= Color(0, 255, 255);
const MapPrinter::Color MapPrinter::Color::geysers				= Color(0, 255, 0);
const MapPrinter::Color MapPrinter::Color::staticBuildings		= Color(255, 0, 255);
const MapPrinter::Color MapPrinter::Color::blockingNeutrals		= Color(0, 0, 0);
const MapPrinter::Color MapPrinter::Color::startingLocations	= Color(255, 255, 0);
const MapPrinter::Color MapPrinter::Color::bases				= Color(0, 0, 255);


void MapPrinter::Initialize(const Map * pMap)
{
	bwem_assert_throw(pMap->Initialized());
	bwem_assert_throw_plus(canWrite(m_fileName), "MapPrinter could not create the file " + m_fileName);

	m_pMap = pMap;
	m_pBMP = make_unique<BMP>();
	m_pBMP->SetSize(bw->mapWidth()*4, bw->mapHeight()*4);
	m_pBMP->SetBitDepth(24);
}


MapPrinter::~MapPrinter()
{
	if (canWrite(m_fileName)) m_pBMP->WriteToFile(m_fileName.c_str());

//	Uncomment the 2 lines below to write one more copy of the bitmap, in the folder of the map used, with the same name.
//	string twinFileName = bw->mapPathName().substr(0, bw->mapPathName().size()-3) + "bmp";
//	if (canWrite(twinFileName)) m_pBMP->WriteToFile(twinFileName.c_str());
}


void MapPrinter::Point(int x, int y, Color col)
{
	bwem_assert((0 <= x) && (x < m_pBMP->TellWidth()));
	bwem_assert((0 <= y) && (y < m_pBMP->TellHeight()));

	RGBApixel rgba;
	rgba.Red = col.red;
	rgba.Green = col.green;
	rgba.Blue = col.blue;

	m_pBMP->SetPixel(x, y, rgba);
}


void MapPrinter::Line(WalkPosition A, WalkPosition B, Color col, dashed_t dashedMode)
{
	int N = max(1, roundedDist(A, B));
	if ((dashedMode == dashed) && (N >= 4)) N /= 2;

	for (int i = 0 ; i <= N ; ++i)
		MapPrinter::Get().Point((A*i + B*(N-i))/N, col);
}


void MapPrinter::Rectangle(WalkPosition TopLeft, WalkPosition BottomRight, Color col, fill_t fillMode, dashed_t dashedMode)
{
	for (int y = TopLeft.y ; y <= BottomRight.y ; ++y)
	for (int x = TopLeft.x ; x <= BottomRight.x ; ++x)
		if ((fillMode == fill) || (y == TopLeft.y) || (y == BottomRight.y) || (x == TopLeft.x) || (x == BottomRight.x))
			if ((dashedMode == not_dashed) || ((x + y) & 1))
				MapPrinter::Get().Point(x, y, col);
}


void MapPrinter::Square(WalkPosition Center, int radius, Color col, fill_t fillMode)
{
	for (int y = Center.y-radius ; y <= Center.y+radius ; ++y)
	for (int x = Center.x-radius ; x <= Center.x+radius ; ++x)
		if ((fillMode == fill) || (y == Center.y-radius) || (y == Center.y+radius) || (x == Center.x-radius) || (x == Center.x+radius))
			if (m_pMap->Valid(WalkPosition(x, y)))
				MapPrinter::Get().Point(x, y, col);
}


void MapPrinter::Circle(WalkPosition Center, int radius, Color col, fill_t fillMode)
{
	for (int y = Center.y-radius ; y <= Center.y+radius ; ++y)
	for (int x = Center.x-radius ; x <= Center.x+radius ; ++x)
	{
		WalkPosition w(x, y);
		if (dist(w, Center) <= radius)
		if ((fillMode == fill) || (dist(w, Center) >= radius-1))
			if (m_pMap->Valid(w))
				MapPrinter::Get().Point(x, y, col);
	}
}

	
}} // namespace BWEM::utils

#endif // BWEM_USE_MAP_PRINTER

