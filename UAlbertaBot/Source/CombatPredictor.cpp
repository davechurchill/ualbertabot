#include "CombatPredictor.h"

using namespace UAlbertaBot;


Combat::Combat(std::vector<BWAPI::UnitInterface*> & ou, std::vector<UnitInfo> &eu, bool meOBS, bool oppOBS)
{
    ourCombatUnits.clear(); enemyCombatUnits.clear();
	HP.clear(); combatTime.clear(); unitTypes.clear(); unitIDS.clear();
    std::vector<int> startHp;
	forceFinish = false;
	I_have_Observers = meOBS;
	bothInvisUnits = false;

	if (ou.size() < 1 || eu.size() < 1) // NOT VALID
	{
		finished = true;
		return;
	}

    for (auto u : ou)
    {
		int t = u->getType();
		int id = u->getID();
		int hp = (u->getHitPoints() + u->getShields());
        ourCombatUnits.push_back(u);
        startHp.push_back(u->getHitPoints() + u->getShields());

		//special case for DTs, 61 - visible, 74 - invisible
		if (t == 61 && !oppOBS)
		{
			unitTypes.push_back(t + 13); 
			bothInvisUnits = true;
		}
		else 
			unitTypes.push_back(t);

		unitIDS.push_back(id);
    }

	unitTypes.push_back(-1);
	bool dtFlag = false;

	for (auto u : eu)
	{
		enemyCombatUnits.push_back(u);

		//special case for DTs
		if (u.type == 61 && !meOBS)
		{
			int hp_last = u.lastHealth;
			if (hp_last < 1) //if I saw it before keep the hp otherwise max hp for DT
				hp_last = 120;
			startHp.push_back(hp_last);
			unitTypes.push_back(u.type+13);
			dtFlag = true;
		}
		else
		{
			startHp.push_back(u.lastHealth);
			unitTypes.push_back(u.type);
		}
		unitIDS.push_back(u.unitID);
    }

    HP.push_back(startHp);
    combatStartTime = BWAPI::Broodwar->getFrameCount();
    combatTime.push_back(0);
    finished = false;
	if (!dtFlag) bothInvisUnits = false;
    ID = CombatPredictor::Instance().combatID;
    CombatPredictor::Instance().combatID += 1;
}

bool Combat::isOneSideDead()
{
	bool meDead = true;
	for (auto u : ourCombatUnits)
	{
		if (u->getHitPoints() + u->getShields() > 0)
		{
			meDead = false;
			break;
		}
	}
	if (meDead) return true;

	bool himDead = true;
	for (auto u : enemyCombatUnits)
	{
		int hp = CombatPredictor::Instance().observedHPs[u.unitID];
		//if (search != temp.end() && search->second.lastHealth > 0)
		if (hp > 0)
		{	
			himDead = false;
			break;
		}
	}

	return himDead;
}

void Combat::update()
{ 

    int const elapsedTime = BWAPI::Broodwar->getFrameCount() - combatStartTime;
    bool process = false;

	//need to check if one of the players
	//has no units left 
	if (isOneSideDead())
	{
		//need to finish battle and log it off!
		process = true;
		forceFinish = true;
	}

    if (process || forceFinish)
    {
        //save new hp values
        combatTime.push_back(elapsedTime);
        std::vector<int> startHp;

        //for me, already have pointers
        for (auto u : ourCombatUnits)
        {
            startHp.push_back(u->getHitPoints() + u->getShields());
        }

        //for enemy, need to look in information manager    
        for (auto u : enemyCombatUnits)
        {	
			int hp = CombatPredictor::Instance().observedHPs[u.unitID];

			//special case for invisible DTs
			if (u.type == BWAPI::UnitTypes::Protoss_Dark_Templar && !I_have_Observers)
			{
				if (hp < 1) //if I saw it before keep the hp otherwise max hp for DT
					hp = 120;
			}

			startHp.push_back(hp);
        }
        HP.push_back(startHp);
    } 
    
	if ((elapsedTime > 801) || forceFinish  ) //finish fight, flush to disk
    {
		std::ostringstream oss;
		int id = ID;  //CombatPredictor::Instance().combatID;

		//I don't want extremely short fights ...
		int total_startHP = std::accumulate(HP[0].begin(), HP[0].end(), 0);
		int total_endHP = std::accumulate(HP.back().begin(), HP.back().end(), 0);

		//if either 100hp damage total or 20% of inital hp
		int difHP = std::abs(total_endHP - total_startHP);

		if ((difHP < 100 && difHP*5 < total_startHP )|| bothInvisUnits)
		{
			//SHORT battle, don't log
			oss << defWriteFolder << "s_battle_" << CombatPredictor::Instance().getSuffix() << "_" << id << ".txt";
			writeToFile(oss.str(),false);
		}
		else
		{
			//PROPER battle
			oss << defWriteFolder << "battle_" << CombatPredictor::Instance().getSuffix() << "_" << id << ".txt";
			writeToFile(oss.str(),true);
		}
        finished = true;
    }
}

void CombatPredictor::addCombat(Combat &c)
{
	observedIDs;
	bool reinforcements = false;
	for (auto id : c.unitIDS)
	{
		UAB_ASSERT(id < 2000, "Too many units !!!");

		if (!observedIDs[id]) //not observed, need to end previous battles  - REINFORCEMENTS
		{
			reinforcements = true;
			observedIDs[id] = 1;
			//break;
		}
	}

	if (!reinforcements)
		//add combat
		//combats.push_back(c);
		// OR MAYBE NOT ADD COMBAT, IRRELEVANT? 
		;
	else
	{	//need to finish all previous combats
		for (unsigned int i = 0; i < combats.size(); i++)
		{
			if (!combats[i].isFinished())
			{
				//force update, and finish
				combats[i].forceFinish = true; 
				combats[i].update();
			}
		}

		//then add new combat
		combats.push_back(c);

	}
}

void Combat::writeToFile(std::string filename, bool valid4Train)
{
	//this is the debug file
    std::ofstream logStream(filename);

	logStream << "Frame: " << combatStartTime << std::endl;
	
	logStream << "Unit Types: ";
	int otherPlayerIDX = 0, j = 0;

	for (int i : unitTypes)
	{
		if (i < 0)
		{
			logStream << " | ";
			otherPlayerIDX = j;
		}
		else
			logStream << i << " ";
		j++;
	}
	logStream << std::endl;
	logStream << "  Unit IDs: ";

	j = 0;
	for (int i : unitIDS)
	{
		if (j == otherPlayerIDX)
			logStream << " | " << i << " ";
		else
			logStream << i << " ";
		j++;
	}
	logStream << std::endl;
	logStream << "-------------------------------" << std::endl;
    //write frame| HPs
    int i = 0;

    for (auto h : HP)
    {
        logStream << combatTime[i++] << " | ";
        for (auto u : h)
            logStream << u << " ";
        logStream << std::endl;
    }

    logStream.flush();
    logStream.close();

	if (valid4Train)
	{
		//this is the MATLAB training file (accumulator)
		std::ofstream trainStream(defWriteFolder + "train_" + CombatPredictor::Instance().getSuffix() + ".txt", std::ofstream::app);

		int counter = 0;
		trainStream << 0 << " " << combatTime.back() << " ";
		counter += 2;

		//write initial conditions
		bool p1 = true;
		for (unsigned int i = 0; i < unitTypes.size(); i++)
		{
			if (unitTypes[i] < 0) //switch players
			{
				p1 = false;
			}
			else
			{
				if (p1) trainStream << unitTypes[i] << " " << (HP[0])[i] << " ";
				if (!p1) trainStream << unitTypes[i] + MAX_UNIT_TYPES << " " << (HP[0])[i - 1] << " ";
				counter += 2;
			}
		}

		//need to add -1s to keep length to 150, easier for matlab to read
		while (counter++ < 150)
			trainStream << -1 << " ";
		trainStream << std::endl;


		//write result
		counter = 0;
		trainStream << 0 << " " << combatTime.back() << " ";
		counter += 2;

		p1 = true;
		for (unsigned int i = 0; i < unitTypes.size(); i++)
		{
			if (unitTypes[i] < 0) //switch players
			{
				p1 = false;
			}
			else
			{
				if (p1) trainStream << unitTypes[i] << " " << (HP.back())[i] << " ";
				if (!p1) trainStream << unitTypes[i] + MAX_UNIT_TYPES << " " << (HP.back())[i - 1] << " ";
				counter += 2;
			}
		}

		//need to add -1s to keep length to 150, easier for matlab to read
		while (counter++ < 150)
			trainStream << -1 << " ";
		trainStream << std::endl;

		trainStream.flush();
		trainStream.close();

		//write down results to compare % acccuracy
		std::ofstream resultStream(defWriteFolder + "results_" + CombatPredictor::Instance().getSuffix() + ".txt", std::ofstream::app);

		std::pair<float, float> ltd = LTD2();
		if (ltd.first > ltd.second *1.1) // P1 win
			resultStream << SparcraftPrediction << " " << MATLAB_Prediction << " " << 1 << " " << ltd.first / (ltd.second + 0.01) << std::endl;
		else if (ltd.second > ltd.first * 1.1) // P2 win
			resultStream << SparcraftPrediction << " " << MATLAB_Prediction << " " << -1 << " " << ltd.second / (ltd.first + 0.01) << std::endl;

		resultStream.flush();
		resultStream.close();
	}
}

std::pair<float, float> Combat::LTD2()
{
	bool p1 = true;
	float LTD_p1 = 0;
	float LTD_p2 = 0;

	float currentSum = 0;
	//float totalSum = 0;


	for (unsigned int i = 0; i < unitTypes.size(); i++)
	{
		if (unitTypes[i] < 0) //switch players
		{
			p1 = false;
			LTD_p1 = currentSum;
			currentSum = 0;
		}
		else
		{
			if (p1)
			{
				BWAPI::UnitType t = BWAPI::UnitType(unitTypes[i]);
				float dpf = CombatPredictor::Instance().dpf(t);

				currentSum += sqrtf((float)(HP.back())[i]) * dpf;
			}

			if (!p1)
			{
				BWAPI::UnitType t = BWAPI::UnitType(unitTypes[i]);
				float dpf = CombatPredictor::Instance().dpf(t);

				currentSum += sqrtf((float)(HP.back())[i-1]) * dpf;
			}
		}
	}

	LTD_p2 = currentSum;
	return std::pair<float, float>(LTD_p1, LTD_p2);
}


CombatPredictor::CombatPredictor() {

}

CombatPredictor & CombatPredictor::Instance()
{
    static CombatPredictor instance;
    return instance;
}

const float CombatPredictor::dpf(BWAPI::UnitType &ut)
{
    float damage = BWAPI::UnitTypes::Protoss_Zealot ?
        2 * (float)ut.groundWeapon().damageAmount() :
        (float)ut.groundWeapon().damageAmount();

    float attackCooldown = (float)ut.groundWeapon().damageCooldown();

    return (float)std::max(0.0f, damage / (attackCooldown + 1));
}



void CombatPredictor::initUnitList()
{
	std::srand((unsigned int)std::time(0));
	uniqSuffix = BWAPI::Broodwar->enemy()->getName() + "_" + std::to_string(rand() % 1000);
    
	auto set = BWAPI::UnitTypes::allUnitTypes();
    int len = set.size();

    //pre-allocate memory
    //no more than 256 different unit types in StarCraft
    MaxHP.resize(256, 0);
	observedIDs.resize(2000, 0);
	observedHPs.resize(2000, -1);

	std::ofstream logStream(defWriteFolder +"predictorInit.txt");

    for (auto type : set)
    {
        int maxHP = type.maxHitPoints() + type.maxShields();
        int ID = type.getID();
        logStream << ID << " " 
                  //<< type.getName() << " "
                  << maxHP << " "
                  << dpf(type)*maxHP << std::endl;

        //save the info
        MaxHP[ID] = maxHP;
    }

    logStream.flush();
    logStream.close();
    Sfeatures.clear();
    SfA.clear(); SfB.clear();

	//READ FEATURES
	std::string enemyFeatures = "f_" + BWAPI::Broodwar->enemy()->getName() + ".txt";
	std::string enemyFile(defReadFolder + enemyFeatures);
	std::ifstream features; // (defReadFolder + enemyFeatures);
	features.open(enemyFile);
	if (features.is_open() && features.good())
	{
		CombatPredictor::Instance().vsTrainedOpp = true;
	}
	else
	{
		//default features
		//std::ifstream features(defReadFolder + "features.txt");
		features.open(defReadFolder + "f_default.txt");
		UAB_ASSERT(features.is_open() && features.good(), "Couldn't open file %sf_default.txt", defReadFolder.c_str());
		CombatPredictor::Instance().vsTrainedOpp = false;
	}

	if (features.is_open() && features.good())
    {
        // read away
        std::string line;

        while (getline(features, line))
        {
            double d = std::stod(line);
            Sfeatures.push_back(d);
        }

        features.close();
        int nrS = Sfeatures.size() / 2;

        SfA = std::vector<double>(&Sfeatures[0], &Sfeatures[nrS - 1]);
        SfB = std::vector<double>(&Sfeatures[nrS], &Sfeatures[nrS * 2 - 1]);
    }
}

const SparCraft::ScoreType CombatPredictor::predictCombat(const HLUnitData &myUnits, const HLUnitData &oppUnits)
{

	bool I_haveOBS = false;
	bool He_hasOBS = false;

	int nrA = 0;
	int nrB = 0;
	double sumA = 0.0;
	double sumB = 0.0;

	for (auto &iter : oppUnits.getUnits())
	{
		const UnitInfo & ui(iter.second);
		if (ui.completed && ui.type.isDetector()) He_hasOBS = true;
	}

	for(auto &iter: myUnits.getUnits())
	{
		const UnitInfo & ui(iter.second);

		if (ui.completed && ui.type.isDetector()) I_haveOBS = true;

		if (ui.completed &&
			(ui.type.canAttack() || ui.type.isWorker()
			|| ui.type.isDetector()
			|| ui.type == BWAPI::UnitTypes::Terran_Medic
			|| ui.type == BWAPI::UnitTypes::Protoss_Reaver
			|| ui.type == BWAPI::UnitTypes::Terran_Bunker))
		{
			if (ui.type == BWAPI::UnitTypes::Protoss_Dark_Templar && !He_hasOBS) //invisible DT, unit ID 74
				sumA += SfA[74] * ui.lastHealth / MaxHP[74];
			else //visible DT or something else
				sumA += SfA[ui.type] * ui.lastHealth / MaxHP[ui.type];

			nrA += 1;
		}
	}

	for(auto &iter: oppUnits.getUnits())
	{
		const UnitInfo & ui(iter.second);

		if (ui.completed &&
			(ui.type.canAttack() || ui.type.isWorker()
			|| ui.type.isDetector()
			|| ui.type == BWAPI::UnitTypes::Terran_Medic
			|| ui.type == BWAPI::UnitTypes::Protoss_Reaver
			|| ui.type == BWAPI::UnitTypes::Terran_Bunker))
		{
			if (ui.type == BWAPI::UnitTypes::Protoss_Dark_Templar && !He_hasOBS) //invisible DT
			{
				//TODO: make sure ui.lastHealth is not 0 for invisible DTs
				UAB_ASSERT(ui.lastHealth > 0, "Invisible DT has HP zero ?!");
				sumB += SfB[74] * ui.lastHealth / MaxHP[74];
				
			}
			else //visible DT or something else
				sumB += SfB[ui.type] * ui.lastHealth / MaxHP[ui.type];

			nrB += 1;
		}
	}

	return (int)((pow(nrA, power - 1)*sumA - pow(nrB, power - 1)*sumB)*100.0);
}

SparCraft::ScoreType  CombatPredictor::predictCombat(std::vector<std::pair<int, int>> &ArmyA,
    std::vector<std::pair<int, int>> &ArmyB)
{
    int nrA = 0;
    int nrB = 0;
    double sumA = 0.0;
    double sumB = 0.0;

    for (auto p : ArmyA)
    {   //p.first = ID of unit
        //p.secont = current HP+shielcs of unit
        sumA += SfA[p.first] * p.second / MaxHP[p.first];
        nrA += 1;
    }

    for (auto p : ArmyB)
    {   //p.first = ID of unit
        //p.secont = current HP+shielcs of unit
        sumB += SfB[p.first] * p.second / MaxHP[p.first];
        nrB += 1;
    }

    return (int) ((pow(nrA, power - 1)*sumA - pow(nrB, power - 1)*sumB)*10.0);
    //convert to LTD2 ? for now it only matters if > 0 or < 0, so no need. 
}
