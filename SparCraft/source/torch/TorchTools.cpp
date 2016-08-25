#include "TorchTools.h"
#include "../AllPlayers.h"
#include "../AIParameters.h"

using namespace SparCraft;

GameState TorchTools::GetSparCraftStateFromTorchCraftFrame(const replayer::Frame & frame)
{
	GameState state;
	size_t currentUnitID = 0;
	const auto & allUnits = frame.units;

	for (const auto & kv : allUnits)
	{
		const auto & playerID = kv.first;
		const auto & playerUnits = kv.second;

		for (const auto & unit : playerUnits)
		{
			const size_t timeCanMove = state.getTime() + unit.groundCD;
			const size_t timeCanAttack = state.getTime() + unit.groundCD;

            const BWAPI::UnitType type(unit.type);

            if (!System::UnitTypeSupported(type))
            {
                continue;
            }

			SparCraft::Unit u(type, Position(unit.x * 8, unit.y * 8), currentUnitID++, playerID, unit.health + unit.shield, unit.energy, 0, timeCanAttack);
			u.setBWAPIUnitID(unit.id);
			state.addUnit(u);
		}
	}

	return state;
}

GameState TorchTools::GetSparCraftStateFromTorchCraftFrameStream(std::istream & in)
{
	replayer::Frame frame;
	in >> frame;
	return GetSparCraftStateFromTorchCraftFrame(frame);
}

GameState TorchTools::GetSparCraftStateFromTorchCraftFrameFile(const std::string & filename)
{
	std::ifstream fin(filename);
	return GetSparCraftStateFromTorchCraftFrameStream(fin);
}

void TorchTools::PrintMoveFromFrameStream(std::istream & sin)
{
    std::string aiPlayerName;
    int playerID = 0, mapWidth = 0, mapHeight = 0;

    sin >> aiPlayerName >> playerID >> mapWidth >> mapHeight;

    GameState state = TorchTools::GetSparCraftStateFromTorchCraftFrameStream(sin);
    state.setMap(std::shared_ptr<Map>(new Map(mapWidth, mapHeight)));

    PlayerPtr player = AIParameters::Instance().getPlayer(playerID, aiPlayerName);
    Move m;
    player->getMove(state, m);
    std::cout << TorchTools::GetMoveStringLUA(state, m);
}

void TorchTools::PrintStateValueFromFrameStream(std::istream & sin)
{
    std::string playerNames[2];
    int mapWidth = 0, mapHeight = 0;

    sin >> playerNames[0] >> playerNames[1] >> mapWidth >> mapHeight;

    GameState state = TorchTools::GetSparCraftStateFromTorchCraftFrameStream(sin);
    state.setMap(std::shared_ptr<Map>(new Map(mapWidth, mapHeight)));

    std::cout << Eval::EvalSim(state, 0, AIParameters::Instance().getPlayer(0, playerNames[0]), AIParameters::Instance().getPlayer(1, playerNames[1]), 0).val();
}

Move TorchTools::GetMove(const GameState & state, const size_t & playerID, const std::string & aiPlayerName)
{
    return Move();
}

std::string TorchTools::GetMoveStringLUA(const GameState & state, const Move & move)
{
    std::stringstream ss;
    ss << "{";

    for (size_t a(0); a < move.size(); ++a)
    {
        
        const Action & action = move[a];
        const Unit & unit = state.getUnitByID(action.getID());
        
        if (action.type() != ActionTypes::MOVE && action.type() != ActionTypes::ATTACK)
        {
            continue;
        }

        ss << "{tc.command_unit_protected, " << unit.getBWAPIUnitID() << ", ";
        if (action.type() == ActionTypes::MOVE)
        {
            ss << "tc.cmd.Move, -1, " << (action.pos().x() / 8) << ", " << (action.pos().y() / 8) << "}";
        }
        else if (action.type() == ActionTypes::ATTACK)
        {
            ss << "tc.cmd.Attack_Unit, " << state.getUnitByID(action.getTargetID()).getBWAPIUnitID() << ", -1, -1}";
        }

        if (a < move.size() - 1)
        {
            ss << ", ";
        }
    }

    ss << "}";

    return ss.str();
}
