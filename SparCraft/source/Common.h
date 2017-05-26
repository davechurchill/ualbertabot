#pragma once

#include "BWAPI.h"
#include "BaseTypes.hpp"
#include "SparCraftAssert.h"
#include "Config.h"

namespace SparCraft
{
	// constants for search
	namespace Constants
	{
		// directions of movement
		const int Move_Dir[4][2] = {{-1,0}, {1,0}, {0,1}, {0,-1} };
	}

    namespace Players
    {
        enum { Player_One = 0, Player_Two = 1, Num_Players = 2, Player_None = 3, Player_Both = 4, Size};
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
