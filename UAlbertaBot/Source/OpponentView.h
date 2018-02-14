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
		virtual ~OpponentView() = default;
		/*
		 Returns bot's player
		*/
		virtual BWAPI::Player self() const = 0;
		/*
		Default enemy for current player
		*/
		virtual BWAPI::Player defaultEnemy() const = 0;
		
		/*
		 Returns set of opponents to the given bot.
		*/
		virtual const BWAPI::Playerset& enemies() const = 0;
	};
}