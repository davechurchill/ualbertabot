#pragma once

#include <vector>
#include "Common.h"
#include "GameState.h"
#include "UnitAction.hpp"

namespace SparCraft
{

class TTEntry
{	
public:

	enum { NONE, UPPER, LOWER, ACCURATE };

private:

	HashType			_hash2;
	StateEvalScore		_score;
	size_t				_depth;
	TTBestMove			_bestMoves[2];
	int					_type;

public:

	TTEntry();
	TTEntry(const HashType & hash2, const StateEvalScore & score, const size_t & depth, const int & type, 
			const IDType & firstPlayer, const AlphaBetaMove & bestFirstMove, const AlphaBetaMove & bestSecondMove);

	const bool hashMatches(const HashType & hash2) const;

	const bool isValid() const;

	const HashType & getHash()									const;
	const StateEvalScore & getScore()							const;
	const size_t & getDepth()									const;
	const int & getType()										const;
	const TTBestMove & getBestMove(const IDType & player)		const;
	void setBestMove(const IDType &firstPlayer, const AlphaBetaMove & bestFirstMove, const AlphaBetaMove & bestSecondMove);

	void print() const
	{
		printf ("%d, %d, %d, %d\n", (int) _hash2, (int) _score.val(), (int) _depth, (int) _type);
	}
};


class TTLookupValue
{
	bool		_found;		// did we find a value?
	bool		_cut;		// should we produce a cut?
	TTEntry *	_entry;		// the entry we found

public:

	TTLookupValue()
		: _found(false)
		, _cut(false)
		, _entry(NULL)
	{
	}

	TTLookupValue(const bool found, const bool cut, TTEntry * entry)
		: _found(found)
		, _cut(cut)
		, _entry(entry)
	{
		
	}

	const bool found() const	{ return _found; }
	const bool cut() const		{ return _cut; }
	TTEntry * entry() const		{ return _entry; }
};

class TranspositionTable 
{
	//Array<TTEntry, Constants::Transposition_Table_Size>	TT;
	//TTEntry TT[Constants::Transposition_Table_Size];
	std::vector<TTEntry> TT;

	size_t			size;

	size_t			minIndex,
					maxIndex;

	const size_t getIndex(const HashType & hash1) const
	{
		return hash1 % size; 
	}

public:

	size_t			collisions,
					lookups, 
					found, 
					notFound,
					saves,
					saveOverwriteSelf,
					saveOverwriteOther,
					saveEmpty;

	TranspositionTable ();
	
	const TTEntry & operator [] (const size_t & hash) const;

	const TTEntry & get(const size_t & hash) const;
        
	void save(const HashType & hash1, const TTEntry & entry) ;

	void save(	const HashType & hash1, const HashType & hash2, const StateEvalScore & value, const size_t & depth, const int & type,
				const IDType & firstPlayer, const AlphaBetaMove & bestFirstMove, const AlphaBetaMove & bestSecondMove);

	TTEntry * lookup(const size_t & index, const HashType & hash2);
	TTEntry * lookupScan(const HashType & hash1, const HashType & hash2);
	
	const size_t & getSize()		const;
	const size_t & numFound()		const;	
	const size_t & numNotFound()	const;
	const size_t & numCollisions()	const;
	const size_t & numSaves()		const;
	const size_t & numLookups()		const;

	const size_t getUsage() const;

	const size_t getSaveIndex(const size_t & index, const HashType & hash2, const size_t & depth) const;

	void print();
};

typedef	boost::shared_ptr<TranspositionTable> TTPtr;
}