#include "HLState.h"

using namespace UAlbertaBot;

HLSquad::HLSquad(const Squad &squad) : Squad(squad), region(BWAPI::Broodwar->getRegionAt(calcCenter()))
{

}

HLSquad::HLSquad(const std::vector<BWAPI::UnitInterface *> & units, SquadOrder order) :
HLSquad(Squad(units, order))
{

}

HLState::HLState(BWAPI::GameWrapper & game, BWAPI::PlayerInterface * player, BWAPI::PlayerInterface * enemy)
{

	UAB_ASSERT(player->getID() < 2, "Only player ids 0 and 1 supported");
	UAB_ASSERT(enemy->getID() < 2, "Only player ids 0 and 1 supported");
	_state[player->getID()] = BOSS::GameState(game, player);
	_state[enemy->getID()] = BOSS::GameState(game, enemy);
	_players[player->getID()] = player;
	_players[enemy->getID()] = enemy;

	addSquads(game, player->getID());
	addSquads(game, enemy->getID());

	//_buildOrderIndex[0] = 0;
	//_buildOrderIndex[1] = 0;

	//TODO:change this to take UnitData as parameter
	_unitData[player->getID()] = InformationManager::Instance().getUnitData(player);
	_unitData[enemy->getID()] = InformationManager::Instance().getUnitData(enemy);

	_workerData[player->getID()] = WorkerManager::Instance().getData();
	_workerData[enemy->getID()]	= WorkerData();
	for (auto &u : enemy->getUnits()){
		if (u->getType().isWorker()){
			_workerData[enemy->getID()].addWorker(u);
		}
	}
	//todo: assign worker jobs
	//3 per gas, 3 per mineral patch, 1 to build, and if scout outside nexus region
}
void HLState::addSquads(const BWAPI::GameWrapper & game, int playerID)
{
	for (auto region : game->getAllRegions()){
		auto unitSet = region->getUnits([playerID](BWAPI::Unit unit)
		{
			return unit
				&&unit->isCompleted()
				&& unit->getHitPoints() > 0
				&& unit->exists()
				&& unit->getType() != BWAPI::UnitTypes::Unknown
				&& unit->getPosition().x != BWAPI::Positions::Unknown.x
				&& unit->getPosition().y != BWAPI::Positions::Unknown.y
				&& (unit->getType().canAttack() ||
				unit->getType() == BWAPI::UnitTypes::Terran_Medic ||
				unit->getType() == BWAPI::UnitTypes::Protoss_High_Templar ||
				unit->getType() == BWAPI::UnitTypes::Protoss_Observer)
				&& unit->getPlayer()->getID() == playerID;
		});
		if (!unitSet.empty()){
			std::vector<BWAPI::Unit> units(unitSet.begin(), unitSet.end());
			_squad[playerID].push_back(HLSquad(units, SquadOrder()));
		}
	}
}

HLState::~HLState()
{
}

//void HLState::forwardGameState(int frames, int playerID)
//{
//	if (_buildOrderIndex[playerID] >= _buildOrder[playerID].size()){
//
//		UAB_ASSERT(BOSS::Races::GetRace(_state[playerID].getRace()) == BWAPI::Races::Protoss, "Non protoss?");
//
//		//get goal units
//		auto goalUnits = StrategyManager::Instance().getBuildOrderGoal(
//			_unitData[playerID],
//			_unitData[1 - playerID],
//			_workerData[playerID],
//			_state[playerID].getCurrentFrame(),
//			0,//strategy
//			getRace(playerID));
//
//		try{
//			BOSS::BuildOrderSearchGoal goal(_state[playerID].getRace());
//
//			for (size_t i = 0; i < goalUnits.size(); ++i)
//			{
//				MetaType type = goalUnits[i].first;
//				BOSS::ActionType actionType;
//				if (type.isUnit())
//				{
//					actionType = type.unitType;
//				}
//				else if (type.isUpgrade())
//				{
//					actionType = type.upgradeType;
//				}
//				else if (type.isTech())
//				{
//					actionType = type.techType;
//				}
//				else{
//					UAB_ASSERT(false, "Should have found a valid type here");
//				}
//				goal.setGoal(actionType, goalUnits[i].second);
//			}
//			//perform search
//			BOSS::DFBB_BuildOrderSmartSearch smartSearch(_state[playerID].getRace());
//			smartSearch.setGoal(goal);
//			smartSearch.setState(_state[playerID]);
//			smartSearch.setTimeLimit(1000);
//			smartSearch.search();
//
//
//			//get build order from search
//			if (smartSearch.getResults().solved){
//				_buildOrder[playerID] = smartSearch.getResults().buildOrder;
//				BWAPI::Broodwar->printf("Build order found");
//			}
//			else{
//				BOSS::NaiveBuildOrderSearch nbos(smartSearch.getParameters().initialState, smartSearch.getParameters().goal);
//				_buildOrder[playerID] = nbos.solve();
//				BWAPI::Broodwar->printf("Build order not found, using naive");
//			}
//
//		}catch (BOSS::Assert::BOSSException &e){
//			BWAPI::Broodwar->printf(e.what());
//		}
//	}
//	//move forward the world, executing the build order
//	int remainingFrames = frames;
//	for (; _buildOrderIndex[playerID] < _buildOrder[playerID].size(); _buildOrderIndex[playerID]++){
//		//BWAPI::Broodwar->printf("Frame: %d, next action: %s",initialState.getCurrentFrame(),order[i].getMetaName().c_str());
//		int nextActionStart = _state[playerID].whenCanPerform(_buildOrder[playerID][_buildOrderIndex[playerID]]);
//		if (nextActionStart < remainingFrames){
//			_state[playerID].doAction(_buildOrder[playerID][_buildOrderIndex[playerID]]);
//			remainingFrames -= nextActionStart;
//		}
//		else{
//			break;
//		}
//	}
//	//TODO: add new units to unitdata. Create squad defending home base with new units.
//	//TODO: check case where the build order (preexistent or newly acquired) is too short and we need to get a second one
//	if (remainingFrames > 0){
//		_state[playerID].fastForward(remainingFrames);
//	}
//	BWAPI::Broodwar->printf("Final Frame: %d", _state[playerID].getCurrentFrame());
//
//	//TODO: execute squad orders
//}

//bool HLState::undecidedChoicePoint(int playerID, int strategy, const std::unordered_map<short, short> choices) const{
//	try{
//		StrategyManager::Instance().getBuildOrderGoal(
//			_unitData[playerID],
//			_unitData[1 - playerID],
//			_workerData[playerID],
//			currentFrame(),
//			strategy,
//			getRace(playerID),
//			choices);
//	}
//	catch (const ChoicePoint &c){
//		return true;//if some script hit an internal choice point
//	}
//	return false;
//}
void HLState::applyAndForward(const std::array<HLMove, 2> &moves)
{
	UAB_ASSERT(getRace(0) == BWAPI::Races::Protoss, "Non protoss?");
	UAB_ASSERT(getRace(1) == BWAPI::Races::Protoss, "Non protoss?");

	//if (undecidedChoicePoint(0, movePlayer0.getStrategy(), movePlayer0.getChoices()) ||
	//	undecidedChoicePoint(1, movePlayer1.getStrategy(), movePlayer1.getChoices())){
	//	return;
	//}

	MetaPairVector goalUnits[2];
	try{
		for (int playerID = 0; playerID < 2; playerID++){
			goalUnits[playerID] = StrategyManager::Instance().getBuildOrderGoal(
				_unitData[playerID],
				_unitData[1 - playerID],
				_workerData[playerID],
				currentFrame(),
				moves[playerID].getStrategy(),
				getRace(playerID),
				moves[playerID].getChoices());
			Logger::LogAppendToFile(UAB_LOGFILE, "player %d goals: ", playerID);
			for (auto u : goalUnits[playerID]){
				Logger::LogAppendToFile(UAB_LOGFILE, "%s %d, ", u.first.getName().c_str(), u.second);
			}
			Logger::LogAppendToFile(UAB_LOGFILE, "\n");
		}
	}
	catch (const ChoicePoint &c){
		Logger::LogAppendToFile(UAB_LOGFILE, "Hit choice point at beginning\n");
		return;//if some script hit an internal choice point
	}

	BOSS::BuildOrder		buildOrder[2];

	for (int playerID = 0; playerID < 2; playerID++){
		try{
			BOSS::BuildOrderSearchGoal goal(_state[playerID].getRace());

			for (size_t i = 0; i < goalUnits[playerID].size(); ++i)
			{
				MetaType type = goalUnits[playerID][i].first;
				BOSS::ActionType actionType;
				if (type.isUnit())
				{
					actionType = type.unitType;
				}
				else if (type.isUpgrade())
				{
					actionType = type.upgradeType;
				}
				else if (type.isTech())
				{
					actionType = type.techType;
				}
				else{
					UAB_ASSERT(false, "Should have found a valid type here");
				}
				goal.setGoal(actionType, goalUnits[playerID][i].second);
			}
			//perform search
			BOSS::DFBB_BuildOrderSmartSearch smartSearch(_state[playerID].getRace());
			smartSearch.setGoal(goal);
			smartSearch.setState(_state[playerID]);
			smartSearch.setTimeLimit(100);
			smartSearch.search();


			//get build order from search
			if (smartSearch.getResults().solved){
				buildOrder[playerID] = smartSearch.getResults().buildOrder;
				BWAPI::Broodwar->printf("Build order found");
			}
			else{
				BOSS::NaiveBuildOrderSearch nbos(smartSearch.getParameters().initialState, smartSearch.getParameters().goal);
				buildOrder[playerID] = nbos.solve();
				BWAPI::Broodwar->printf("Build order not found, using naive");
			}

		}
		catch (BOSS::Assert::BOSSException &e){
			BWAPI::Broodwar->printf(e.what());
		}
	}


	//execute build orders
	int						buildOrderIndex[2] = { 0, 0 };

	//check build queues, redo script+BOSS if necessary
	//check scripts choices after enemy starts to build
	Logger::LogAppendToFile(UAB_LOGFILE, "Build order sizes: %d %d\n", buildOrder[0].size(), buildOrder[1].size());
	for (int playerID = 0; playerID < 2; playerID++){
		std::ofstream out(UAB_LOGFILE,std::ios::app);
		std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
		std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
		_state[playerID].toString();
		std::cout.rdbuf(coutbuf);
		//Logger::LogAppendToFile(UAB_LOGFILE, "Units in progress for player %d ", playerID);
		//for (int i = 0; i < _state[playerID].getUnitData().getNumActionsInProgress(); i++){
		//	Logger::LogAppendToFile(UAB_LOGFILE, " %s", _state[playerID].getUnitData().getActionInProgressByIndex(i).getName().c_str());
		//}
		//Logger::LogAppendToFile(UAB_LOGFILE, "\n");
	}
	while (buildOrderIndex[0] < buildOrder[0].size() &&
		buildOrderIndex[1] < buildOrder[1].size() ){
		int nextActionStart[2];
		for (int playerID = 0; playerID < 2; playerID++){
			nextActionStart[playerID] = _state[playerID].whenCanPerform(buildOrder[playerID][buildOrderIndex[playerID]]);
		}

		int nextPlayer = nextActionStart[0] < nextActionStart[1] ? 0 : 1;
		Logger::LogAppendToFile(UAB_LOGFILE, "Start new unit: %s to player %d frame %d\n", buildOrder[nextPlayer][buildOrderIndex[nextPlayer]].getName().c_str(), nextPlayer, _state[nextPlayer].getCurrentFrame());

		synchronizeNewUnits(nextPlayer, _state[nextPlayer].doAction(buildOrder[nextPlayer][buildOrderIndex[nextPlayer]++]));
		//doAction(nextPlayer, buildOrder[nextPlayer][buildOrderIndex[nextPlayer]++]);
		try{
			StrategyManager::Instance().getBuildOrderGoal(
				_unitData[1-nextPlayer],
				_unitData[nextPlayer],
				_workerData[1-nextPlayer],
				currentFrame(),
				moves[1-nextPlayer].getStrategy(),
				getRace(1-nextPlayer),
				moves[1-nextPlayer].getChoices());
		}
		catch (const ChoicePoint &c){
			Logger::LogAppendToFile(UAB_LOGFILE, "Hit choice point while forwarding 1\n");
			break;;//if some script hit an internal choice point
		}
		if (nextActionStart[0] == nextActionStart[1]){
			Logger::LogAppendToFile(UAB_LOGFILE, "Start new unit: %s to player %d frame %d\n", buildOrder[1 - nextPlayer][buildOrderIndex[1 - nextPlayer]].getName().c_str(), 1 - nextPlayer, _state[1-nextPlayer].getCurrentFrame());

			synchronizeNewUnits(1 - nextPlayer, _state[1 - nextPlayer].doAction(buildOrder[1 - nextPlayer][buildOrderIndex[1 - nextPlayer]++]));
			//doAction(1 - nextPlayer, buildOrder[1 - nextPlayer][buildOrderIndex[1 - nextPlayer]++]);
			try{
				StrategyManager::Instance().getBuildOrderGoal(
					_unitData[nextPlayer],
					_unitData[1 - nextPlayer],
					_workerData[nextPlayer],
					currentFrame(),
					moves[nextPlayer].getStrategy(),
					getRace(nextPlayer),
					moves[nextPlayer].getChoices());
			}
			catch (const ChoicePoint &c){
				Logger::LogAppendToFile(UAB_LOGFILE, "Hit choice point while forwarding 2\n");
				break;//if some script hit an internal choice point
			}
		}
		else{

			synchronizeNewUnits(1 - nextPlayer, _state[1 - nextPlayer].fastForward(nextActionStart[nextPlayer]));
		}
		//todo: if unit built is a combat unit, add it somewhere
		//todo: move units and resolve combat 
		synchronizeDeadUnits();
	}

	//make sure both states are at the same frame
	int frameDiff = _state[0].getCurrentFrame() - _state[1].getCurrentFrame();
	if (frameDiff<0){
		synchronizeNewUnits(0,_state[0].fastForward(_state[1].getCurrentFrame()));
	}
	else if (frameDiff > 0){
		synchronizeNewUnits(1,_state[1].fastForward(_state[0].getCurrentFrame()));
	}

	Logger::LogAppendToFile(UAB_LOGFILE, "Finished forwarding\n");
}

//void HLState::doAction(int playerID, const BOSS::ActionType action){
//	_state[playerID].doAction(action);
//	if (action.isUnit()){
//		_unitData[playerID].updateUnit();
//		if (action.isWorker()){
//			_workerData[playerID].addWorker();
//		}
//	}
//}

void HLState::synchronizeNewUnits(int playerID, const std::vector<BOSS::ActionType> &newUnits)
{
	for (auto unit : newUnits){
		if (unit.isUnit()){
			auto parentType = unit.getUnitType().whatBuilds().first;
			//BWAPI::Position pos;
			//for (auto temp : _unitData[playerID].getUnitVector()){//let's take the position of the first building that can build it 
			//	if (temp.type == parentType){
			//		pos = temp.lastPosition;
			//		break;
			//	}
			//}
			BWAPI::Position pos(_players[playerID]->getStartLocation());//everything spawns at the main base
			
			int newID = std::max(_unitData[playerID].highestID(), _unitData[1 - playerID].highestID()) + 1;
			UnitInfo newUnit(newID, _players[playerID], pos, unit.getUnitType(), true);
			
			_unitData[playerID].addUnit(newUnit);
			Logger::LogAppendToFile(UAB_LOGFILE, "Added new unit: %s to player %d frame %d\n", newUnit.type.getName().c_str(), playerID, _state[playerID].getCurrentFrame());
			if (unit.isWorker()){
				//todo:for now we'll ignore new workers 
				//_workerData[playerID].addWorker();
			}
		}

	}
	
}

void HLState::synchronizeDeadUnits()
{

}

std::vector<HLMove> HLState::getMoves(int playerID, int strategy, const std::unordered_map<short, short> &choices) const
{
	std::vector<HLMove> moves;
	try{
		auto goalUnits = StrategyManager::Instance().getBuildOrderGoal(
			_unitData[playerID],
			_unitData[1 - playerID],
			_workerData[playerID],
			_state[playerID].getCurrentFrame(),
			strategy,
			getRace(playerID),
			choices);
		moves.push_back(HLMove(strategy, choices));
	}
	catch (const ChoicePoint &c){
		for (int option = 0; option < c._options; option++){
			auto newChoices = choices;
			newChoices[c._point] = option;
			auto temp = getMoves(playerID, strategy, newChoices);
			moves.insert(moves.end(), temp.begin(), temp.end());
		}
	}
	return moves;
}
std::vector<HLMove> HLState::getMoves(int playerID) const
{
	std::vector<HLMove> moves;

	for (int s = 0; s < StrategyManager::getNumStrategies(getRace(playerID));s++){

		try{
			auto goalUnits = StrategyManager::Instance().getBuildOrderGoal(
				_unitData[playerID],
				_unitData[1 - playerID],
				_workerData[playerID],
				_state[playerID].getCurrentFrame(),
				s,//strategy
				getRace(playerID),
				std::unordered_map<short, short>());
			moves.push_back(HLMove(s));
		}
		catch (const ChoicePoint &c){
			for (int option = 0; option < c._options; option++){
				std::unordered_map<short, short> choices;
				choices[c._point] = option;
				auto temp = getMoves(playerID, s, choices);
				moves.insert(moves.end(), temp.begin(), temp.end());
			}
		}
	}

	return moves;

	////do nothing move
	//HLMove move(0);
	//for (const auto &s : _squad[playerID]){
	//	move.addSquad(s);
	//}
	//moves.push_back(move);

	////merge squads move
	//move= HLMove(0);
	//if (_squad[playerID].size() > 1){
	//	for (int i = 0; i < _squad[playerID].size()-1; i+=2){
	//		move.addSquad(merge(_squad[playerID][i], _squad[playerID][i + 1]));
	//	}
	//	if (_squad[playerID].size() % 2 == 1){
	//		move.addSquad(_squad[playerID].back());
	//	}
	//}
	//moves.push_back(move);

	////attack move, just attack first neighbouring region
	//move = HLMove(0);
	//for (auto s : _squad[playerID]){
	//	BWAPI::Position pos = (*s.getUnits().front()->getRegion()->getNeighbors().begin())->getCenter();
	//	int radius = 1000;
	//	s.setSquadOrder(SquadOrder(SquadOrder::Attack, pos, radius));
	//	move.addSquad(s);
	//}
	//moves.push_back(move);

	//return moves;
}

HLSquad HLState::merge(const HLSquad& s1, const HLSquad &s2)
{
	auto units = s1.getUnits();
	auto units2 = s2.getUnits();
	units.insert(units.end(), units2.begin(),units2.end());
	return HLSquad(units, s1.getSquadOrder());
}

int HLState::evaluate(int playerID) const
{
	CombatSimulation sim;
	SparCraft::GameState state;

	for (auto unit : _unitData[playerID].getUnits()){
		if (InformationManager::Instance().isCombatUnit(unit.second.type) 
			&& SparCraft::System::isSupportedUnitType(unit.second.type))
		{
			try
			{
				state.addUnit(sim.getSparCraftUnit(unit.second));
			}
			catch (int e)
			{
				e = 1;
				BWAPI::Broodwar->printf("Problem Adding Self Unit with ID: %d", unit.second.unitID);
			}
		}
	}

	for (auto unit : _unitData[1 - playerID].getUnits()){
		if (!unit.second.type.isFlyer() && SparCraft::System::isSupportedUnitType(unit.second.type) && unit.second.completed)
		{
			try
			{
				state.addUnit(sim.getSparCraftUnit(unit.second));
			}
			catch (int e)
			{
				BWAPI::Broodwar->printf("Problem Adding Enemy Unit with ID: %d %d", unit.second.unitID, e);
			}
		}
	}

	

	state.finishedMoving();

	try
	{
		
		SparCraft::PlayerPtr selfNOK(new SparCraft::Player_NOKDPS(
			playerID == BWAPI::Broodwar->self()->getID() ? SparCraft::Players::Player_One : SparCraft::Players::Player_Two));

		SparCraft::PlayerPtr enemyNOK(new SparCraft::Player_NOKDPS(
			playerID == BWAPI::Broodwar->enemy()->getID() ? SparCraft::Players::Player_One : SparCraft::Players::Player_Two));

		SparCraft::Game g(state, selfNOK, enemyNOK, 2000);

		g.play();

		SparCraft::ScoreType eval = g.getState().eval(
			playerID == BWAPI::Broodwar->self()->getID() ? SparCraft::Players::Player_One : SparCraft::Players::Player_Two
			, SparCraft::EvaluationMethods::LTD2).val();

		return eval;
	}
	catch (int e)
	{
		BWAPI::Broodwar->printf("SparCraft FatalError, simulateCombat() threw");

		UAB_ASSERT(false, "SparCraft FatalError, simulateCombat() threw");
	}
}

bool HLState::gameOver() const
{
	return false;
}


HLMove::HLMove(int strategy, const std::vector<HLSquad> &squads) :_strategy(strategy), _squads(squads)
{

}

HLMove::HLMove(int strategy, const std::unordered_map<short, short> &choices) :_strategy(strategy), _choices(choices)
{

}

HLMove::HLMove(int strategy) :_strategy(strategy)
{

}
HLMove::HLMove() : _strategy(-1)
{

}
void HLMove::addSquad(const HLSquad &squad)
{
	_squads.push_back(squad);
}

std::string HLMove::toString() const
{
	std::ostringstream ss;
	ss << "[strat:"<<_strategy<<", ";
	for (auto c : _choices){
		ss << "[" << c.first << ", " << c.second << "] ";
	}
	ss << "]";
	return ss.str();
}

int HLState::currentFrame() const
{
	return std::min(_state[0].getCurrentFrame(), _state[1].getCurrentFrame());
}