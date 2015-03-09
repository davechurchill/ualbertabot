#pragma once
#include "Common.h"
#include "../../BOSS/source/BOSS.h"
#include "Squad.h"
#include "HLUnitData.h"

namespace UAlbertaBot{

	class HLSquad : public Squad{
		BWAPI::Region region;
	public:
		HLSquad(const std::vector<BWAPI::UnitInterface *> & units, SquadOrder order);
		HLSquad() :Squad() {}
		~HLSquad() {}
		HLSquad(const Squad &squad);//implicit conversion
		bool done();//check if order completed
		BWAPI::Region getRegion() const{ return region; }
		void addUnit(BWAPI::UnitInterface *u){ Squad::addUnit(u); }
		bool removeUnit(BWAPI::UnitInterface *u){ return Squad::removeUnit(u); }
	};


	class HLMove{
		int _strategy;				//build order
		std::unordered_map<short, short> _choices;
		std::vector<HLSquad> _squads;	//units and their orders
		//int _frames;					//length of the move
	public:
		HLMove(int strategy, const std::vector<HLSquad> &squads);
		HLMove(int strategy, const std::unordered_map<short, short> &choices);
		HLMove(int strategy);
		HLMove();
		void addSquad(const HLSquad &squad);
		const std::vector<HLSquad>& getSquads() const{
			return _squads;
		};
		const std::unordered_map<short, short>& getChoices() const{
			return _choices;
		};
		int getStrategy() const{
			return _strategy;
		};
		bool isEmpty() const{ return _strategy == -1; };
		std::string toString() const;
	};


	class HLState
	{

		BOSS::GameState			_state[2];		//includes all we need for the economic part, includes frame
		std::vector<HLSquad>	_squad[2]; //combat units
		BWAPI::Player			_players[2];

		HLUnitData				_unitData[2];
		WorkerData				_workerData[2];
		//BOSS::BuildOrder		_buildOrder[2];
		//int						_buildOrderIndex[2];

		void addSquads(const BWAPI::GameWrapper & game, int playerID);
		static HLSquad merge(const HLSquad& s1, const HLSquad &s2);//merge s2 into s1, keeps order of s1
		//void forwardGameState(int frames, int playerID);
		void synchronizeNewUnits(int playerID, const std::vector<BOSS::ActionType> &newUnits);
		void synchronizeDeadUnits();
	public:
		HLState(BWAPI::GameWrapper & game, BWAPI::PlayerInterface * player, BWAPI::PlayerInterface * enemy);
		~HLState();

		std::vector<HLState> getChildren() const;
		std::vector<HLMove> getMoves(int playerID) const;
		std::vector<HLMove> getMoves(int playerID, int strategy, const std::unordered_map<short, short> &choices) const;
		void applyAndForward(const std::array<HLMove, 2> &moves);
		int evaluate(int playerID) const;
		bool gameOver() const;
		friend class HLSearch;
		int currentFrame() const;
		BWAPI::Race getRace(int playerID) const{
			return BOSS::Races::GetRace(_state[playerID].getRace());
		};
		//bool undecidedChoicePoint(int playerID, int strategy, const std::unordered_map<short, short> choices) const;
		//void HLState::doAction(int playerID, const BOSS::ActionType action);
	};

	class HLNode{
		HLState _state;
		HLNode *_children;
	};
}