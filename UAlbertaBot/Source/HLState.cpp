#include "HLState.h"

using namespace UAlbertaBot;


HLState::HLState(BWAPI::GameWrapper & game, BWAPI::PlayerInterface * player, BWAPI::PlayerInterface * enemy)
{

	UAB_ASSERT(player->getID() < 2, "Only player ids 0 and 1 supported");
	UAB_ASSERT(enemy->getID() < 2, "Only player ids 0 and 1 supported");
	_state[player->getID()] = BOSS::GameState(game, player);
	_state[enemy->getID()] = BOSS::GameState(game, enemy);
	_players[player->getID()] = player;
	_players[enemy->getID()] = enemy;

	addSquads(player);
	addSquads(enemy);

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

void HLState::addSquads(const BWAPI::PlayerInterface *player)
{
	std::unordered_map < BWTA::Region*, std::vector<UnitInfo> > units;
	for (auto unit : player->getUnits()){
		if (unit
			&&unit->isCompleted()
			&& unit->getHitPoints() > 0
			&& unit->exists()
			&& unit->getType() != BWAPI::UnitTypes::Unknown
			&& unit->getPosition().x != BWAPI::Positions::Unknown.x
			&& unit->getPosition().y != BWAPI::Positions::Unknown.y
			&& (unit->getType().canAttack() ||
			unit->getType() == BWAPI::UnitTypes::Terran_Medic ||
			unit->getType() == BWAPI::UnitTypes::Protoss_High_Templar ||
			unit->getType() == BWAPI::UnitTypes::Protoss_Observer))
		{
			units[BWTA::getRegion(unit->getPosition())].push_back(UnitInfo(unit));
		}
	}
	for (auto it : units)
	{
		_squad[player->getID()].push_back(HLSquad(it.second, it.first));
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

bool HLState::checkChoicePoint(const HLMove &move, int playerID) const
{
	try{
		StrategyManager::Instance().getBuildOrderGoal(
			_unitData[playerID],
			_unitData[1 - playerID],
			_workerData[playerID],
			currentFrame(),
			move.getStrategy(),
			getRace(playerID),
			move.getChoices());
	}
	catch (const ChoicePoint &){
		return true;//if some script hit an internal choice point
	}
	return false;
}

BOSS::BuildOrder HLState::getBuildOrder(const HLMove &move, int playerID) const
{
	MetaPairVector goalUnits;
	try{
		goalUnits = StrategyManager::Instance().getBuildOrderGoal(
			_unitData[playerID],
			_unitData[1 - playerID],
			_workerData[playerID],
			currentFrame(),
			move.getStrategy(),
			getRace(playerID),
			move.getChoices());
	}

	catch (const ChoicePoint &){
		Logger::LogAppendToFile(UAB_LOGFILE, "Hit choice point at beginning\n");
		return BOSS::BuildOrder();//if some script hit an internal choice point
	}

	BOSS::BuildOrder		buildOrder;

	BOSS::BuildOrderSearchGoal goal(_state[playerID].getRace());

	for (size_t i = 0; i < goalUnits.size(); ++i)
	{
		MetaType type = goalUnits[i].first;
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
		goal.setGoal(actionType, goalUnits[i].second);
	}

	BOSS::NaiveBuildOrderSearch nbos(_state[playerID], goal);
	buildOrder = nbos.solve();

	return buildOrder;
}



void HLState::applyAndForward(const std::array<HLMove, 2> &moves)
{
	UAB_ASSERT(getRace(0) == BWAPI::Races::Protoss, "Non protoss?");
	UAB_ASSERT(getRace(1) == BWAPI::Races::Protoss, "Non protoss?");


	BOSS::BuildOrder buildOrder[2] = { getBuildOrder(moves[0], 0), getBuildOrder(moves[1], 1) };

	//execute build orders
	unsigned int buildOrderIndex[2] = { 0, 0 };

	while (buildOrderIndex[0] < buildOrder[0].size() &&
		buildOrderIndex[1] < buildOrder[1].size()){
		int nextActionStart[2];
		for (int playerID = 0; playerID < 2; playerID++){
			nextActionStart[playerID] = _state[playerID].whenCanPerform(buildOrder[playerID][buildOrderIndex[playerID]]);
		}

		
		int nextPlayer = nextActionStart[0] < nextActionStart[1] ? 0 : 1;
		int framesToForward = nextActionStart[nextPlayer] - _state[nextPlayer].getCurrentFrame();
		const BOSS::ActionType& action = buildOrder[nextPlayer][buildOrderIndex[nextPlayer]++];
		//Logger::LogAppendToFile(UAB_LOGFILE, "Start new unit: %s to player %d frame %d\n", action.getName().c_str(), nextPlayer, _state[nextPlayer].getCurrentFrame());


		synchronizeNewUnits(nextPlayer, _state[nextPlayer].doAction(action), action);
		if (checkChoicePoint(moves[1 - nextPlayer], 1 - nextPlayer))
		{
			//Logger::LogAppendToFile(UAB_LOGFILE, "Hit choice point while forwarding 1\n");
			break;;//if some script hit an internal choice point
		}

		if (nextActionStart[0] == nextActionStart[1]){
			const BOSS::ActionType& action = buildOrder[1 - nextPlayer][buildOrderIndex[1 - nextPlayer]++];
			//Logger::LogAppendToFile(UAB_LOGFILE, "Start new unit: %s to player %d frame %d\n", action.getName().c_str(), 1 - nextPlayer, _state[1 - nextPlayer].getCurrentFrame());

			synchronizeNewUnits(1 - nextPlayer, _state[1 - nextPlayer].doAction(action), action);
			if (checkChoicePoint(moves[nextPlayer], nextPlayer))
			{
				//Logger::LogAppendToFile(UAB_LOGFILE, "Hit choice point while forwarding 1\n");
				break;;//if some script hit an internal choice point
			}
		}
		else{

			synchronizeNewUnits(1 - nextPlayer, _state[1 - nextPlayer].fastForward(nextActionStart[nextPlayer]));
		}
		//todo: if unit built is a combat unit, add it somewhere
		//todo: move units and resolve combat 
		forwardSquads(framesToForward);
		synchronizeDeadUnits();
	}

	//make sure both states are at the same frame
	int frameDiff = _state[0].getCurrentFrame() - _state[1].getCurrentFrame();
	if (frameDiff < 0){
		synchronizeNewUnits(0, _state[0].fastForward(_state[1].getCurrentFrame()));
	}
	else if (frameDiff > 0){
		synchronizeNewUnits(1, _state[1].fastForward(_state[0].getCurrentFrame()));
	}

	//Logger::LogAppendToFile(UAB_LOGFILE, "Finished forwarding\n");
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
void HLState::synchronizeNewUnits(int playerID, const std::vector<BOSS::ActionType> &finishedUnits)
{
	for (auto unit : finishedUnits){
		if (unit.isUnit()){
			if ((_unitData[playerID].getNumUnits(unit.getUnitType()) ==
				_unitData[playerID].getNumCompletedUnits(unit.getUnitType())) ||
				!_unitData[playerID].finishUnit(unit.getUnitType()))//units of this type were in progress, let's finish them
			{//let's add a new finished unit

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
				//Logger::LogAppendToFile(UAB_LOGFILE, "Added new unit: %s to player %d frame %d\n", newUnit.type.getName().c_str(), playerID, _state[playerID].getCurrentFrame());
				if (unit.isWorker()){
					//todo:for now we'll ignore new workers 
					//_workerData[playerID].addWorker();
				}
			}
			else{
				//Logger::LogAppendToFile(UAB_LOGFILE, "Finished new unit: %s to player %d frame %d\n", unit.getName().c_str(), playerID, _state[playerID].getCurrentFrame());

			}
		}

	}
}

void HLState::synchronizeNewUnits(int playerID, const std::vector<BOSS::ActionType> &finishedUnits, const 	BOSS::ActionType &startedUnit)
{
	if (startedUnit.isUnit()){
		auto parentType = startedUnit.getUnitType().whatBuilds().first;
		BWAPI::Position pos(_players[playerID]->getStartLocation());//everything spawns at the main base

		int newID = std::max(_unitData[playerID].highestID(), _unitData[1 - playerID].highestID()) + 1;
		UnitInfo newUnit(newID, _players[playerID], pos, startedUnit.getUnitType(), false);

		_unitData[playerID].addUnit(newUnit);
		//Logger::LogAppendToFile(UAB_LOGFILE, "Started new unit: %s to player %d frame %d\n", newUnit.type.getName().c_str(), playerID, _state[playerID].getCurrentFrame());
		if (startedUnit.isWorker()){
			//todo:for now we'll ignore new workers 
			//_workerData[playerID].addWorker();
		}
	}

	synchronizeNewUnits(playerID, finishedUnits);
	
}

//void HLState::synchronizeDeadUnits()
//{
//
//}


void HLState::assignDefenseSquads()
{
	for (int playerId = 0; playerId < 2; playerId++)
	{
		//check if we need to defend any of our regions
		for ( BWTA::BaseLocation *l: getOccupiedBaseLocations(playerId))
		{
			BWTA::Region *r = l->getRegion();
			int enemyFlyingStrength = 0;
			int enemyGroundStrength = 0;
			for (auto s : _squad[1 - playerId])
			{
				if (s.getCurrentRegion() == r)
				{
					enemyFlyingStrength += s.getFlyingStrength();
					enemyGroundStrength += s.getGroundStrength();
				}
			}

			//assign closest squads to region to defend until we outnumber enemies
			while ((enemyFlyingStrength > 0) || (enemyGroundStrength > 0))
			{
				try
				{
					HLSquad &s = getClosestUnassignedSquad(playerId,r);

					if ((enemyFlyingStrength > 0) && (s.getAntiAirStrength() > 0))
					{
						s.order(HLSquadOrder(HLSquadOrder::Defend, r));
						enemyFlyingStrength -= s.getAntiAirStrength();
					}
					else if ((enemyGroundStrength > 0) && (s.getGroundStrength() > 0))
					{
						s.order(HLSquadOrder(HLSquadOrder::Defend, r));
						enemyGroundStrength -= s.getGroundStrength();
					}
				}
				catch (...)
				{
					break;
				}
			}
		}
	}
	
}
void HLState::assignAttackSquads()
{
	for (int playerId = 0; playerId < 2; playerId++)
	{
		//if there are unassigned squads, send them to the closest enemy region
		try
		{
			BWTA::Region *r = getClosestBaseLocation(1 - playerId, BWTA::getStartLocation(_players[playerId])->getRegion())->getRegion();
			while (1)
			{
				HLSquad &s = getUnassignedSquad(playerId);
				s.order(HLSquadOrder(HLSquadOrder::OrderType::Attack, r));
			}
		}
		catch (...)
		{

		}
	}
}

//std::vector<std::pair<std::vector<int>, std::vector<int> > > HLState::getCombats() const
//{
////	return std::vector<std::pair<std::vector<int>, std::vector<int> > >();
//}
//
//
//void HLState::forwardCombat(const std::vector<int> &squads1, const std::vector<int> &squads2, int frames)
//{
//
//}

BWTA::BaseLocation * HLState::getClosestBaseLocation(int playerId, BWTA::Region *r) const
{
	int min = std::numeric_limits<int>::max();
	BWTA::BaseLocation *min_base = NULL;
	for (auto l : BWTA::getBaseLocations())
	{
		auto d = l->getRegion()->getCenter().getApproxDistance(r->getCenter());
		if (d<min)
		{
			min = d;
			min_base = l;
		}
	}
	return min_base;
}
HLSquad & HLState::getClosestUnassignedSquad(int playerId, const BWTA::Region *r)
{
	int min = std::numeric_limits<int>::max(), min_i = -1;
	for (int i = 0; i < _squad[playerId].size(); i++)
	{
		auto s = _squad[playerId][i];
		if ((s.order()._type == HLSquadOrder::None))
		{
			auto d = s.getCurrentRegion()->getCenter().getApproxDistance(r->getCenter());
			if (d < min)
			{
				min = d;
				min_i = i;
			}
		}
	}
	return _squad[playerId][min_i];
}
HLSquad & HLState::getUnassignedSquad(int playerId) const
{
	for (auto s : _squad[playerId])
	{
		if (s.order()._type == HLSquadOrder::None)
		{
			return s;
		}
	}
}
void HLState::clearOrders()
{
	for (int playerId = 0; playerId < 2; playerId++)
	{
		for (auto s : _squad[playerId])
		{
			s.order(HLSquadOrder());
		}
	}
}

void HLState::forwardSquads(int frames)
{
	//assign orders to squads
	if ((currentFrame()-lastSquadUpdate)>100)
	{
		lastSquadUpdate = currentFrame();
		clearOrders();
		assignDefenseSquads();
		assignAttackSquads();
	}
		
	std::vector<bool> doneSquads[2] = { std::vector<bool>(_squad[0].size(),false), 
		std::vector<bool>(_squad[1].size(),false) };

	//execute orders
		//if enemy in same region or path
			//do combat
	for (auto combat : getCombats()){
		forwardCombat(combat.first, combat.second, frames);
		for (int s : combat.first)
		{
			doneSquads[0][s] = true;
		}
	}

		//move towards destination
	for (int playerId = 0; playerId < 2; playerId++)
	{
		for (int s = 0; s < doneSquads[playerId].size(); s++)
		{
			if (!doneSquads[playerId][s])
			{
				_squad[playerId][s].travel(frames);
			}
		}
	}
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

//HLSquad HLState::merge(const HLSquad& s1, const HLSquad &s2)
//{
//	auto units = s1.getUnits();
//	auto units2 = s2.getUnits();
//	units.insert(units.end(), units2.begin(),units2.end());
//	return HLSquad(units, s1.getSquadOrder());
//}

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

		//g.play();

		SparCraft::ScoreType eval = g.getState().eval(
			playerID == BWAPI::Broodwar->self()->getID() ? SparCraft::Players::Player_One : SparCraft::Players::Player_Two
			, SparCraft::EvaluationMethods::LTD2).val();

		return eval;
	}
	catch (int)
	{
		BWAPI::Broodwar->printf("SparCraft FatalError, simulateCombat() threw");

		UAB_ASSERT(false, "SparCraft FatalError, simulateCombat() threw");
	}
}

bool HLState::gameOver() const
{
	return false;
}


//HLMove::HLMove(int strategy, const std::vector<HLSquad> &squads) :_strategy(strategy), _squads(squads)
//{
//
//}

HLMove::HLMove(int strategy, const std::unordered_map<short, short> &choices) :_strategy(strategy), _choices(choices)
{

}

HLMove::HLMove(int strategy) :_strategy(strategy)
{

}
HLMove::HLMove() : _strategy(-1)
{

}
//void HLMove::addSquad(const HLSquad &squad)
//{
//	_squads.push_back(squad);
//}

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