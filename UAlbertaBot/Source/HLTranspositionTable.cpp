#include "HLTranspositionTable.h"

using namespace UAlbertaBot;

HLTranspositionTable::HLTranspositionTable(int size) :_size(size), _entries(new HLEntry[size])
{
}


HLTranspositionTable::~HLTranspositionTable()
{
}
void HLTranspositionTable::store(const HLState &origState, const HLMove &bestMove, int value, int alpha, int beta, int height)
{
	unsigned int hash = origState.getHash();

	_entries[hash%_size]._bestMove = bestMove;
	_entries[hash%_size]._hash = hash;
	_entries[hash%_size]._value = value;
	_entries[hash%_size]._height = height;
	if (value <= alpha){
		_entries[hash%_size]._exact = false;
		_entries[hash%_size]._upper = true;
	}
	else if (value >= beta){
		_entries[hash%_size]._exact = false;
		_entries[hash%_size]._upper = false;
	}
	else{
		_entries[hash%_size]._exact = true;
		_entries[hash%_size]._upper = false;
	}
}
void HLTranspositionTable::store(const HLState &origState, int depth, const HLMove &move, const HLMove &bestMove, int value, int alpha, int beta, int height)
{
	unsigned int hash = origState.getHash(depth, move);

	_entries[hash%_size]._bestMove = bestMove;
	_entries[hash%_size]._hash = hash;
	_entries[hash%_size]._value = value;
	_entries[hash%_size]._height = height;
	if (value <= alpha){
		_entries[hash%_size]._exact = false;
		_entries[hash%_size]._upper = true;
	}
	else if (value >= beta){
		_entries[hash%_size]._exact = false;
		_entries[hash%_size]._upper = false;
	}
	else{
		_entries[hash%_size]._exact = true;
		_entries[hash%_size]._upper = false;
	}
}
const HLEntry& HLTranspositionTable::lookup(const HLState &state, int depth, const HLMove &move)
{
	return _entries[state.getHash(depth, move) % _size];
}
const HLEntry& HLTranspositionTable::lookup(const HLState &state)
{
	return _entries[state.getHash() % _size];
}

HLCacheTable::HLCacheTable(int size) : _size(size), _entries(new HLCacheEntry[size])
{

}
HLCacheTable::~HLCacheTable()
{

}
void HLCacheTable::store(const HLState &origState, int depth, const std::array < HLMove, 2 > &movePair, const HLState &newState)
{
	unsigned int hash = origState.getHash(depth, movePair);
	_entries[hash%_size]._hash = hash;
	_entries[hash%_size]._state = newState;
}
const HLCacheEntry& HLCacheTable::lookup(const HLState &state, int depth, const std::array < HLMove, 2 > &movePair)
{
	return _entries[state.getHash(depth, movePair) % _size];
}