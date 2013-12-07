﻿#include "unit_morph.h"
#include <SCBW/scbwdata.h>
#include <SCBW/enumerations/TechId.h>
#include <SCBW/api.h>
#include <cassert>

namespace hooks {

//Check if @p unit can morph into @p morphUnitId.
bool unitCanMorphHook(const CUnit *unit, u16 morphUnitId) {
  //Default StarCraft behavior

  if (unit->id == UnitId::hydralisk) {
    if (morphUnitId == UnitId::lurker
        && unit->canUseTech(TechId::LurkerAspect, *ACTIVE_NATION_ID) == 1) {
      return true;
    }
  }

  if (unit->id == UnitId::larva || unit->id == UnitId::mutalisk) {
    if (unit->canBuild(morphUnitId, *ACTIVE_NATION_ID) == 1) {
      return true;
    }
  }

  return false;
}

//Check if @p unitId is an egg unit.
bool isEggUnitHook(u16 unitId) {

  if (unitId == UnitId::egg
      || unitId == UnitId::cocoon
      || unitId == UnitId::lurker_egg
      || unitId == UNIT_KUKULZA_COCOON)  //쿠쿨자 변태용 코쿤 추가
    return true;

  return false;
}

//Check if @p unitId is an egg unit that can be rallied
bool isRallyableEggUnitHook(u16 unitId) {

	if (unitId == UnitId::cocoon
      || unitId == UnitId::lurker_egg
      || unitId == UNIT_KUKULZA_COCOON)
    return false;

  return true;
}

//Return the ID of the egg unit to use when morphing @p unitId.
//If the unit cannot morph, return UnitId::None.
u16 getUnitMorphEggTypeHook(u16 unitId) {

  if (unitId == UnitId::larva)
    return UnitId::egg;

  if (unitId == UnitId::mutalisk)
    return UnitId::cocoon;

  if (unitId == UnitId::hydralisk)
    return UnitId::lurker_egg;

  if (unitId == UnitId::kukulza_mutalisk)   //뮤탈 영웅 변태시 사용하는 코쿤 유닛
	  return UNIT_KUKULZA_COCOON;

  return UnitId::None;
}

//Determine the type (unit ID) of the unit to revert to when cancelling an
//@p eggUnit while it is morphing.
u16 getCancelMorphRevertTypeHook(const CUnit *eggUnit) {

  if (eggUnit->id == UnitId::cocoon)
    return UnitId::mutalisk;

  if (eggUnit->id == UnitId::lurker_egg)
    return UnitId::hydralisk;

  if (eggUnit->id == UNIT_KUKULZA_COCOON)   //뮤탈 영웅 변태시 사용하는 코쿤 유닛
	  return UnitId::kukulza_mutalisk;

  return UnitId::None;  //Default (no revert for larvae)
}

//Determines the vertical (Y) offset by which the @p unit will be shifted to
//when it finishes morphing.
s16 getUnitVerticalOffsetOnBirth(const CUnit *unit) {

  //No offset, birth offset is handled elsewhere
  if (Unit::BaseProperty[unit->id] & UnitProperty::TwoUnitsIn1Egg)
    return 0;

  //No offset, since the morphed unit should stay where it is
  if (unit->displayedUnitId == UnitId::cocoon
      || unit->displayedUnitId == UNIT_KUKULZA_COCOON)  //뮤탈 영웅 변태시 사용하는 코쿤 유닛
    return 0;

  //Hovering units (?) float 7 pixels above ground
  //Note: This is not a mistake; SC actually uses a "==" comparison to check flags (I know it's a WTF).
  if (Unit::MovementFlags[unit->id] == (0x01 | 0x40 | 0x80))
    return -7;

  //Air units float 42 pixels above ground
  if (unit->status & UnitStatus::InAir)
    return -42;

  //Default for ground units
  return 0;
}

//Check if @p playerId has enough supplies to build @p unitId.
bool hasSuppliesForUnitHook(s8 playerId, u16 unitId, bool canShowErrorMessage) {
  //Default StarCraft behavior
  s32 supplyCost = Unit::SupplyRequired[unitId];

  if (Unit::BaseProperty[unitId] & UnitProperty::TwoUnitsIn1Egg)
    supplyCost *= 2;

  if (unitId == UnitId::lurker)
    supplyCost -= Unit::SupplyRequired[UnitId::hydralisk];

  aiSupplyReserved[playerId] = supplyCost;

  //No supply cost check needed
  if (supplyCost == 0 || Unit::BaseProperty[unitId] & UnitProperty::MorphFromOtherUnit)
    return true;

  const u8 raceId = scbw::getRaceId(unitId);
  assert(raceId <= 2);
  const u32 supplyUsed = raceSupply[raceId].used[playerId];

  //Must construct additional pylons
  if (supplyUsed + supplyCost > raceSupply[raceId].max[playerId]) {
    if (canShowErrorMessage)
      scbw::showErrorMessageWithSfx(playerId, 847 + raceId, 1 + raceId);
    return false;
  }

  //Supply limit exceeded
  if (supplyCost > scbw::getSupplyRemaining(playerId, raceId)) {
    if (canShowErrorMessage)
      scbw::showErrorMessageWithSfx(playerId, 844 + raceId, 153 + raceId);
    return false;
  }

  return true;
}

} //hooks