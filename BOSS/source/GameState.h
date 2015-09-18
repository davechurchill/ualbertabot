#pragma once

#include "Common.h"

#include "ActionInProgress.h"
#include "BuildingData.h"
#include "UnitData.h"
#include "ActionType.h"
#include "PrerequisiteSet.h"
#include "ActionSet.h"

//#define ENABLE_BWAPI_GAMESTATE_CONSTRUCTOR

namespace BOSS
{
    
typedef std::pair<ResourceCountType, ResourceCountType>     ResourcePair;
typedef std::pair<FrameCountType, FrameCountType>           FramePair;

class ActionPerformed
{
public:
    ActionType          actionType;
    FrameCountType      actionQueuedFrame;
    ResourceCountType   mineralsWhenQueued;
    ResourceCountType   gasWhenQueued;

    ActionPerformed()
        : actionQueuedFrame(0)
        , mineralsWhenQueued(0)
        , gasWhenQueued(0)
    {
    
    }
};

class GameState 
{
    UnitData                    _units;  
    RaceID                      _race;

    ActionType                  _actionPerformed; 		    // the action which generated this state
    size_t                      _actionPerformedK;

    FrameCountType              _currentFrame;
    FrameCountType              _lastActionFrame;		    // the current frame of the game

    ResourceCountType           _minerals; 			        // current mineral count
    ResourceCountType           _gas;						// current gas count

    Vec<ActionPerformed, 100>   _actionsPerformed;

    const FrameCountType        raceSpecificWhenReady(const ActionType & a) const;
    void                        fixZergUnitMasks();
    
    const FrameCountType        whenSupplyReady(const ActionType & action)                              const;
    const FrameCountType        whenPrerequisitesReady(const ActionType & action)                       const;
    const FrameCountType        whenBuildingPrereqReady(const ActionType & action)                      const;
    //const FrameCountType        whenConstructedBuildingReady(const ActionType & builder)                const;
    const FrameCountType        whenMineralsReady(const ActionType & action)                            const;
    const FrameCountType        whenGasReady(const ActionType & action)                                 const;
    const FrameCountType        whenWorkerReady(const ActionType & action)                              const;

public: 

    GameState(const RaceID r = Races::None);

// constructor based on BWAPI::Game only makes sense if using VS
// we won't be using this if we're compiling to emscripten or linux
#ifdef _MSC_VER
    GameState(BWAPI::GameWrapper & game, BWAPI::PlayerInterface * player, const std::vector<BWAPI::UnitType> & buildingsQueued);
#endif

	std::vector<ActionType>     doAction(const ActionType & action);
    std::vector<ActionType>     fastForward(const FrameCountType toFrame) ;
    void                        finishNextActionInProgress();

    const FrameCountType        getCurrentFrame()                                                       const;
    const FrameCountType        whenCanPerform(const ActionType & action)                               const;
    const FrameCountType        getLastActionFinishTime()                                               const;

    void                        getAllLegalActions(ActionSet & actions)                                 const;
    bool                        whyIsNotLegal(const ActionType & action)                                const;
    bool                        isLegal(const ActionType & action)                                      const;
    bool                        canAfford(const ActionType & action)                                    const;
    bool                        canAffordGas(const ActionType & action)                                 const;
    bool                        canAffordMinerals(const ActionType & action)                            const;

    size_t 				        getMineralsPerFrame()		    const;
    size_t 				        getGasPerFrame()	            const;
    const UnitCountType 		getNumMineralWorkers()	        const;				
    const UnitCountType 		getNumGasWorkers()		        const;					
    const UnitCountType 		getNumBuildingWorkers()	        const;		
    const ResourceCountType 	getMinerals() 				    const;
    const ResourceCountType	    getGas()					    const;
    const RaceID                getRace()                       const;

    const UnitData &            getUnitData()                   const;

    const ResourceCountType     getMinerals(const int frame)    const;
    const ResourceCountType     getGas(const int frame)         const;
    const ResourceCountType     getFinishTimeMinerals()         const;
    const ResourceCountType     getFinishTimeGas()              const;

    const std::string           toString()                      const;
    const std::string           getActionsPerformedString()     const;
    const BuildingData &        getBuildingData()               const;
    const HatcheryData &        getHatcheryData()               const;

    void                        setStartingState();
    void                        setMinerals(const ResourceCountType & minerals);
    void                        setGas(const ResourceCountType & gas);
    void                        addCompletedAction(const ActionType & action, const size_t num = 1);
	void                        removeCompletedAction(const ActionType & action, const size_t num = 1);
};
}
