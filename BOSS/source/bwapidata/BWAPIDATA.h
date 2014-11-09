#include <BWAPIDATA/AIModule.h>
#include <BWAPIDATA/Bullet.h>
#include <BWAPIDATA/BulletType.h>
#include <BWAPIDATA/Color.h>
#include <BWAPIDATA/Constants.h>
#include <BWAPIDATA/CoordinateType.h>
#include <BWAPIDATA/DamageType.h>
#include <BWAPIDATA/Error.h>
#include <BWAPIDATA/Event.h>
#include <BWAPIDATA/EventType.h>
#include <BWAPIDATA/ExplosionType.h>
#include <BWAPIDATA/Flag.h>
#include <BWAPIDATA/Force.h>
#include <BWAPIDATA/Game.h>
#include <BWAPIDATA/GameType.h>
#include <BWAPIDATA/Input.h>
#include <BWAPIDATA/Latency.h>
#include <BWAPIDATA/Order.h>
#include <BWAPIDATA/Player.h>
#include <BWAPIDATA/PlayerType.h>
#include <BWAPIDATA/Position.h>
#include <BWAPIDATA/Race.h>
#include <BWAPIDATA/TechType.h>
#include <BWAPIDATA/TilePosition.h>
#include <BWAPIDATA/Unit.h>
#include <BWAPIDATA/UnitCommand.h>
#include <BWAPIDATA/UnitCommandType.h>
#include <BWAPIDATA/UnitSizeType.h>
#include <BWAPIDATA/UnitType.h>
#include <BWAPIDATA/UpgradeType.h>
#include <BWAPIDATA/WeaponType.h>

namespace BWAPIDATA
{
  /** You have to call this from your AIModule Dllmain function.
   *
   * \code
   * BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
   * {
   *   switch (ul_reason_for_call)
   *   {
   *     case DLL_PROCESS_ATTACH:
   *       BWAPIDATA::BWAPIDATA_init();
   *       break;
   *     case DLL_PROCESS_DETACH:
   *       break;
   *   }
   *
   *   return TRUE;
   * }
   * \endcode */
  void BWAPIDATA_init();
}
