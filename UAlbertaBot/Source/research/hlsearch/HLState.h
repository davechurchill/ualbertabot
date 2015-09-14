#pragma once
#include "Common.h"
#include "../../BOSS/source/BOSS.h"
#include "Squad.h"
#include "HLUnitData.h"
#include "HLSquad.h"
#include <random>
#include <array>

namespace UAlbertaBot{

	class HLMove{
		int _strategy;				//build order
		std::unordered_map<short, short> _choices;
		//std::vector<HLSquad> _squads;	//units and their orders
		//int _frames;					//length of the move
	public:
		//HLMove(int strategy, const std::vector<HLSquad> &squads);
		HLMove(int strategy, const std::unordered_map<short, short> &choices);
		HLMove(int strategy);
		HLMove();
		//void addSquad(const HLSquad &squad);
		//const std::vector<HLSquad>& getSquads() const{
		//	return _squads;
		//};
		const std::unordered_map<short, short>& getChoices() const{
			return _choices;
		};
		int getStrategy() const{
			return _strategy;
		};
		bool isEmpty() const{ return _strategy == -1; };
		std::string toString() const;
		bool operator==(const HLMove &m) const
		{
			return _strategy == m._strategy && _choices == m._choices;
		}
		//HLMove& operator=(const HLMove &m)
		//{
		//	_strategy = m._strategy;
		//	_choices = m._choices;

		//	return *this;
		//}
	};


	class HLState
	{

		BOSS::GameState			_state[2];		//includes all we need for the economic part, includes frame
		std::vector<HLSquad>	_squad[2]; //combat units
		BWAPI::Player			_players[2];

		HLUnitData				_unitData[2];
		WorkerData				_workerData[2];



		static unsigned int				_zobristChoice[20][StrategyManager::NumProtossStrategies][20][10];//10 depth, 5 strategies, 20 choice points, 10 choices each
		static unsigned int				_zobristStrategy[20][StrategyManager::NumProtossStrategies];

		unsigned int						_hash;
		//bool					_opening[2];
		//BOSS::BuildOrder		_buildOrder[2];
		//int						_buildOrderIndex[2];
		int lastSquadUpdate=0;
		void addSquads(const BWAPI::PlayerInterface *player);
		//static HLSquad merge(const HLSquad& s1, const HLSquad &s2);//merge s2 into s1, keeps order of s1
		//void forwardGameState(int frames, int playerID);
		void synchronizeNewUnits(int playerID, const std::vector<BOSS::ActionType> &newUnits);
		void synchronizeNewUnits(int playerID, const std::vector<BOSS::ActionType> &newUnits, const BOSS::ActionType &startedUnit);
		void synchronizeDeadUnits(const std::array<std::vector<UnitInfo>, 2> &units);
		BOSS::BuildOrder getBuildOrder(const HLMove &move, int playerID) const;
		bool checkChoicePoint(const HLMove &move, int playerID) const;
		void forwardSquads(int frames);
		void assignDefenseSquads();
		void assignAttackSquads();
		void forwardCombat(const std::array<std::vector<int>, 2 > &squads, int frames);
		BWTA::BaseLocation * getClosestBaseLocation(int playerId, BWTA::Region *r) const;
		int getClosestUnassignedSquad(int playerId, const BWTA::Region *r, const std::vector<bool> skip);
		HLSquad & getUnassignedSquad(int playerId);
		void clearOrders();
		std::vector<std::array<std::vector<int>, 2 > > getCombats() const;
		void addNewUnitToSquad(const UnitInfo &newUnit);
		std::vector<HLMove> getMoves(int playerID, int strategy, const std::unordered_map<short, short> &choices) const;

	public:
		HLState(){}
		HLState(BWAPI::GameWrapper & game, BWAPI::PlayerInterface * player, BWAPI::PlayerInterface * enemy);
		~HLState();

		std::vector<HLState> getChildren() const;
		std::vector<HLMove> getMoves(int playerID) const;
		void applyAndForward(int depth, int frames, const std::array<HLMove, 2> &moves);
		int evaluate(int playerID) const;
		bool gameOver() const;
		friend class HLSearch;
		int currentFrame() const;
		BWAPI::Race getRace(int playerID) const{
			return BOSS::Races::GetRace(_state[playerID].getRace());
		};
		//bool undecidedChoicePoint(int playerID, int strategy, const std::unordered_map<short, short> choices) const;
		//void HLState::doAction(int playerID, const BOSS::ActionType action);
		static bool HLState::isNonWorkerCombatUnit(const BWAPI::UnitInterface *unit);
		static bool HLState::isNonWorkerCombatUnit(const UnitInfo &unit);
		unsigned int getHash(int depth, const std::array < HLMove, 2 >  &moves) const
		{
			unsigned int hash = _hash;
			for (int p = 0; p < 2; p++)
			{
				hash ^= _zobristStrategy[depth-p][moves.at(p).getStrategy()];
				for (auto c : moves.at(p).getChoices())
				{
					hash ^= _zobristChoice[depth - p][moves.at(p).getStrategy()][c.first][c.second];
				}
			}
			return hash;
		}
		unsigned int getHash(int depth, const HLMove  &move) const
		{
			unsigned int hash = _hash;
			hash ^= _zobristStrategy[depth][move.getStrategy()];
			for (auto c : move.getChoices())
			{
				hash ^= _zobristChoice[depth][move.getStrategy()][c.first][c.second];
			}

			return hash;
		}
		unsigned int getHash() const { return _hash; }
	//	friend struct _dummy_static_initializer;
	};

	//struct _dummy_static_initializer{
	//	_dummy_static_initializer(){
	//		std::mt19937 rng(1);//todo:use a seed?
	//		for (int depth = 0; depth < 10; depth++){
	//			for (int s = 0; s < 5; s++){
	//				HLState::_zobristStrategy[depth][s] = rng();
	//				Logger::LogAppendToFile(UAB_LOGFILE, "\n");
	//				for (int point = 0; point < 20; point++){
	//					for (auto c = 0; c < 10; c++){
	//						HLState::_zobristChoice[depth][s][point][c] = rng();
	//						Logger::LogAppendToFile(UAB_LOGFILE, "%u ", HLState::_zobristChoice[depth][s][point][c]);
	//					}
	//				}
	//			}
	//		}
	//	}
	//};

	

}