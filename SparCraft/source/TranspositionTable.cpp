#include "TranspositionTable.h"

using namespace SparCraft;

TTEntry::TTEntry()
	: _hash2(0)
	, _depth(0)
	, _type(TTEntry::NONE)
{

}

TTEntry::TTEntry(const HashType & hash2, const StateEvalScore & score, const size_t & depth, const int & type, 
				const IDType & firstPlayer, const AlphaBetaMove & bestFirstMove, const AlphaBetaMove & bestSecondMove)
	: _hash2(hash2)
	, _score(score)
	, _depth(depth)
	, _type(type)
{
	_bestMoves[firstPlayer] = TTBestMove(bestFirstMove, bestSecondMove);
}

const bool TTEntry::hashMatches(const HashType & hash2) const
{
	return hash2 == _hash2;
}

const bool TTEntry::isValid() const
{
	return _type != TTEntry::NONE;
}

const HashType & TTEntry::getHash()								const { return _hash2; }
const StateEvalScore & TTEntry::getScore()						const { return _score; }
const size_t & TTEntry::getDepth()								const { return _depth; }
const int & TTEntry::getType()									const { return _type;  }
const TTBestMove & TTEntry::getBestMove(const IDType & player)	const { return _bestMoves[player];  }
void TTEntry::setBestMove(const IDType &firstPlayer, const AlphaBetaMove & bestFirstMove, const AlphaBetaMove & bestSecondMove)
{
	_bestMoves[firstPlayer] = TTBestMove(bestFirstMove, bestSecondMove);
}

TranspositionTable::TranspositionTable () 
	: TT(Constants::Transposition_Table_Size, TTEntry())
	, size(Constants::Transposition_Table_Size)
    , collisions(0)
    , lookups(0)
    , found(0)
    , notFound(0)
	, saves(0)
	, minIndex(0)
	, maxIndex(0)
	, saveOverwriteSelf(0)
	, saveOverwriteOther(0)
	, saveEmpty(0)
{
}
	
const TTEntry & TranspositionTable::operator [] (const size_t & hash) const
{
	return TT[getIndex(hash)];
}

const TTEntry & TranspositionTable::get(const size_t & hash) const
{
	return TT[getIndex(hash)];
}
        
const size_t TranspositionTable::getSaveIndex(const size_t & index, const HashType & hash2, const size_t & depth) const
{
	size_t worstDepth(1000);
	size_t worstDepthIndex(index);

	// scan ahead to find the best spot to store this entryw
	for (size_t i(index); (i<(index+Constants::Transposition_Table_Scan)) && (i <TT.size()); ++i)
	{
		// if this index holds worse data about the current state, use it to overwrite
		if (TT[i].getHash() == hash2 && TT[i].getDepth() < depth)
		{
			return i;
		}
		else if (TT[i].getHash() == hash2 && TT[i].getDepth() > depth)
		{
			printf("We shouldn't get here\n");
		}
		// otherwise if this entry is empty, use it
		else if (!TT[i].isValid())
		{
			return i;
		}
		// otherwise if the hashes don't match, check to see how old the data is
		else if (TT[i].getHash() != hash2)
		{
			if (TT[i].getDepth() < worstDepth)
			{
				worstDepth = TT[i].getDepth();
				worstDepthIndex = i;
			}
		}
	}

	return worstDepthIndex;
}

void TranspositionTable::save(	const HashType & hash1, const HashType & hash2, const StateEvalScore & value, const size_t & depth, const int & type,
			const IDType & firstPlayer, const AlphaBetaMove & bestFirstMove, const AlphaBetaMove & bestSecondMove)
{
	size_t indexToSave = getSaveIndex(getIndex(hash1), hash2, depth);
	TTEntry existing = TT[indexToSave];

	if (existing.isValid())
	{
		if (existing.getHash() == hash2)
		{
			saveOverwriteSelf++;
		}
		else
		{
			saveOverwriteOther++;
		}
	}
	else
	{
		saveEmpty++;
	}

	saves++;
	
	TT[indexToSave] = TTEntry(hash2, value, depth, type, firstPlayer, bestFirstMove, bestSecondMove);
}

TTEntry * TranspositionTable::lookupScan(const HashType & hash1, const HashType & hash2)
{
	lookups++;
	size_t index = getIndex(hash1);

	// scan to see if this exists anywhere in the next few entries
	for (size_t i(index); (i<(index+Constants::Transposition_Table_Scan)) && (i <TT.size()); ++i)
	{
		TTEntry * entry = lookup(i, hash2);

		// if it does, return it
		if (entry)
		{
			found++;
			return entry;
		}
	}

	// otherwise return null
	notFound++;
	return NULL;
}

// look up a value in the transposition table, return NULL if not found or collision
TTEntry * TranspositionTable::lookup(const size_t & index, const HashType & hash2)
{
	if (index < minIndex) minIndex = index;
	if (index > maxIndex) maxIndex = index;
	
	TTEntry tte = TT[index];

	// if there is a valid entry at that location
	if (TT[index].isValid())
	{
		// test for matching secondary hash
		if (TT[index].hashMatches(hash2))
		{
			found++;
			return &TT[index];
		}
		// if no match it is a collision
		else
		{
			//collisions++;
			return NULL;
		}
	}
	else
	{
		//notFound++;
		return NULL;
	}
}
	
const size_t & TranspositionTable::getSize()		const { return size; }
const size_t & TranspositionTable::numFound()		const { return found; }	
const size_t & TranspositionTable::numNotFound()	const { return notFound; }
const size_t & TranspositionTable::numCollisions()	const { return collisions; }
const size_t & TranspositionTable::numSaves()		const { return saves; }
const size_t & TranspositionTable::numLookups()		const { return lookups; }

const size_t TranspositionTable::getUsage() const
{
	size_t sum(0);
	for (size_t i(0); i<TT.size(); ++i)
	{
		if (TT[i].isValid())
		{
			sum++;
		}
	}

	return sum;
}

void TranspositionTable::print()
{
	std::cout << "TT stats: " << lookups << " lookups, " << found << " found, " << notFound << " not found, " << collisions << " collions. " << minIndex << " min, " << maxIndex << " max.\n";
}