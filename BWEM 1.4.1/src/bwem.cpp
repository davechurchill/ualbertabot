//////////////////////////////////////////////////////////////////////////
//
// This file is part of the BWEM Library.
// BWEM is free software, licensed under the MIT/X11 License. 
// A copy of the license is provided with the library in the LICENSE file.
// Copyright (c) 2015, 2017, Igor Dimitrijevic
//
//////////////////////////////////////////////////////////////////////////


#include "bwem.h"


using namespace BWAPI;
using namespace BWAPI::UnitTypes::Enum;
namespace { auto & bw = Broodwar; }

using namespace std;


namespace BWEM {

using namespace utils;

namespace detail {

void onAssertThrowFailed(const std::string & file, int line, const std::string & condition, const std::string & message)
{
	unused(file);
	unused(line);
	unused(condition);
	unused(message);

	assert(false);
	throw Exception(file + ", line " + std::to_string(line) + " - " + message);
}

} // namespace details



} // namespace BWEM


