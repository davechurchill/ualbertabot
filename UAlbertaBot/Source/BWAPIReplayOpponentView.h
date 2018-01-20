#pragma once
#include "OpponentView.h"
#include <BWAPI/Game.h>

namespace AKBot
{
	/*
	 Provide view to the bot opponents using BWAPI
	*/
	class BWAPIReplayOpponentView : public OpponentView
	{
		BWAPI::Player _self;
		BWAPI::Playerset _enemies;
	public:
		/*
		* Initialize new instance of the @see BWAPIReplayOpponentView class
		* @param game BWAPI game
		*/
		BWAPIReplayOpponentView(BWAPI::Game* game);
		/*
		Returns bot's player
		*/
		virtual BWAPI::Player self() const override;

		/*
		Default enemy for current player
		*/
		virtual BWAPI::Player defaultEnemy() const override;

		/*
		Returns set of opponents to the given bot.
		*/
		virtual const BWAPI::Playerset& enemies() const override;
	};
}