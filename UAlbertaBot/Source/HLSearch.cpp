#include "HLSearch.h"
#include "../../BOSS/source/BOSS.h"

using namespace UAlbertaBot;

HLSearch::HLSearch()
{
}


HLSearch::~HLSearch()
{
}

// get an instance of this
HLSearch & HLSearch::Instance()
{
	static HLSearch instance;
	return instance;
}

void HLSearch::search(double timeLimit)
{
	Logger::LogAppendToFile(UAB_LOGFILE, "\n\nStarting search\n\n");
	_stats.clear();
	_stats.startTimer();
	HLState state(BWAPI::Broodwar, BWAPI::Broodwar->self(), BWAPI::Broodwar->enemy());
	int score = alphaBeta(state, 0, state.currentFrame()+3000, BWAPI::Broodwar->self()->getID(), HLMove(), MIN_SCORE, MAX_SCORE);
	Logger::LogAppendToFile(UAB_LOGFILE, _stats.header()+"\n");
	Logger::LogAppendToFile(UAB_LOGFILE, _stats.toString()+"\n");
	//exit(EXIT_SUCCESS);
}

int HLSearch::alphaBeta(const HLState& state, int depth, int frameLimit, int turn, const HLMove &firstSideMove, int alpha, int beta)
{
	_stats.incNodeCount();
	if (firstSideMove.isEmpty() && (state.currentFrame() >= frameLimit || state.gameOver())){
		return state.evaluate(turn);
	}

	int score = MIN_SCORE;
	int al = alpha;
	HLMove bestMove;
	int i = 0;
	for (auto m : state.getMoves(turn)){
		//Logger::LogAppendToFile(UAB_LOGFILE, "Searching depth %d, move %s, move %s\n", depth, firstSideMove.toString().c_str(), m.toString().c_str());

		int value;
		if (firstSideMove.isEmpty()){
			value = alphaBeta(state, depth + 1, frameLimit, 1 - turn, m, -beta, -al);
		}
		else{
			HLState newState(state);
			//Logger::LogAppendToFile(UAB_LOGFILE, "Start frame: %d\n", newState.currentFrame());
			try
			{
				if (turn == 1){
					newState.applyAndForward(std::array < HLMove, 2 > { {firstSideMove, m} });
				}
				else{//move for player 0 first
					newState.applyAndForward(std::array < HLMove, 2 > { {m, firstSideMove} });
				}
			}
			catch (const BOSS::Assert::BOSSException &){
				continue;//couldn't find legal build order, skip
			}
			//Logger::LogAppendToFile(UAB_LOGFILE, "New frame: %d\n", newState.currentFrame());
			if (state.currentFrame() == newState.currentFrame()){//didn't progress
				continue;
			}
			_stats.addFwd(newState.currentFrame() - state.currentFrame());
			value = alphaBeta(newState, depth + 1, frameLimit, 1 - turn, HLMove(), -beta, -al);
		}
		i++;
		if (value > score){
			score = value;
			bestMove = m;
			if (value > al){
				al = value;
				if (al >= beta){
					break;
				}
			}
		}
		//break;//temporary, let's only check 1 move wide
	}
	_stats.addBF(i);
	if (depth == 0){
		_bestMove = bestMove;
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