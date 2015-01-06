#pragma once

#include <ctime>
#include <limits>

namespace SparCraft
{
	class RandomInt;
}
	
class SparCraft::RandomInt
{
	int _seed;
    int _min;
    int _max;

public:

	RandomInt(int min, int max, int seed)
        : _seed(seed)
        , _min(min)
        , _max(max)
	{
		srand(seed);
	}

	int nextInt()
	{
		return ( rand() % (_max-_min) ) + _min;
	}
};
