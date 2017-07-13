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
		virtual BWAPI::Player self() override;

		/*
		Returns set of opponents to the given bot.
		*/
		virtual BWAPI::Playerset& enemies() override;
	};
}