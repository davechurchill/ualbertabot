#pragma once
#include "OpponentView.h"

namespace AKBot
{
	class PlainOpponentView : public OpponentView
	{
		BWAPI::Player _self;
		BWAPI::Playerset *_enemies;
	public:
		BWAPI::Player self() const override { return _self; }
		BWAPI::Player defaultEnemy() const override { return *_enemies->begin(); }
		BWAPI::Playerset& enemies() const override { return *_enemies; }
		void self(BWAPI::Player value) { _self = value; }
	};
}