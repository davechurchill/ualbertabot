#pragma once
#include "OpponentView.h"

namespace AKBot
{
	/*
	 Provide view to the bot opponents using BWAPI
	*/
	class BWAPIOpponentView : public OpponentView
	{
	public:
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
		virtual BWAPI::Playerset& enemies() const override;
	};
}