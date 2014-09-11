#include "SparCraft.h"

using namespace SparCraft;

// IMPORTANT:
// Consult BaseTypes.hpp for all important variable typedefs
// Consult Common.h for all important constant and enum values 

Unit getSampleUnit()
{
    // Unit has several constructors
    // You will typically only be using this one to construct a 'starting' unit

    // Unit(const BWAPI::UnitType unitType, const IDType & playerID, const Position & pos)

    // The BWAPI::UnitType of the unit to be added
    BWAPI::UnitType marine = BWAPI::UnitTypes::Terran_Marine;

    // The player to add this unit to, specified by an IDType
    IDType player = Players::Player_One;

    // A Position, measured in Pixel coordinates
    Position p(0,0);

    // Simple unit constructor
    Unit marineAtOrigin(marine, player, p);
    
    return marineAtOrigin;
}

GameState getSampleState()
{
    // GameState only has a default constructor, you must add units to it manually
    GameState state;

    // The recommended way of adding a unit to a state is to just construct a unit and add it with:
    state.addUnit(getSampleUnit());

    // Or it can be added to the state via unit construction parameters
    state.addUnit(BWAPI::UnitTypes::Terran_Marine, Players::Player_One, Position(10,10));
    state.addUnit(BWAPI::UnitTypes::Protoss_Dragoon, Players::Player_Two, Position(40,40));

    // Units added with those 2 functions will be given a unique unitID inside GameState
    // If you require setting your own unique unitID for a unit, for example when translating a BWAPI::Broodwar state to GameState

    // Construct the unit
    Unit u(BWAPI::UnitTypes::Terran_Marine, Players::Player_One, Position(0,0));

    // Set the unitID
    u.setUnitID(5);

    // Add it to the state and tell it not to change the unitID.
    // If a state contains two units with the same ID, an error will occur
    state.addUnitWithID(u);

    return state;
}

Map getSampleMap()
{
    // Maps are used to constrain the movement of Units on a battlefield

    // There are 3 resolution scales at which positions operate in StarCraft maps
    // Pixel Resolution = 1x1 pixel = StarCraft smallest movement resolution
    // WalkTile Resolution = 8x8 pixels = StarCraft 'walkable' resolution
    // BuildTile Resolution = 32x32 pixels, or 4x4 WalkTiles = StarCraft "map size" resolution

    // Example: A Map of size 32*32 BuildTiles has size 128*128 WalkTiles or 1024*1024 pixels
    
    // The Map object constructor takes in size coordinates in BWAPI BuildTile resolution    
    Map smallMap(32, 32);

    // We can set the walkable values of WalkTile resolution via
    // void setMapData(const size_t & buildTileX, const size_t & buildTileY, const bool val)
    smallMap.setMapData(21, 98, false);

    // The default map sets all tiles to walkable, with an upper-left boundary of (0,0) and a lower-right boundary of (x,y)
    // We can query whether or not a unit can walk at a given position 
    bool canWalkHere = smallMap.isWalkable(Position(100, 30));

    // You can also construct a Map from a BWAPI::Game object, if you are using this code from within a bot
    // Map gameMap(BWAPI::BroodWar)

    // Once constructed, maps can be saved or loaded to files
    // A sample map (Destination) is provided under the sample_experiment directory
    // smallMap.load("mapname.txt");

    // We can set the Map of a GameState via a pointer to the map, as Map objects can be quite large:
    GameState state(getSampleState());
    state.setMap(&smallMap);

    return smallMap;
}

// When dealing with players, use a shared pointer, it's safer
// PlayerPtr is a boost::shared_pointer wrapper for Player *

PlayerPtr getSamplePlayer(const IDType playerID)
{
    // Player is the base class for all Player objects
    //
    // Scripted players all have the same constructor which is just the player ID which will be using this script
    // It is imoprtant to set that player ID correctly, as that player will only be generating and returning moves for that player

    PlayerPtr attackClosest(new Player_AttackClosest(playerID));

    return attackClosest;
}

std::vector<UnitAction> getSamplePlayerActionsFromState()
{
    // get our sample player
    IDType currentPlayerID = Players::Player_One;
    PlayerPtr myPlayer = getSamplePlayer(currentPlayerID);

    // Construct a blank vector of UnitActions, which are individual unit moves
    std::vector<UnitAction> move;

    // Get a state
    GameState state = getSampleState();

    // Construct a MoveArray. This structure will hold all the legal moves for each unit possible for this state
    MoveArray moveArray;

    // Generate the moves possible by currentPlayer from state into moveArray
    state.generateMoves(moveArray, currentPlayerID);

    // Call getMoves with these arguments
    myPlayer->getMoves(state, moveArray, move);
    
    return move;
}

void runSampleGame()
{
    // running a game is quite simple, you just need 2 players and an initial state
    GameState initialState = getSampleState();

    // get the players
    PlayerPtr p1 = getSamplePlayer(Players::Player_One);
    PlayerPtr p2 = getSamplePlayer(Players::Player_Two);

    // enter a maximum move limit for the game to go on for
    int moveLimit = 1000;

    // contruct the game
    Game g(initialState, p1, p2, moveLimit);

    // play the game
    g.play();

    // you can access the resulting game state after g has been played via getState
    GameState finalState = g.getState();

    // you can now evaluate the state however you wish. let's use an LTD2 evaluation from the point of view of player one
    StateEvalScore score = finalState.eval(Players::Player_One, EvaluationMethods::LTD2);

    // StateEvalScore has two components, a numerical score and a number of Movement actions performed by each player
    // with this evaluation, positive val means win, negative means loss, 0 means tie
    if (score.val() > 0)
    {
        std::cout << "Player One Wins!\n";
    }
    else if (score.val() < 0)
    {
        std::cout << "Player Two Wins!\n";
    }
    else
    {
        std::cout << "Game is a draw!\n";
    }
}