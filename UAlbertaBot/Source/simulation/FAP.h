#pragma once

#include <BWAPI/DamageType.h>
#include "..\UnitData.h"

namespace UAlbertaBot {

	class FastAPproximation {
		struct FAPUnit {
			FAPUnit(BWAPI::Unit u);
			FAPUnit(UnitInfo ui);
			const FAPUnit &operator= (const FAPUnit &other) const;

			int id = 0;

			mutable int x = 0, y = 0;

			mutable int health = 0;
			mutable int maxHealth = 0;
			mutable int armor = 0;

			mutable int shields = 0;
			mutable int shieldArmor = 0;
			mutable int maxShields = 0;

			mutable double speed = 0;
			mutable bool flying = 0;
			mutable int elevation = -1;

			mutable BWAPI::UnitSizeType unitSize;

			mutable int groundDamage = 0;
			mutable int groundCooldown = 0;
			mutable int groundMaxRange = 0;
			mutable int groundMinRange = 0;
			mutable BWAPI::DamageType groundDamageType;

			mutable int airDamage = 0;
			mutable int airCooldown = 0;
			mutable int airMaxRange = 0;
			mutable int airMinRange = 0;
			mutable BWAPI::DamageType airDamageType;

			mutable BWAPI::UnitType unitType;
			mutable BWAPI::Player player = nullptr;
			mutable int healTimer = 0;
			mutable bool isOrganic = false;
			mutable bool didHealThisFrame = false;
			mutable int score = 0;

			mutable int attackCooldownRemaining = 0;

#ifdef _DEBUG

			mutable int damageTaken = 0;

#endif

			bool operator< (const FAPUnit &other) const;
		};

		public:

			FastAPproximation();

			void addUnitPlayer1(FAPUnit fu);
			void addIfCombatUnitPlayer1(FAPUnit fu);
			void addUnitPlayer2(FAPUnit fu);
			void addIfCombatUnitPlayer2(FAPUnit fu);

			void simulate(int nFrames = 96); // = 24*4, 4 seconds on fastest

			std::pair <int, int> playerScores() const;
			std::pair <int, int> playerScoresUnits() const;
			std::pair <int, int> playerScoresBuildings() const;
			std::pair <std::vector <FAPUnit> *, std::vector <FAPUnit> *> getState();
			void clearState();

		private:
			std::vector <FAPUnit> player1, player2;

			bool didSomething;
			void dealDamage(const FastAPproximation::FAPUnit & fu, int damage, BWAPI::DamageType damageType) const;
			int distButNotReally(const FastAPproximation::FAPUnit & u1, const FastAPproximation::FAPUnit & u2) const;
			bool isSuicideUnit(BWAPI::UnitType ut);
			void unitsim(const FAPUnit & fu, std::vector <FAPUnit> &enemyUnits);
			void medicsim(const FAPUnit & fu, std::vector <FAPUnit> &friendlyUnits);
			bool suicideSim(const FAPUnit & fu, std::vector <FAPUnit> &enemyUnits);
			void isimulate();
			void unitDeath(const FAPUnit & fu, std::vector <FAPUnit> &itsFriendlies);
			void convertToUnitType(const FAPUnit &fu, BWAPI::UnitType ut);

	};

}

extern UAlbertaBot::FastAPproximation fap;
