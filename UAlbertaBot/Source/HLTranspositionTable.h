#pragma once
#include "HLState.h"

namespace UAlbertaBot
{
	struct HLEntry{
		HLMove _bestMove;
		unsigned int _hash;
		int _value;
		int _height;
		bool _exact;
		bool _upper;
	};
	class HLTranspositionTable
	{
		int _size;
		std::unique_ptr<HLEntry[]> _entries;
	public:
		HLTranspositionTable(int size);
		~HLTranspositionTable();
		void store(const HLState &origState, const HLMove &bestMove, int value, int alpha, int beta, int height);
		void store(const HLState &origState, int depth, const HLMove &move, const HLMove &bestMove, int value, int alpha, int beta, int height);
		const HLEntry& lookup(const HLState &state, int depth, const HLMove &move);
		const HLEntry& lookup(const HLState &state);
	};

	struct HLCacheEntry{
		HLState _state;
		unsigned int _hash;
	};
	class HLCacheTable
	{
		int _size;
		std::unique_ptr<HLCacheEntry[]> _entries;
	public:
		HLCacheTable(int size);
		~HLCacheTable();
		void store(const HLState &origState, int depth, const std::array < HLMove, 2 > &movePair, const HLState &newState);
		const HLCacheEntry& lookup(const HLState &state, int depth, const std::array < HLMove, 2 > &movePair);
	};

}