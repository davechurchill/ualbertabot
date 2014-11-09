#include <BWAPIDATA/AIModule.h>
namespace BWAPIDATA
{
  AIModule::AIModule()
  { }
  AIModule::~AIModule()
  { }
  void AIModule::onStart()
  { }
  void AIModule::onEnd(bool isWinner)
  { }
  void AIModule::onFrame()
  { }
  void AIModule::onSendText(std::string text)
  { }
  void AIModule::onReceiveText(Player* player, std::string text)
  { }
  void AIModule::onPlayerLeft(Player *player)
  { }
  void AIModule::onNukeDetect(Position target)
  { }
  void AIModule::onUnitDiscover(BWAPIDATA::Unit* unit)
  { }
  void AIModule::onUnitEvade(BWAPIDATA::Unit* unit)
  { }
  void AIModule::onUnitShow(BWAPIDATA::Unit* unit)
  { }
  void AIModule::onUnitHide(BWAPIDATA::Unit* unit)
  { }
  void AIModule::onUnitCreate(BWAPIDATA::Unit* unit)
  { }
  void AIModule::onUnitDestroy(BWAPIDATA::Unit* unit)
  { }
  void AIModule::onUnitMorph(BWAPIDATA::Unit* unit)
  { }
  void AIModule::onUnitRenegade(BWAPIDATA::Unit* unit)
  { }
  void AIModule::onSaveGame(std::string gameName)
  { }
}
