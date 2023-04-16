//////////////////////////////////////////////////////////////////////////
//
// This file is part of the BWEM Library.
// BWEM is free software, licensed under the MIT/X11 License. 
// A copy of the license is provided with the library in the LICENSE file.
// Copyright (c) 2015, 2017, Igor Dimitrijevic
//
//////////////////////////////////////////////////////////////////////////


#ifndef BWEM_EXAMPLES_H
#define BWEM_EXAMPLES_H

#include <BWAPI.h>
#include "exampleWall.h"
#include "defs.h"

namespace BWEM
{

class Map;

namespace utils
{

void drawMap(const Map & theMap);

#if BWEM_USE_MAP_PRINTER

// Prints information about theMap into the file specified in MapPrinter::m_fileName
// The printed informations are highly customizable (Cf. mapPrinter.cpp).
void printMap(const Map & theMap);


// Prints information about theMap onto the game screen.
// The printed informations are highly customizable (Cf. mapDrawer.cpp).
void pathExample(const Map & theMap);

#endif


// Demonstrates the use of utils::GridMap.
void gridMapExample(const Map & theMap);



///////////////    See alse the examples in exampleWall.h    ///////////////


}} // namespace BWEM::utils


#endif

