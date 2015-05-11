#pragma once

#include "Common.h"
#include "MapGrid.h"
#include "HLUnitData.h"

#ifdef USING_VISUALIZATION_LIBRARIES
#include "Visualizer.h"
#endif

#include "..\..\SparCraft\source\GameState.h"
#include "..\..\SparCraft\source\Game.h"
#include "..\..\SparCraft\source\Unit.h"
#include "..\..\SparCraft\source\AllPlayers.h"
#include "InformationManager.h"
#include <numeric>

namespace UAlbertaBot
{
	const int MAX_UNIT_TYPES = 234 ; //TODO: double check
	const std::string defWriteFolder = "bwapi-data/write/";
	const std::string defReadFolder = "bwapi-data/";
	
class Combat
{
    std::vector<BWAPI::UnitInterface*> ourCombatUnits;
    std::vector<UnitInfo> enemyCombatUnits;
	std::vector<int> unitTypes;

    std::vector<std::vector<int> > HP;
    int combatStartTime;
	std::vector<int> combatTime; 
    bool finished;
	bool bothInvisUnits;
    int ID;
	bool I_have_Observers; 

public:
	int SparcraftPrediction;
	int MATLAB_Prediction;
	bool  forceFinish;
	std::vector<int> unitIDS;
    Combat(std::vector<BWAPI::UnitInterface*> & ou, std::vector<UnitInfo> &eu, bool meOBS, bool oppOBS);
    void update();
    void writeToFile(std::string filename, bool valid4train);
    bool isFinished(){ return finished; }
	bool isOneSideDead();
	std::pair<float, float> LTD2();
};


class CombatPredictor
{
    double power = 1.52;
    
    std::vector<double> Sfeatures;
    std::vector<double> SfA;
    std::vector<double> SfB;
    std::vector<int> MaxHP;
	std::vector<int> observedIDs; 
	std::string uniqSuffix;

public:
	bool vsTrainedOpp = false;
    int combatID = 0;
    std::vector<Combat> combats;
	std::vector<int> observedHPs;
    CombatPredictor();
    static CombatPredictor & Instance();
    void initUnitList();
	std::string getSuffix(){ return uniqSuffix; };
    const float CombatPredictor::dpf(BWAPI::UnitType &ut);
	const SparCraft::ScoreType predictCombat(const HLUnitData &myUnits, const HLUnitData &oppUnits);

    //predict combat, for each army need {ID, current HP+shields} x each unit
    SparCraft::ScoreType  predictCombat(std::vector<std::pair<int,int>> &ArmyA,
                                        std::vector<std::pair<int,int>> &ArmyB);

    void updateFeatures(int afterHowManyNewBattles);
    void retrainMATLAB_model();
	void addCombat(Combat &c);
};
}