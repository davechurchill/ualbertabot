//////////////////////////////////////////////////////////////////////////////
// Timer.h
// =======
// High Resolution Timer.
// This timer is able to measure the elapsed time with 1 micro-second accuracy
// in both Windows, Linux and Unix system 
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2003-01-13
// UPDATED: 2006-01-13
//
// Copyright (c) 2003 Song Ho Ahn
//////////////////////////////////////////////////////////////////////////////
// Modified by: David Churchill (dave.churchill@gmail.com)
// For academic use (2011)
//////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef WIN32   // Windows system specific
	#include <windows.h>
#else          // Unix based system specific
	#include <sys/time.h>
#endif

#include "Common.h"

namespace SparCraft
{
class Timer
{
 	double startTimeInMicroSec;                 // starting time in micro-second
    double endTimeInMicroSec;                   // ending time in micro-second
    int    stopped;                             // stop flag 
	#ifdef WIN32
		LARGE_INTEGER frequency;                    // ticks per second
		LARGE_INTEGER startCount;                   //
		LARGE_INTEGER endCount;                     //
	#else
		timeval startCount;                         //
		timeval endCount;                           //
	#endif

public:

	Timer();
	
    ~Timer();

    void start();
	
    void stop();
	
    double getElapsedTimeInMicroSec();
	double getElapsedTimeInMilliSec();
	double getElapsedTimeInSec();
	double getElapsedTime();
};
}