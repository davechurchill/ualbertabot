#pragma once

#include "BWAPI.h"
#include "BaseTypes.hpp"
#include "SparCraftAssert.h"

namespace SparCraft
{
	// constants for search
	namespace Constants
	{
		// number of players in the game
		const size_t Num_Players				= 2;
		
		// maximum number of units a player can have
		const size_t Max_Units					= 100;

		// max depth the search can ever handle
		const size_t Max_Search_Depth			= 50;

		// number of directions that units can move
		const size_t Num_Directions				= 4;

		// max number of ordered moves in a search depth
		const size_t Max_Ordered_Moves			= 10;

		// distance moved for a 'move' command
		const size_t Move_Distance				= 16;

        // add between a move and attack as penalty
        const TimeType Move_Penalty             = 4;

        // add range to units because of bounding boxes
        const PositionType Range_Addition       = 32;

		// maximum number of moves possible for any unit
		const size_t Max_Moves					= Max_Units + Num_Directions + 1;
		const bool   Use_Unit_Bounding			= false;
		const size_t Pass_Move_Duration			= 20;
		const float  Min_Unit_DPF				= 0.1f;
		const HealthType Starting_Energy		= 50;

		// whether to use transposition table in search
		const bool   Use_Transposition_Table	= true;
		const size_t Transposition_Table_Size	= 100000;
		const size_t Transposition_Table_Scan	= 10;
		const size_t Num_Hashes					= 2;
        
        // UCT options
        const size_t Max_UCT_Children           = 10;

		// rng seeding options
		const bool Seed_Hash_Time				= false;
		const bool Seed_Player_Random_Time		= true;

		// directions of movement
		const int Move_Dir[4][2] = {{-1,0}, {1,0}, {0,1}, {0,-1} };
	}

    namespace Players
    {
        enum { Player_One = 0, Player_Two = 1, Player_None = 2, Player_Both = 3, Size};
    }

    namespace SearchMethods
    {
        enum { AlphaBeta, IDAlphaBeta, MiniMax, Size };
    }

    namespace PlayerModels
    {
        enum { AlphaBeta, AttackClosest, Kiter, Random, AttackWeakest, AttackDPS, KiterDPS, NOKDPS, Kiter_NOKDPS, Cluster, PortfolioGreedySearch, UCT, None, Size };
    }

    namespace EvaluationMethods
    {
        enum { LTD, LTD2, Playout, Size };
    }

    namespace SearchNodeType
    {
        enum { Default, RootNode, SoloNode, FirstSimNode, SecondSimNode, Size };
    }

    namespace MoveOrderMethod
    {
        enum { ScriptFirst, None, Size };
    }

    namespace PlayerToMove
    {
        enum { Random, Alternate, Not_Alternate, Size };
    }

    namespace System
    {
        bool UnitTypeSupported(const BWAPI::UnitType & type);
    }
        
    namespace FileUtils
    {
        std::string ReadFile(const std::string & filename);
    }
};
