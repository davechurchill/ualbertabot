//////////////////////////////////////////////////////////////////////////
//
// This file is part of the BWEM Library.
// BWEM is free software, licensed under the MIT/X11 License. 
// A copy of the license is provided with the library in the LICENSE file.
// Copyright (c) 2015, 2017, Igor Dimitrijevic
//
//////////////////////////////////////////////////////////////////////////


#ifndef BWEM_WINUTILS_H
#define BWEM_WINUTILS_H

#include "defs.h"
#if BWEM_USE_WINUTILS

#include "utils.h"

namespace BWEM {
namespace utils {



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                  class Timer
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////
//
// High resolution timer - uses Windows's QueryPerformanceCounter
//

class Timer
{
public:
						Timer();		// calls Reset()

	void				Reset()							{ m_start = Now(); }
	double				ElapsedMilliseconds() const		{ return (Now() - m_start) / m_freq; }

private:
	int64_t				Now() const;

	int64_t				m_start;

	static double		m_freq;		// counts per millisecond
};


}} // namespace BWEM::utils


#endif // BWEM_USE_WINUTILS


#endif

