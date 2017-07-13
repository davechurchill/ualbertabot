#pragma once
#include <memory>
#include <BWAPI\Player.h>
#include <BWAPI\Playerset.h>

namespace AKBot
{
	/*
	 Abstract class which provide information about bot player and his opponents
	*/
	class OpponentView
	{
	public:
		/*
		 Returns bot's player
		*/
		virtual BWAPI::Player self() = 0;
		
		/*
		 Returns set of opponents to the given bot.
		*/
		virtual BWAPI::Playerset& enemies() = 0;
	};

	typedef std::shared_ptr<OpponentView> OpponentViewPtr;
}