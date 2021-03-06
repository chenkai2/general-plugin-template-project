﻿#include "armor_bonus.h"
#include "../SCBW/scbwdata.h"
#include "../SCBW/enumerations.h"
#include "../SCBW/api.h"

namespace hooks {

/// Returns the bonus armor for this unit.
u8 getArmorBonusHook(const CUnit *unit) {
  using scbw::getUpgradeLevel;

  u8 armorUpg = 0;
  if (scbw::isBroodWarMode()) {
    if (unit->id == UnitId::Hero_Torrasque || unit->id == UnitId::ultralisk) {
      if ((Unit::BaseProperty[unit->id] & UnitProperty::Hero)
          || getUpgradeLevel(unit->playerId, UpgradeId::ChitinousPlating)) {
        armorUpg = 2;
      }
    }
  }

  //테란 건물 방어력 업그레이드
  if (Unit::BaseProperty[unit->id] & UnitProperty::Building
      && Unit::GroupFlags[unit->id].isTerran
      && getUpgradeLevel(unit->playerId, UPGRADE_TERRAN_BLDG_ARMOR))
  {
    armorUpg = 5;
  }

  return armorUpg + getUpgradeLevel(unit->playerId, Unit::ArmorUpgrade[unit->id]);
}

}