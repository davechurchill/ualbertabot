#pragma once

#include <ctime>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <limits>

namespace SparCraft
{
	class RandomInt;
}
	
class SparCraft::RandomInt
{
	boost::random::mt19937 gen;
	boost::random::uniform_int_distribution<> dist;

public:

	RandomInt(int min, int max, int seed)
		: dist(min, max)
	{
		gen.seed(seed);
	}

	int nextInt()
	{
		return dist(gen);
	}

	void seed(int seed)
	{
		gen.seed(seed);
	}

	int reset(int min, int max, int seed)
	{
		dist = boost::random::uniform_int_distribution<>(min, max);
		gen.seed(seed);
	}
};
