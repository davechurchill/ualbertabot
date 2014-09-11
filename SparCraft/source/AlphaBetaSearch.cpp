#include "AlphaBetaSearch.h"

using namespace SparCraft;

AlphaBetaSearch::AlphaBetaSearch(const AlphaBetaSearchParameters & params, TTPtr TT) 
	: _params(params)
	, _currentRootDepth(0)
	, _TT(TT ? TT : TTPtr(new TranspositionTable()))
{
    for (size_t p(0); p<Constants::Num_Players; ++p)
    {
        // set ordered move script player objects
        for (size_t s(0); s<_params.getOrderedMoveScripts().size(); ++s)
        {
            _allScripts[p].push_back(AllPlayers::getPlayerPtr(p, _params.getOrderedMoveScripts()[s]));
        }

        // set player model objects
        if (_params.playerModel(p) != PlayerModels::None)
        {
            _playerModels[p] = AllPlayers::getPlayerPtr(p, _params.playerModel(p));
        }
    }
}

void AlphaBetaSearch::doSearch(GameState & initialState)
{
	_searchTimer.start();

	StateEvalScore alpha(-10000000, 1000000);
	StateEvalScore beta	( 10000000, 1000000);

	AlphaBetaValue val;

	if (_params.searchMethod() == SearchMethods::AlphaBeta)
	{
		val = alphaBeta(initialState, _params.maxDepth(), Players::Player_None, NULL, alpha, beta);
	}
	else if (_params.searchMethod() == SearchMethods::IDAlphaBeta)
	{
		val = IDAlphaBeta(initialState, _params.maxDepth());
	}

	_results.timeElapsed = _searchTimer.getElapsedTimeInMilliSec();
}

AlphaBetaValue AlphaBetaSearch::IDAlphaBeta(GameState & initialState, const size_t & maxDepth)
{
	AlphaBetaValue val;
	_results.nodesExpanded = 0;
	_results.maxDepthReached = 0;

	for (size_t d(1); d < maxDepth; ++d)
	{
		
		StateEvalScore alpha(-10000000, 999999);
		StateEvalScore beta	( 10000000, 999999);
		
		_results.maxDepthReached = d;
		_currentRootDepth = d;

		// perform ID-AB until time-out
		try
		{
			val = alphaBeta(initialState, d, Players::Player_None, NULL, alpha, beta);

			_results.bestMoves = val.abMove().moveVec();
			_results.abValue = val.score().val();
		}
		// if we do time-out
		catch (int e)
		{
			e += 1;

			// if we didn't finish the first depth, set the move to the best script move
			if (d == 1)
			{
				MoveArray moves;
				const IDType playerToMove(getPlayerToMove(initialState, 1, Players::Player_None, true));
				initialState.generateMoves(moves, playerToMove);
				boost::shared_ptr<Player> bestScript(new Player_NOKDPS(playerToMove));
				bestScript->getMoves(initialState, moves, _results.bestMoves);
			}

			break;
		}

		long long unsigned nodes = _results.nodesExpanded;
		double ms = _searchTimer.getElapsedTimeInMilliSec();

		//printTTResults();
		//fprintf(stdout, "%s %8d %9d %9d %13.4lf %14llu %12d %12llu %15.2lf\n", "IDA", d, val.score().val(), (int)val.abMove().moveTuple(), ms, nodes, (int)_TT->numFound(), getResults().ttcuts, 1000*nodes/ms);
	}

	return val;
}

// Transposition Table save 
void AlphaBetaSearch::TTsave(	GameState & state, const StateEvalScore & value, const StateEvalScore & alpha, const StateEvalScore & beta, const size_t & depth, 
						const IDType & firstPlayer, const AlphaBetaMove & bestFirstMove, const AlphaBetaMove & bestSecondMove) 
{
	// IF THE DEPTH OF THE ENTRY IS BIGGER THAN CURRENT DEPTH, DO NOTHING
	TTEntry * entry = _TT->lookupScan(state.calculateHash(0), state.calculateHash(1));
	bool valid = entry && entry->isValid();
	size_t edepth = entry ? entry->getDepth() : 0;

	_results.ttSaveAttempts++;
	
	if (valid && (edepth > depth)) 
	{
		return;
	}
	
	int type(TTEntry::NONE);

	if      (value <= alpha) type = TTEntry::UPPER;
	else if (value >= beta)  type = TTEntry::LOWER;
	else                     type = TTEntry::ACCURATE;

	// SAVE A NEW ENTRY IN THE TRANSPOSITION TABLE
	_TT->save(state.calculateHash(0), state.calculateHash(1), value, depth, type, firstPlayer, bestFirstMove, bestSecondMove);
}

// Transposition Table look up + alpha/beta update
TTLookupValue AlphaBetaSearch::TTlookup(const GameState & state, StateEvalScore & alpha, StateEvalScore & beta, const size_t & depth)
{
	TTEntry * entry = _TT->lookupScan(state.calculateHash(0), state.calculateHash(1));
	if (entry && (entry->getDepth() == depth)) 
	{
		// get the value and type of the entry
		StateEvalScore TTvalue = entry->getScore();
		
		// set alpha and beta depending on the type of entry in the TT
		if (entry->getType() == TTEntry::LOWER)
		{
			if (TTvalue > alpha) 
			{
				alpha = TTvalue;
			}
		}
		else if (entry->getType() == TTEntry::UPPER) 
		{
			if (TTvalue < beta)
			{
				beta  = TTvalue;
			}
		} 
		else
		{
			printf("LOL\n");
			alpha = TTvalue;
			beta = TTvalue;
		}
		
		if (alpha >= beta) 
		{
			// this will be a cut
			_results.ttcuts++;
			return TTLookupValue(true, true, entry);
		}
		else
		{
			// found but no cut
			_results.ttFoundNoCut++;
			return TTLookupValue(true, false, entry);
		}
	}
	else if (entry)
	{
		_results.ttFoundLessDepth++;
		return TTLookupValue(true, false, entry);
	}

	return TTLookupValue(false, false, entry);
}

const bool AlphaBetaSearch::searchTimeOut()
{
	return (_params.timeLimit() && (_results.nodesExpanded % 50 == 0) && (_searchTimer.getElapsedTimeInMilliSec() >= _params.timeLimit()));
}

const bool AlphaBetaSearch::terminalState(GameState & state, const size_t & depth) const
{
	return (depth <= 0 || state.isTerminal());
}

const AlphaBetaMove & AlphaBetaSearch::getAlphaBetaMove(const TTLookupValue & TTval, const IDType & playerToMove) const
{
	const IDType enemyPlayer(getEnemy(playerToMove));

	// if we have a valid first move for this player, use it
	if (TTval.entry()->getBestMove(playerToMove).firstMove().isValid())
	{
		return TTval.entry()->getBestMove(playerToMove).firstMove();
	}
	// otherwise return the response to an opponent move, if it doesn't exist it will just be invalid
	else
	{
		return TTval.entry()->getBestMove(enemyPlayer).secondMove();
	}
}

void AlphaBetaSearch::generateOrderedMoves(GameState & state, MoveArray & moves, const TTLookupValue & TTval, const IDType & playerToMove, const size_t & depth)
{
	// get the array where we will store the moves and clear it
	Array<std::vector<UnitAction>, Constants::Max_Ordered_Moves> & orderedMoves(_orderedMoves[depth]);
	orderedMoves.clear();

	// if we are using opponent modeling, get the move and then return, we don't want to put any more moves in
	if (_params.playerModel(playerToMove) != PlayerModels::None)
	{
        // put the vector into the ordered moves array
        orderedMoves.add(std::vector<UnitAction>());

        // generate the moves into that vector
        _playerModels[playerToMove]->getMoves(state, moves, orderedMoves[0]);
		
		return;
	}

	// if there is a transposition table entry for this state
	/*if (TTval.found())
	{
		// get the abMove we stored for this player
		const AlphaBetaMove & abMove = getAlphaBetaMove(TTval, playerToMove);

		_results.ttFoundCheck++;

		// Two checks:
		// 1) Is the move 'valid' ie: was it actually set inside the TT
		// TODO:: Possibly validate this move a second time somehow
        //        Was previously done with move tuple numbers
		if (abMove.isValid())
		{
			orderedMoves.add(abMove.moveVec());
			_results.ttMoveOrders++;
			return;
		}
		else
		{
			_results.ttFoundButNoMove++;
		}
	}*/

    if (depth == 2)
    {
        int a = 6;
    }

    // if we are using script move ordering, insert the script moves we want
    if (_params.moveOrderingMethod() == MoveOrderMethod::ScriptFirst)
    {
        for (size_t s(0); s<_params.getOrderedMoveScripts().size(); s++)
	    {
            std::vector<UnitAction> moveVec;
		    _allScripts[playerToMove][s]->getMoves(state, moves, moveVec);
		    orderedMoves.add(moveVec);
	    }

        if (orderedMoves.size() < 2)
        {
            int a = 6;
        }
    }
}

bool AlphaBetaSearch::getNextMoveVec(IDType playerToMove, MoveArray & moves, const size_t & moveNumber, const TTLookupValue & TTval, const size_t & depth, std::vector<UnitAction> & moveVec) const
{
    if (_params.maxChildren() && (moveNumber >= _params.maxChildren()))
    {
        return false;
    }

    // if this move is beyond the first, check to see if we are only using a single move
    if (moveNumber == 1)
    {
        // if we are player modeling, we should have only generated the first move
        if (_params.playerModel(playerToMove) != PlayerModels::None)
	    {
            // so return false
		    return false;
	    }

	    // if there is a transposition table entry for this state
	    if (TTval.found())
	    {
		    // if there was a valid move found with higher depth, just do that one
		    const AlphaBetaMove & abMove = getAlphaBetaMove(TTval, playerToMove);
		    if ((TTval.entry()->getDepth() >= depth) && abMove.isValid())
		    {
                // so return false
			    return false;
		    }
	    }
    }

	const Array<std::vector<UnitAction>, Constants::Max_Ordered_Moves> & orderedMoves(_orderedMoves[depth]);
    moveVec.clear();
   
	// if this move should be from the ordered list, return it from the list
	if (moveNumber < orderedMoves.size())
	{
        moveVec.assign(orderedMoves[moveNumber].begin(), orderedMoves[moveNumber].end());
        return true;
	}
	// otherwise return the next move vector starting from the beginning
	else
	{
        if (moves.hasMoreMoves())
        {
            moves.getNextMoveVec(moveVec);
            return true;
        }
        else
        {
            return false;
        }
	}
}

const IDType AlphaBetaSearch::getPlayerToMove(GameState & state, const size_t & depth, const IDType & lastPlayerToMove, const bool isFirstSimMove) const
{
	const IDType whoCanMove(state.whoCanMove());

	// if both players can move
	if (whoCanMove == Players::Player_Both)
	{
		// no matter what happens, the 2nd player to move is always the enemy of the first
		if (!isFirstSimMove)
		{
			return getEnemy(lastPlayerToMove);
		}

		// pick the first move based on our policy
		const IDType policy(_params.playerToMoveMethod());
		const IDType maxPlayer(_params.maxPlayer());

		if (policy == SparCraft::PlayerToMove::Alternate)
		{
			return isRoot(depth) ? maxPlayer : getEnemy(lastPlayerToMove);
		}
		else if (policy == SparCraft::PlayerToMove::Not_Alternate)
		{
			return isRoot(depth) ? maxPlayer : lastPlayerToMove;
		}
		else if (policy == SparCraft::PlayerToMove::Random)
		{
			// srand(state.calculateHash(0));
			return isRoot(depth) ? maxPlayer : rand() % 2;
		}

		// we should never get to this state
		System::FatalError("AlphaBeta Error: Nobody can move for some reason");
		return Players::Player_None;
	}
	else
	{
		return whoCanMove;
	}
}

const bool AlphaBetaSearch::isTranspositionLookupState(GameState & state, const std::vector<UnitAction> * firstSimMove) const
{
	return !state.bothCanMove() || (state.bothCanMove() && !firstSimMove);
}

AlphaBetaValue AlphaBetaSearch::alphaBeta(GameState & state, size_t depth, const IDType lastPlayerToMove, std::vector<UnitAction> * prevSimMove, StateEvalScore alpha, StateEvalScore beta)
{
	// update statistics
	_results.nodesExpanded++;

	if (searchTimeOut())
	{
		throw 1;
	}
    
	if (terminalState(state, depth))
	{
		// return the value, but the move will not be valid since none was performed
        StateEvalScore evalScore = state.eval(_params.maxPlayer(), _params.evalMethod(), _params.simScript(Players::Player_One), _params.simScript(Players::Player_Two));
		
		return AlphaBetaValue(StateEvalScore(evalScore.val(), state.getNumMovements(_params.maxPlayer()) + evalScore.numMoves() ), AlphaBetaMove());
	}

	// figure out which player is to move
	const IDType playerToMove(getPlayerToMove(state, depth, lastPlayerToMove, !prevSimMove));

	// is the player to move the max player?
	bool maxPlayer = (playerToMove == _params.maxPlayer());

	// Transposition Table Logic
	TTLookupValue TTval;
	if (isTranspositionLookupState(state, prevSimMove))
	{
		TTval = TTlookup(state, alpha, beta, depth);

		// if this is a TT cut, return the proper value
		if (TTval.cut())
		{
			return AlphaBetaValue(TTval.entry()->getScore(), getAlphaBetaMove(TTval, playerToMove));
		}
	}

	bool bestMoveSet(false);

	// move generation
	MoveArray & moves = _allMoves[depth];
	state.generateMoves(moves, playerToMove);
    moves.shuffleMoveActions();
	generateOrderedMoves(state, moves, TTval, playerToMove, depth);

	// while we have more simultaneous moves
	AlphaBetaMove bestMove, bestSimResponse;
	    
    size_t moveNumber(0);
    std::vector<UnitAction> moveVec;

    // for each child
    while (getNextMoveVec(playerToMove, moves, moveNumber, TTval, depth, moveVec))
	{
        // the value of the recursive AB we will call
		AlphaBetaValue val;
		
		// generate the child state
		GameState child(state);

		bool firstMove = true;

		// if this is the first player in a simultaneous move state
		if (state.bothCanMove() && !prevSimMove && (depth != 1))
		{
			firstMove = true;
			// don't generate a child yet, just pass on the move we are investigating
			val = alphaBeta(state, depth-1, playerToMove, &moveVec, alpha, beta);
		}
		else
		{
			firstMove = false;

			// if this is the 2nd move of a simultaneous move state
			if (prevSimMove)
			{
				// do the previous move selected by the first player to move during this state
                child.makeMoves(*prevSimMove);
			}

			// do the moves of the current player
            child.makeMoves(moveVec);
			child.finishedMoving();

			// get the alpha beta value
			val = alphaBeta(child, depth-1, playerToMove, NULL, alpha, beta);
		}

		// set alpha or beta based on maxplayer
		if (maxPlayer && (val.score() > alpha)) 
		{
			alpha = val.score();
			bestMove = AlphaBetaMove(moveVec, true);
			bestMoveSet = true;

			if (state.bothCanMove() && !prevSimMove)
			{
				bestSimResponse = val.abMove();
			}

			// if this is depth 1 of the first try at depth 1, store the best in results
		}
		else if (!maxPlayer && (val.score() < beta))
		{
			beta = val.score();
			bestMove = AlphaBetaMove(moveVec, true);
			bestMoveSet = true;

			if (state.bothCanMove() && prevSimMove)
			{
				bestSimResponse = val.abMove();
			}
		}

		if (alpha.val() == -10000000 && beta.val() == 10000000)
		{
			fprintf(stderr, "\n\nALPHA BETA ERROR, NO VALUE SET\n\n");
		}

		// alpha-beta cut
		if (alpha >= beta) 
		{ 
			break; 
		}

        moveNumber++;
	}
	
	if (isTranspositionLookupState(state, prevSimMove))
	{
		TTsave(state, maxPlayer ? alpha : beta, alpha, beta, depth, playerToMove, bestMove, bestSimResponse);
	}

	return maxPlayer ? AlphaBetaValue(alpha, bestMove) : AlphaBetaValue(beta, bestMove);
}


AlphaBetaSearchResults & AlphaBetaSearch::getResults()
{
	return _results;
}

const IDType AlphaBetaSearch::getEnemy(const IDType & player) const
{
	return (player + 1) % 2;
}

const bool AlphaBetaSearch::isRoot(const size_t & depth) const
{
	return depth == _currentRootDepth;
}

void AlphaBetaSearch::printTTResults() const
{
	printf("\n");
	printf("Total Usage            %9d\n", (int)_TT->getUsage());
	printf("Save Attempt           %9d\n", (int)_results.ttSaveAttempts);
	printf("   Save Succeed        %9d\n", (int)_TT->numSaves());
	printf("      Save Empty       %9d\n", (int)_TT->saveEmpty);
	printf("      Save Self        %9d\n", (int)_TT->saveOverwriteSelf);
	printf("      Save Other       %9d\n", (int)_TT->saveOverwriteOther);
	printf("Look-Up                %9d\n", (int)_TT->numLookups());
	printf("   Not Found           %9d\n", (int)_TT->numNotFound());
	printf("   Collisions          %9d\n", (int)_TT->numCollisions());
	printf("   Found               %9d\n", (int)_TT->numFound());
	printf("      Less Depth       %9d\n", (int)_results.ttFoundLessDepth);
	printf("      More Depth       %9d\n", ((int)_results.ttFoundCheck + (int)_results.ttcuts));
	printf("         Cut           %9d\n", (int)_results.ttcuts);
	printf("         Move          %9d\n", (int)_results.ttMoveOrders);
	printf("         No Move       %9d\n", (int)_results.ttFoundButNoMove);
	printf("\n");
}
