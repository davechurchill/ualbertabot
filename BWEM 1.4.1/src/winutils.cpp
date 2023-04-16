//////////////////////////////////////////////////////////////////////////
//
// This file is part of the BWEM Library.
// BWEM is free software, licensed under the MIT/X11 License. 
// A copy of the license is provided with the library in the LICENSE file.
// Copyright (c) 2015, 2017, Igor Dimitrijevic
//
//////////////////////////////////////////////////////////////////////////


#include "winutils.h"
#if BWEM_USE_WINUTILS
#include <windows.h>
#include <stdexcept>


using namespace std;

namespace BWEM {
namespace utils {


//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                  class Timer
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

double Timer::m_freq = 0.0;


Timer::Timer()
{
	if (m_freq == 0.0)		// m_freq not initialized yet ?
	{
		LARGE_INTEGER li;
		auto res = QueryPerformanceFrequency(&li);
	    bwem_assert(res);
		unused(res);
		m_freq = li.QuadPart / 1000.0;
	}

	Reset();
}


int64_t Timer::Now() const
{
	LARGE_INTEGER li;
	auto res = QueryPerformanceCounter(&li);
	bwem_assert(res);
	unused(res);
	return li.QuadPart;
}



}} // namespace BWEM::utils

#endif // BWEM_USE_WINUTILS

