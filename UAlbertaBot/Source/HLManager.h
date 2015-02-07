#pragma once
#include "Common.h"
#include "Squad.h"
#include "HLSearch.h"
#include "ScoutManager.h"
namespace UAlbertaBot
{
	class HLManager
	{
		//ScoutManager scoutManager;
		HLManager();
		~HLManager();
	public:

		static	HLManager &	Instance();

		void update();
		void search(double timeLimit);
	};

}