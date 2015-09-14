#pragma once

#include <vector>
#include <map>
#include "BWAPI.h"

namespace UAlbertaBot
{
class ControlGroupData
{
	std::map<BWAPI::Key, std::set<BWAPI::UnitInterface*>> keyMap;

public:

	ControlGroupData() {}
	~ControlGroupData() {}

	void update()
	{
		
		
	}

	void onUnitDestroy(BWAPI::UnitInterface* unit)
	{
		typedef std::map<BWAPI::Key, std::set<BWAPI::UnitInterface*>> mapType;
		for (mapType::value_type & t : keyMap)
		{
			t.second.erase(unit);
		}
	}

	// sets control group to only selected units
	void newControlGroup(BWAPI::Key k)
	{
		BWAPI::Broodwar->printf("New control group %d", k-BWAPI::K_0);

		keyMap[k] = std::set<BWAPI::UnitInterface*>();

		for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->getSelectedUnits())
		{
			if (unit->getPlayer() == BWAPI::Broodwar->self())
			{
				keyMap[k].insert(unit);
			}
		}
	}

	void addToControlGroup(BWAPI::Key k)
	{
		BWAPI::Broodwar->printf("Add to control group %d", k-BWAPI::K_0);

		if (!controlGroupExists(k))
		{
			keyMap[k] = std::set<BWAPI::UnitInterface*>();
		}

		for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->getSelectedUnits())
		{
			if (unit->getPlayer() == BWAPI::Broodwar->self())
			{
				keyMap[k].insert(unit);
			}
		}
	}

	bool controlGroupExists(BWAPI::Key k)
	{
		return keyMap.find(k) != keyMap.end();
	}

	std::set<BWAPI::UnitInterface*> * getControlGroupUnits(BWAPI::Key k)
	{
		if (!controlGroupExists(k))
		{
			keyMap[k] = std::set<BWAPI::UnitInterface*>();
		}

		return &keyMap[k];
	}
};

class EnhancedInterface
{
	int lastKeyEvent;
	int lastControlGroup;
	std::vector<BWAPI::Key> keys;

	ControlGroupData data;

	std::set<BWAPI::UnitInterface*> * selectedUnits;

public:

	EnhancedInterface() 
		: lastKeyEvent(0)
		, lastControlGroup(0)
		, selectedUnits(NULL)
	{
		// numbers
		keys.push_back(BWAPI::K_0);
		keys.push_back(BWAPI::K_1);
		keys.push_back(BWAPI::K_2);
		keys.push_back(BWAPI::K_3);
		keys.push_back(BWAPI::K_4);
		keys.push_back(BWAPI::K_5);
		keys.push_back(BWAPI::K_6);
		keys.push_back(BWAPI::K_7);
		keys.push_back(BWAPI::K_8);
		keys.push_back(BWAPI::K_9);

		// letters
		keys.push_back(BWAPI::K_A);
		keys.push_back(BWAPI::K_B);
		keys.push_back(BWAPI::K_C);
		keys.push_back(BWAPI::K_D);
		keys.push_back(BWAPI::K_E);
		keys.push_back(BWAPI::K_F);
		keys.push_back(BWAPI::K_G);
		keys.push_back(BWAPI::K_H);
		keys.push_back(BWAPI::K_I);
		keys.push_back(BWAPI::K_J);
		keys.push_back(BWAPI::K_K);
		keys.push_back(BWAPI::K_L);
		keys.push_back(BWAPI::K_M);
		keys.push_back(BWAPI::K_N);
		keys.push_back(BWAPI::K_O);
		keys.push_back(BWAPI::K_P);
		keys.push_back(BWAPI::K_Q);
		keys.push_back(BWAPI::K_R);
		keys.push_back(BWAPI::K_S);
		keys.push_back(BWAPI::K_T);
		keys.push_back(BWAPI::K_U);
		keys.push_back(BWAPI::K_V);
		keys.push_back(BWAPI::K_W);
		keys.push_back(BWAPI::K_X);
		keys.push_back(BWAPI::K_Y);
		keys.push_back(BWAPI::K_Z);
	}
	
	~EnhancedInterface() {}

	void onUnitDestroy(BWAPI::UnitInterface* unit)
	{
		data.onUnitDestroy(unit);
	}

	void update() 
	{
		data.update();
		processKeyboardEvents();
		processMouseEvents();

		if (selectedUnits)
		{
			drawSelectedUnits(175,275);
		}
	}

	void processKeyboardEvents()
	{
		for (BWAPI::Key k : keys)
		{
			if (BWAPI::Broodwar->getKeyState(k))
			{
				keyboardEvent(k);
			}
		}
	}

	void processMouseEvents()
	{
		if (BWAPI::Broodwar->getMouseState(BWAPI::M_RIGHT))
		{
			mouseEvent(BWAPI::M_RIGHT);
		}
	}

	void mouseEvent(BWAPI::MouseButton m)
	{
		int mouseMapX = BWAPI::Broodwar->getMousePosition().x + BWAPI::Broodwar->getScreenPosition().x;
		int mouseMapY = BWAPI::Broodwar->getMousePosition().y + BWAPI::Broodwar->getScreenPosition().y;

		if (m == BWAPI::M_RIGHT)
		{
			if (selectedUnits)
			{
				for (BWAPI::UnitInterface* unit : *selectedUnits)
				{
					unit->rightClick(BWAPI::Position(mouseMapX, mouseMapY));
				}
			}
		}
	}

	void keyboardEvent(BWAPI::Key k)
	{
		if (!validKeyEvent())
		{
			return;
		}

		bool control = BWAPI::Broodwar->getKeyState(BWAPI::K_CONTROL);
		bool shift = BWAPI::Broodwar->getKeyState(BWAPI::K_SHIFT);

		std::string prefix = std::string("") + (control ? "CTRL + " : "") + (shift ? "SHIFT + " : "");
		
		//BWAPI::Broodwar->printf("Key Pressed: %s %d", prefix.c_str(), k);

		switch (k) 
		{
			case BWAPI::K_D : 
				Options::Debug::DRAW_DEBUG_INTERFACE = false;
				break;
		}

		if (k >= BWAPI::K_0 && k <= BWAPI::K_9)
		{
			if (shift && control)
			{
				data.newControlGroup(k);
				selectedUnits = data.getControlGroupUnits(k);
				lastControlGroup = k;
			}
			else if (control)
			{
				data.addToControlGroup(k);
				selectedUnits = data.getControlGroupUnits(k);
				lastControlGroup = k;
			}
			else
			{
				selectedUnits = data.getControlGroupUnits(k);
				lastControlGroup = k;
			}
		}

		lastKeyEvent = BWAPI::Broodwar->getFrameCount();
	}

	bool validKeyEvent()
	{
		return (BWAPI::Broodwar->getFrameCount() - lastKeyEvent) > 20;
	}

	void drawSelectedUnits(int x, int y)
	{
		
		int col = 0;
		int row = 0;
		int cols = 12;

		BWAPI::Broodwar->drawBoxScreen(x-5, y-15, x+10+cols*22, y+100, BWAPI::Colors::Black, true);

		BWAPI::Broodwar->drawTextScreen(x, y-13,	"\x07%d Selected Units: Group %d", selectedUnits->size(), lastControlGroup - BWAPI::K_0);
		int yskip = 0;

		int xx = x;
		int yy = y;


		for (BWAPI::UnitInterface* unit : *selectedUnits)
		{
			xx = x + (col%cols) * 22;
			yy = y + (col/cols) * 26;

			BWAPI::Broodwar->drawCircleMap(unit->getPosition().x, unit->getPosition().y, unit->getType().dimensionLeft(), BWAPI::Colors::Green, false);
			
			if (unit->getType().isBuilding())
			{
				BWAPI::Broodwar->drawLineMap(unit->getPosition().x, unit->getPosition().y, unit->getRallyPosition().x, unit->getRallyPosition().y, BWAPI::Colors::White);
			}

			
			drawUnit(unit, xx, yy);

			col++;
		}
	}

	void drawUnit(BWAPI::UnitInterface* unit, int x, int y)
	{
		int BOX_HEIGHT = 20;
		int BOX_WIDTH = 20;

		int unitHealth = unit->getHitPoints() + unit->getShields();
		int maxUnitHealth = unit->getType().maxHitPoints() + unit->getType().maxShields();

		char unitChar = unit->getType().getName()[8];

		std::string prefixColor = "\x07";

		BWAPI::Color boxColor = BWAPI::Colors::Green;

		if (unitHealth < maxUnitHealth / 3)
		{
			boxColor = BWAPI::Colors::Red;
			prefixColor = "\x08";
		}
		else if (unitHealth < 2 * maxUnitHealth / 3)
		{
			boxColor = BWAPI::Colors::Orange;
			prefixColor = "\x11";
		}

		BWAPI::Broodwar->drawBoxScreen(x, y, x + BOX_WIDTH, y + BOX_HEIGHT, BWAPI::Colors::Black, true);
		BWAPI::Broodwar->drawBoxScreen(x, y, x + BOX_WIDTH, y + BOX_HEIGHT, boxColor, false);
		BWAPI::Broodwar->drawTextScreen(x + 2, y + 1, "%s%c", prefixColor.c_str(), unitChar);

		BWAPI::Color statusColor = getUnitColor(unit);

		int SUB_BOX_HEIGHT = 5;

		BWAPI::Broodwar->drawBoxScreen(x, y+BOX_HEIGHT+1, x+(int)(getBoxWidth(unit)*BOX_WIDTH), y+BOX_HEIGHT+SUB_BOX_HEIGHT, statusColor, true);
	}

	BWAPI::Color getUnitColor(BWAPI::UnitInterface* unit)
	{
		if (unit->getType().isWorker())
		{
			if (unit->isGatheringMinerals())
			{
				return BWAPI::Colors::Cyan;
			}

			if (unit->isGatheringGas())
			{
				return BWAPI::Colors::Green;
			}

			if (unit->isIdle())
			{
				return BWAPI::Colors::Grey;
			}
		}

		if (unit->getType().isBuilding())
		{
			if (unit->isTraining())
			{
				return BWAPI::Colors::Grey;
			}

			return BWAPI::Colors::Black;
		}

		return BWAPI::Colors::Red;
	}

	double getBoxWidth(BWAPI::UnitInterface* unit)
	{
		if (unit->isTraining())
		{
			return 1 - (double)unit->getRemainingTrainTime() / unit->getTrainingQueue().front().buildTime();
		}

		return 1;
	}
};
}