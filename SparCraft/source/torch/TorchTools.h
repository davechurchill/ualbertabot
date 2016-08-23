#pragma once

#include "../Common.h"
#include "../GameState.h"

#include "frame.h"
#include <istream>

namespace SparCraft
{
namespace TorchTools
{
	GameState GetSparCraftStateFromTorchCraftFrame(const replayer::Frame & frame);
	GameState GetSparCraftStateFromTorchCraftFrameStream(std::istream & in);
	GameState GetSparCraftStateFromTorchCraftFrameFile(const std::string & filename);

    void PrintStateValueFromFrameStream(std::istream & sin);
    void PrintMoveFromFrameStream(std::istream & in);
    Move GetMove(const GameState & state, const size_t & playerID, const std::string & aiPlayerName);
    std::string GetMoveStringLUA(const GameState & state, const Move & move);
}
}
