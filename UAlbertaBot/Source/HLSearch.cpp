#include "HLSearch.h"
#include "../../BOSS/source/BOSS.h"

using namespace UAlbertaBot;

HLSearch::HLSearch() :_tt(20000), _cache(20000), _bestMove(StrategyManager::ProtossHighLevelSearch)
{
}


HLSearch::~HLSearch()
{
}

long long HLSearch::search(double timeLimit, int frameLimit, int maxHeight)
{
	
	_stats.clear();
	_stats.startTimer();
	_timeUp = false;
	_timeLimitMs = timeLimit;
	

	HLState state(BWAPI::Broodwar, BWAPI::Broodwar->self(), BWAPI::Broodwar->enemy());
	int currentEval = state.evaluate(BWAPI::Broodwar->self()->getID());
	Logger::LogAppendToFile(UAB_LOGFILE, "\n\nStarting search at frame %d, static eval: %d\n", 
		BWAPI::Broodwar->getFrameCount(), 
		currentEval);


	//if (currentEval > 150 || currentEval < -200)
	//{
	//	Logger::LogAppendToFile(UAB_LOGFILE, "Game seems decided, skipping HL search\n");
	//	return _stats.getRunningTimeMilliSecs();
	//}
	_minFrame = 1000000;
	Logger::LogAppendToFile(UAB_LOGFILE, _stats.header() + " score\tbest move\tmin frame\tmax frame\n"); 
	//for (int height = 2; height <= maxHeight && !_timeUp; height += 2)
	//{
	int height = maxHeight;
	for (int frames = 2000; frames <= frameLimit && !_timeUp; frames += 2000)
	{
		_maxFrame = 0;
		int prevFrame = _minFrame;
		_minFrame = 1000000;
		int score = alphaBeta(state, 0, height, state.currentFrame() + frames, BWAPI::Broodwar->self()->getID(), HLMove(), MIN_SCORE, MAX_SCORE);
		Logger::LogAppendToFile(UAB_LOGFILE, _stats.toString());
		Logger::LogAppendToFile(UAB_LOGFILE, " %d %s %d %d\n", score, _bestMove.toString().c_str(), _minFrame - state.currentFrame(), _maxFrame - state.currentFrame());
		if (prevFrame == _minFrame || (_minFrame - state.currentFrame()) > frameLimit)
		{
			break;
		}
	}

	return _stats.getRunningTimeMilliSecs();
}

int HLSearch::alphaBeta(const HLState& state, int depth, int height, int frameLimit, int turn, const HLMove &firstSideMove, int alpha, int beta)
{
	_stats.incNodeCount();
	
	if (firstSideMove.isEmpty() && (height <=0 || state.currentFrame() >= frameLimit || state.gameOver())){
		if (state.currentFrame() < _minFrame)
		{
			_minFrame = state.currentFrame();
		}
		if (state.currentFrame() > _maxFrame)
		{
			_maxFrame = state.currentFrame();
		}
		return state.evaluate(turn);
	}
	if (_stats.getNodeCount() % 10 && _stats.getRunningTimeMilliSecs() > _timeLimitMs)
	{
		_timeUp = true;
	}
	int score = MIN_SCORE;
	int al = alpha;
	HLMove bestMove;
	int i = 0;
	HLMove ttBest;
	if (firstSideMove.isEmpty())
	{
		_stats.incTTquery();
		const HLEntry &e = _tt.lookup(state);
		if (e._hash == state.getHash())
		{
			_stats.incTTfound();
			ttBest = e._bestMove;
		}
	}
	else
	{
		_stats.incTTquery();
		const HLEntry &e = _tt.lookup(state, depth,firstSideMove);
		if (e._hash == state.getHash(depth, firstSideMove))
		{
			_stats.incTTfound();
			ttBest = e._bestMove;
		}
	}
	////todo: use value to return or at least shrink alpha-beta window?

	auto &moves = state.getMoves(turn);
	UAB_ASSERT(moves.size() != 0, "No moves generated at depth %d", depth);
	if (!ttBest.isEmpty())
	{
		auto it=std::find(moves.begin(), moves.end(), ttBest);
		UAB_ASSERT(it != moves.end(), "Couldn't find best move among valid moves");
		if (it != moves.end())
		{
			std::iter_swap(it, moves.begin());
		}
	}
	//std::sort(moves.begin(), moves.end(), [this, &state, depth](const HLMove &m1, const HLMove &m2)
	//{
	//	_stats.incTTquery();
	//	const HLEntry &e1 = _tt.lookup(state, depth, m1);
	//	if (e1._hash != state.getHash(depth, m1))
	//	{
	//		return false;
	//	}
	//	_stats.incTTfound();
	//	_stats.incTTquery();
	//	const HLEntry &e2 = _tt.lookup(state, depth, m2);
	//	if (e2._hash != state.getHash(depth, m2))
	//	{
	//		return true;
	//	}
	//	_stats.incTTfound();
	//	return e1._value > e2._value;
	//});
	
	for (auto it = moves.begin(); it != moves.end() && !_timeUp;it++){
		auto m = *it;
		//Logger::LogAppendToFile(UAB_LOGFILE, "Searching depth %d, move %s, move %s\n", depth, firstSideMove.toString().c_str(), m.toString().c_str());

		int value;
		if (firstSideMove.isEmpty()){
			value = alphaBeta(state, depth + 1, height - 1, frameLimit, 1 - turn, m, -beta, -al);
		}
		else{

			//Logger::LogAppendToFile(UAB_LOGFILE, "Start frame: %d\n", newState.currentFrame());

			std::array < HLMove, 2 > movePair;
			if (turn == 1){
				movePair = std::array < HLMove, 2 > { {firstSideMove, m} };
			}
			else{//move for player 1 first
				movePair = std::array < HLMove, 2 > { {m, firstSideMove} };
			}
			//Logger::LogAppendToFile(UAB_LOGFILE, "Turn %d depth %d Checking moves: %s %s hash: %u\n", turn, depth,
			//	firstSideMove.toString().c_str(), m.toString().c_str(), state.getHash(depth,movePair));
			try
			{
				_stats.incCacheQuery();
				const auto &entry = _cache.lookup(state, depth, movePair);
				if (entry._hash == state.getHash(depth, movePair))//found
				{
					_stats.incCacheFound();
					//Logger::LogAppendToFile(UAB_LOGFILE, "Moves "+ firstSideMove.toString() + m.toString()+" matches\n");
					//Logger::LogAppendToFile(UAB_LOGFILE, " hashes: %u %u\n", entry._hash, entry._state.getHash());
					if (state.currentFrame() == entry._state.currentFrame()){//didn't progress
						//Logger::LogAppendToFile(UAB_LOGFILE, "No progress (cached)");
						continue;
					} 
					value = alphaBeta(entry._state, depth + 1, height - 1, frameLimit, 1 - turn, HLMove(), -beta, -al);
				}
				else
				{
					HLState newState(state);
					newState.applyAndForward(depth, frameLimit-newState.currentFrame(), movePair);
					if (state.currentFrame() == newState.currentFrame()){//didn't progress
						//Logger::LogAppendToFile(UAB_LOGFILE, "No progress");
						continue;
					}
					_stats.addFwd(newState.currentFrame() - state.currentFrame());
					_cache.store(state, depth, movePair, newState);
					value = alphaBeta(newState, depth + 1, height - 1, frameLimit, 1 - turn, HLMove(), -beta, -al);
				}

			}
			catch (const BOSS::Assert::BOSSException &){
				//Logger::LogAppendToFile(UAB_LOGFILE, "No Legal build order");
				continue;//couldn't find legal build order, skip
			}
			
		}

		i++;
		if (value > score){
			score = value;
			bestMove = m;
			if (value > alpha){
				al = value;
				if (al >= beta){
					break;
				}
			}
		}
		//break;//temporary, let's only check 1 move wide
	}
	if (!_timeUp)
	{
		_stats.addBF(i);

		if (firstSideMove.isEmpty())
		{
			_tt.store(state, bestMove, score, alpha, beta, height);
		}
		else
		{
			_tt.store(state, depth, firstSideMove, bestMove, score, alpha, beta, height);
		}
	}
	if (depth == 0){
		if (!bestMove.isEmpty())
		{
			_bestMove = bestMove;
		}
		else if (height == 2)//only use default move if first ID iteration
		{
			//_bestMove = HLMove(StrategyManager::ProtossHighLevelSearch);
			Logger::LogAppendToFile(UAB_LOGFILE, "Couldn't find a strategy, using default\n");
		}
	}

	return score;
}

HLMove HLSearch::getBestMove()
{
	return _bestMove;
}

//std::vector<Squad> HLSearch::getSquads(const std::set<BWAPI::UnitInterface*> & combatUnits)
//{
//	auto searchSquads = _bestMove.getSquads();
//	std::set<BWAPI::UnitInterface*> unitsToAdd(combatUnits.begin(),combatUnits.end());
//	
//	//remove non combat units
//	for (auto &s : searchSquads){
//		for (unsigned int i = 0; i < s.getUnits().size();i++){
//			auto &u = s.getUnits()[i];
//			if (combatUnits.count(u) == 0){//if unit is not combat, remove it from squad
//				s.removeUnit(u);
//				i--;
//			}
//			else{//unit already assigned
//				unitsToAdd.erase(u);
//			}
//		}
//	}
//
//	//assign remaining units
//	for (auto &u : unitsToAdd){
//		bool assigned=false;
//		for (auto &s : searchSquads){
//			if (u->getRegion() == s.getRegion()){
//				s.addUnit(u);
//				assigned = true;
//				break;
//			}
//		}
//		if (!assigned){//create new squad
//			std::vector<BWAPI::UnitInterface *> unit(1, u);
//			searchSquads.push_back(HLSquad(unit, SquadOrder()));
//		}
//	}
//
//	std::vector<Squad> squads(searchSquads.size());
//	//transform HLSquad into Squad
//	std::transform(searchSquads.begin(), searchSquads.end(), squads.begin(),
//		[](const HLSquad &s){return static_cast<Squad>(s); });
//
//	return squads;
//}