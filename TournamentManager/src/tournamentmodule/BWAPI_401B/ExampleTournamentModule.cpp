#include "ExampleTournamentModule.h"

using namespace BWAPI;

class TournamentModuleState
{

public:

	std::string selfName;
	std::string enemyName;
	std::string mapName;

	int frameCount;
	int selfScore;
	int enemyScore;
	int gameElapsedTime;
	int gameOver;
	int gameTimeUp;

	std::vector<int> timeOutExceeded;

	Timer gameTimer;

	TournamentModuleState()
	{
		selfName		= BWAPI::Broodwar->self()->getName();
		enemyName		= BWAPI::Broodwar->enemy()->getName();
		mapName			= BWAPI::Broodwar->mapName();

		frameCount		= BWAPI::Broodwar->getFrameCount();
		selfScore		= BWAPI::Broodwar->self()->getKillScore() 
						+ BWAPI::Broodwar->self()->getBuildingScore() 
						+ BWAPI::Broodwar->self()->getRazingScore() 
						+ BWAPI::Broodwar->self()->gatheredMinerals()
						+ BWAPI::Broodwar->self()->gatheredGas();

		enemyScore		= 0;

		gameOver = 0;
		gameTimeUp = 0;

		gameTimer.start();

		gameElapsedTime = 0;
	}

	void update(std::vector<int> & times)
	{
		frameCount		= BWAPI::Broodwar->getFrameCount();
		selfScore		= BWAPI::Broodwar->self()->getKillScore() 
						+ BWAPI::Broodwar->self()->getBuildingScore() 
						+ BWAPI::Broodwar->self()->getRazingScore() 
						+ BWAPI::Broodwar->self()->gatheredMinerals()
						+ BWAPI::Broodwar->self()->gatheredGas();

		timeOutExceeded = times;

		gameElapsedTime = (int)gameTimer.getElapsedTimeInMilliSec();
	}

	void ended()
	{
		gameOver = 1;
	}

	bool write(std::string filename)
	{
		gameTimeUp = Broodwar->getFrameCount() > 85714;

		std::ofstream outfile(filename.c_str(), std::ios::out);
		if (outfile.is_open())
		{
			outfile << selfName			<< std::endl;
			outfile << enemyName		<< std::endl;
			outfile << mapName			<< std::endl;
			outfile << frameCount		<< std::endl;
			outfile << selfScore		<< std::endl;
			outfile << enemyScore		<< std::endl;
			outfile << gameElapsedTime  << std::endl;
			outfile << BWAPI::Broodwar->self()->isDefeated()   << std::endl;
			outfile << BWAPI::Broodwar->self()->isVictorious() << std::endl;
			outfile << gameOver		    << std::endl;
			outfile << gameTimeUp       << std::endl;
			
			for (size_t i(0); i<timeOutExceeded.size(); ++i)
			{
				outfile << timeOutExceeded[i] << std::endl;
			}

			return true;
		}
		else
		{
			return false;
		}

		outfile.close();
	}	
};

std::vector<int> timerLimits;
std::vector<int> timerLimitsBound;
std::vector<int> timerLimitsExceeded;

int cameraMoveTime = 48;
int lastMoved = 0;
int localSpeed = 0;
int frameSkip = 0;
int gameTimeLimit = 85714;

bool drawBotNames = true;
bool drawUnitInfo = true;
bool drawTournamentInfo = true;

char buffer[MAX_PATH];

std::vector<int> frameTimes(100000,0);

void ExampleTournamentAI::onStart()
{
	
	GetModuleFileName(NULL, buffer, MAX_PATH);
	BWAPI::Broodwar->printf("Path is %s", buffer);

	// Set the command optimization level (reduces high APM, size of bloated replays, etc)
	Broodwar->setCommandOptimizationLevel(MINIMUM_COMMAND_OPTIMIZATION);

	timerLimits.push_back(55);
	timerLimitsBound.push_back(320);
	timerLimits.push_back(1000);
	timerLimitsBound.push_back(10);
	timerLimits.push_back(10000);
	timerLimitsBound.push_back(1);
	timerLimitsExceeded.push_back(0);
	timerLimitsExceeded.push_back(0);
	timerLimitsExceeded.push_back(0);

	parseConfigFile("bwapi-data\\tm_settings.ini");
	
	Broodwar->setLocalSpeed(localSpeed);
	Broodwar->setFrameSkip(frameSkip);
}


void ExampleTournamentAI::onEnd(bool isWinner)
{
	TournamentModuleState state = TournamentModuleState();
	state.ended();
	state.update(timerLimitsExceeded);
	state.write("gameState.txt");
}

void ExampleTournamentAI::onFrame()
{
	if ((int)frameTimes.size() < BWAPI::Broodwar->getFrameCount() + 10)
	{
		frameTimes.push_back(0);
	}

	moveCamera();

	if (Broodwar->getFrameCount() % 360 == 0)
	{
		TournamentModuleState state = TournamentModuleState();
		state.update(timerLimitsExceeded);
		state.write("gameState.txt");
	}

	if ((gameTimeLimit > 0) && (Broodwar->getFrameCount() > gameTimeLimit))
	{
		Broodwar->sendText("Game time limit of %d frames reached, exiting", gameTimeLimit);
		Broodwar->leaveGame();
	}
	
	int frame = BWAPI::Broodwar->getFrameCount();
	// check the sum of the times for the previous frame

	if (frame < 10)
	{
		return;
	}

	// add the framer times for this frame
	frameTimes[frame] += BWAPI::Broodwar->getLastEventTime();

	// the total time for the last frame
	int timeElapsed = frameTimes[frame-1];

	// check to see if the timer exceeded any frame time limits
	for (size_t t(0); t<timerLimits.size(); ++t)
	{
		if (timeElapsed > timerLimits[t])
		{
			timerLimitsExceeded[t]++;

			if (timerLimitsExceeded[t] >= timerLimitsBound[t])
			{
				Broodwar->sendText("TIMEOUT on %d ms", timerLimits[t]);
				Broodwar->leaveGame();
			}
		}
	}

	drawTournamentModuleSettings(10, 10);

	if (drawUnitInfo)
	{
		drawUnitInformation(440,6);
	}
}

void ExampleTournamentAI::drawTournamentModuleSettings(int x, int y)
{
	int drawX = x;
	int drawY = y;
	int width = 120;

	if (drawBotNames)
	{
		BWAPI::Broodwar->setTextSize(BWAPI::Text::Size::Huge);
		Broodwar->drawTextScreen(drawX, drawY, "\x07%s \x04vs. \x06%s", BWAPI::Broodwar->self()->getName().c_str(), BWAPI::Broodwar->enemy()->getName().c_str());
		drawY += 18;
		
		BWAPI::Broodwar->setTextSize(BWAPI::Text::Size::Large);
		Broodwar->drawTextScreen(drawX, drawY, "\x03%s", BWAPI::Broodwar->mapFileName().c_str());
		BWAPI::Broodwar->setTextSize();
		drawY += 30;
	}

	/*Broodwar->drawTextScreen(drawX, drawY, "\x04 Player Name:");
	Broodwar->drawTextScreen(drawX+width, drawY, "\x07 %s", BWAPI::Broodwar->self()->getName().c_str());
	drawY += 10;

	Broodwar->drawTextScreen(drawX, drawY, "\x04 Enemy Name:");
	Broodwar->drawTextScreen(drawX+width, drawY, "\x07 %s", BWAPI::Broodwar->enemy()->getName().c_str());
	drawY += 10;

	Broodwar->drawTextScreen(drawX, drawY, "\x04 Map Filename:");
	Broodwar->drawTextScreen(drawX+width, drawY, " %s", BWAPI::Broodwar->mapFileName().c_str());
	drawY += 20;*/

	if (drawTournamentInfo)
	{
		BWAPI::Broodwar->drawTextScreen(drawX, drawY, "\x04 Current Game Time: ");
		BWAPI::Broodwar->drawTextScreen(drawX + width, drawY, " %d", BWAPI::Broodwar->getFrameCount());
		drawY += 10;


		BWAPI::Broodwar->drawTextScreen(drawX, drawY, "\x04 Game Time Limit: ");
		BWAPI::Broodwar->drawTextScreen(drawX + width, drawY, " %d", gameTimeLimit);
		drawY += 10;

		BWAPI::Broodwar->drawTextScreen(drawX, drawY, "\x04 BWAPI Local Speed: ");
		BWAPI::Broodwar->drawTextScreen(drawX + width, drawY, " %d", localSpeed);
		drawY += 10;

		BWAPI::Broodwar->drawTextScreen(drawX, drawY, "\x04 BWAPI Frame Skip: ");
		BWAPI::Broodwar->drawTextScreen(drawX + width, drawY, " %d", frameSkip);
		drawY += 10;

		drawY += 10;
		for (size_t t(0); t<timerLimits.size(); ++t)
		{
			BWAPI::Broodwar->drawTextScreen(drawX, drawY, "\x04 # Frames > %d ms ", timerLimits[t]);
			BWAPI::Broodwar->drawTextScreen(drawX + width, drawY, " %d   (Max %d)", timerLimitsExceeded[t], timerLimitsBound[t]);

			drawY += 10;
		}
	}
	
}

void ExampleTournamentAI::drawUnitInformation(int x, int y) 
{
	
	std::string prefix = "\x04";

	//BWAPI::Broodwar->drawBoxScreen(x-5, y-4, x+200, y+200, BWAPI::Colors::Black, true);

	//BWAPI::Broodwar->drawTextScreen(x, y, "\x04 Unit Information: %s", BWAPI::Broodwar->self()->getRace().getName().c_str());
	BWAPI::Broodwar->drawTextScreen(x, y+20, "\x04%s's Units", BWAPI::Broodwar->self()->getName().c_str());
	BWAPI::Broodwar->drawTextScreen(x+160, y+20, "\x04#");
	BWAPI::Broodwar->drawTextScreen(x+180, y+20, "\x04X");

	int yspace = 0;

	// for each unit in the queue
	for each (BWAPI::UnitType t in BWAPI::UnitTypes::allUnitTypes()) 
	{
		int numUnits = BWAPI::Broodwar->self()->completedUnitCount(t) + BWAPI::Broodwar->self()->incompleteUnitCount(t);
		int numDeadUnits = BWAPI::Broodwar->self()->deadUnitCount(t);

		// if there exist units in the vector
		if (numUnits > 0) 
		{
			if (t.isWorker())			{ prefix = "\x0F"; }
			else if (t.isDetector())	{ prefix = "\x07"; }		
			else if (t.canAttack())		{ prefix = "\x08"; }		
			else if (t.isBuilding())	{ prefix = "\x03"; }
			else						{ prefix = "\x04"; }

			BWAPI::Broodwar->drawTextScreen(x, y+40+((yspace)*10), "%s%s", prefix.c_str(), t.getName().c_str());
			BWAPI::Broodwar->drawTextScreen(x+160, y+40+((yspace)*10), "%s%d", prefix.c_str(), numUnits);
			BWAPI::Broodwar->drawTextScreen(x+180, y+40+((yspace++)*10), "%s%d", prefix.c_str(), numDeadUnits);
		}
	}
}

void ExampleTournamentAI::moveCamera()
{
	if (BWAPI::Broodwar->getFrameCount() - lastMoved < cameraMoveTime)
	{
		return;
	}

	for (Unitset::iterator  unitIt=BWAPI::Broodwar->self()->getUnits().begin();unitIt!=BWAPI::Broodwar->self()->getUnits().end();unitIt++)
	{
		if (unitIt->isUnderAttack() || unitIt->isAttacking())
		{
			BWAPI::Broodwar->setScreenPosition(unitIt->getPosition() - BWAPI::Position(320, 240));
			lastMoved = BWAPI::Broodwar->getFrameCount();
		}
	}
}

void ExampleTournamentAI::onSendText(std::string text)
{
	frameTimes[BWAPI::Broodwar->getFrameCount()] += BWAPI::Broodwar->getLastEventTime();
}

void ExampleTournamentAI::onReceiveText(BWAPI::Player player, std::string text)
{
	frameTimes[BWAPI::Broodwar->getFrameCount()] += BWAPI::Broodwar->getLastEventTime();
}

void ExampleTournamentAI::onPlayerLeft(BWAPI::Player player)
{
	frameTimes[BWAPI::Broodwar->getFrameCount()] += BWAPI::Broodwar->getLastEventTime();
}

void ExampleTournamentAI::onPlayerDropped(BWAPI::Player* player)
{
	frameTimes[BWAPI::Broodwar->getFrameCount()] += BWAPI::Broodwar->getLastEventTime();
}

void ExampleTournamentAI::onNukeDetect(BWAPI::Position target)
{
	frameTimes[BWAPI::Broodwar->getFrameCount()] += BWAPI::Broodwar->getLastEventTime();
}

void ExampleTournamentAI::onUnitDiscover(BWAPI::Unit unit)
{
	frameTimes[BWAPI::Broodwar->getFrameCount()] += BWAPI::Broodwar->getLastEventTime();
}

void ExampleTournamentAI::onUnitEvade(BWAPI::Unit unit)
{
	frameTimes[BWAPI::Broodwar->getFrameCount()] += BWAPI::Broodwar->getLastEventTime();
}

void ExampleTournamentAI::onUnitShow(BWAPI::Unit unit)
{
	frameTimes[BWAPI::Broodwar->getFrameCount()] += BWAPI::Broodwar->getLastEventTime();
}

void ExampleTournamentAI::onUnitHide(BWAPI::Unit unit)
{
	frameTimes[BWAPI::Broodwar->getFrameCount()] += BWAPI::Broodwar->getLastEventTime();
}

void ExampleTournamentAI::onUnitCreate(BWAPI::Unit unit)
{
	frameTimes[BWAPI::Broodwar->getFrameCount()] += BWAPI::Broodwar->getLastEventTime();

	int mult = 3;

	if (BWAPI::Broodwar->getFrameCount() - lastMoved < cameraMoveTime*mult)
	{
		return;
	}

	BWAPI::Broodwar->setScreenPosition(unit->getPosition() - BWAPI::Position(320, 240));
	lastMoved = BWAPI::Broodwar->getFrameCount();
}

void ExampleTournamentAI::onUnitDestroy(BWAPI::Unit unit)
{
	frameTimes[BWAPI::Broodwar->getFrameCount()] += BWAPI::Broodwar->getLastEventTime();
}

void ExampleTournamentAI::onUnitMorph(BWAPI::Unit unit)
{
	frameTimes[BWAPI::Broodwar->getFrameCount()] += BWAPI::Broodwar->getLastEventTime();
}

void ExampleTournamentAI::onUnitComplete(BWAPI::Unit *unit)
{
	frameTimes[BWAPI::Broodwar->getFrameCount()] += BWAPI::Broodwar->getLastEventTime();
}

void ExampleTournamentAI::onUnitRenegade(BWAPI::Unit unit)
{
}

void ExampleTournamentAI::onSaveGame(std::string gameName)
{
}

bool ExampleTournamentModule::onAction(int actionType, void *parameter)
{
	switch ( actionType )
	{
		case Tournament::EnableFlag:
			switch ( *(int*)parameter )
			{
				case Flag::CompleteMapInformation:		return false;
				case Flag::UserInput:					return false;
			}

		case Tournament::PauseGame:
	//	case Tournament::RestartGame:
		case Tournament::ResumeGame:
		case Tournament::SetFrameSkip:
		case Tournament::SetGUI:
		case Tournament::SetLocalSpeed:					return false;
		case Tournament::SetMap:						return false; 
		case Tournament::LeaveGame:
	//	case Tournament::ChangeRace:
		case Tournament::SetLatCom:
		case Tournament::SetTextSize:
		case Tournament::SendText:
		case Tournament::Printf:
		case Tournament::SetCommandOptimizationLevel:	return false; 
							
		default:										break;
	}

	return true;
}

void ExampleTournamentModule::onFirstAdvertisement()
{

}


std::vector<std::string> ExampleTournamentAI::getLines(const std::string & filename)
{
    // set up the file
    std::ifstream fin(filename.c_str());
    if (!fin.is_open())
    {
		BWAPI::Broodwar->printf("Tournament Module Settings File Not Found, Using Defaults", filename.c_str());
		return std::vector<std::string>();
    }

	std::string line;

    std::vector<std::string> lines;

    // each line of the file will be a new player to add
    while (fin.good())
    {
        // get the line and set up the string stream
        getline(fin, line);
       
        // skip blank lines and comments
        if (line.length() > 1 && line[0] != '#')
        {
            lines.push_back(line);
        }
    }

	fin.close();

    return lines;
}

void ExampleTournamentAI::parseConfigFile(const std::string & filename)
{
    std::vector<std::string> lines(getLines(filename));

	if (lines.size() > 0)
	{
		timerLimits.clear();
		timerLimitsBound.clear();
		timerLimitsExceeded.clear();
	}

    for (size_t l(0); l<lines.size(); ++l)
    {
        std::istringstream iss(lines[l]);
        std::string option;
        iss >> option;

        if (strcmp(option.c_str(), "LocalSpeed") == 0)
        {
			iss >> localSpeed;
        }
        else if (strcmp(option.c_str(), "FrameSkip") == 0)
        {
            iss >> frameSkip;
        }
        else if (strcmp(option.c_str(), "Timeout") == 0)
        {
            int timeLimit = 0;
			int bound = 0;

			iss >> timeLimit;
			iss >> bound;

			timerLimits.push_back(timeLimit);
			timerLimitsBound.push_back(bound);
			timerLimitsExceeded.push_back(0);
        }
        else if (strcmp(option.c_str(), "GameFrameLimit") == 0)
        {
			iss >> gameTimeLimit;
        }
		else if (strcmp(option.c_str(), "DrawUnitInfo") == 0)
        {
            std::string val;
			iss >> val;
            
			if (strcmp(val.c_str(), "false") == 0)
			{
				drawUnitInfo = false;
			}
        }
		else if (strcmp(option.c_str(), "DrawTournamentInfo") == 0)
        {
            std::string val;
			iss >> val;
            
			if (strcmp(val.c_str(), "false") == 0)
			{
				drawTournamentInfo = false;
			}
        }
		else if (strcmp(option.c_str(), "DrawBotNames") == 0)
        {
            std::string val;
			iss >> val;
            
			if (strcmp(val.c_str(), "false") == 0)
			{
				drawBotNames = false;
			}
        }
		else
		{
			BWAPI::Broodwar->printf("Invalid Option in Tournament Module Settings: %s", option.c_str());
		}
    }
}