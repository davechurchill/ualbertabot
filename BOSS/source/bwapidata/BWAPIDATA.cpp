#include <BWAPIDATA.h>

namespace BWAPIDATA
{
    Game * Broodwar;
}

void BWAPIDATA::BWAPIDATA_init()
{
  BWAPIDATA::Races::init();
  BWAPIDATA::DamageTypes::init();
  BWAPIDATA::ExplosionTypes::init();
  BWAPIDATA::Orders::init();
  BWAPIDATA::TechTypes::init();
  BWAPIDATA::PlayerTypes::init();
  BWAPIDATA::UpgradeTypes::init();
  BWAPIDATA::WeaponTypes::init();
  BWAPIDATA::UnitSizeTypes::init();
  BWAPIDATA::UnitCommandTypes::init();
  BWAPIDATA::UnitTypes::init();
  BWAPIDATA::BulletTypes::init();
  BWAPIDATA::Errors::init();
  BWAPIDATA::Colors::init();
  BWAPIDATA::GameTypes::init();
}
