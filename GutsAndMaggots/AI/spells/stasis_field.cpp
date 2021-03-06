#include "psi_storm.h"

namespace AI {

class StasisFieldTargetFinderProc: public scbw::UnitFinderCallbackMatchInterface {
  private:
    const CUnit *caster;
    bool isUnderAttack;
  public:
    StasisFieldTargetFinderProc(const CUnit *caster, bool isUnderAttack)
      : caster(caster), isUnderAttack(isUnderAttack) {}

    bool match(const CUnit *target) {
      if (target == caster)
        return false;

      if (!isTargetWorthHitting(target, caster))
        return false;

      if (!scbw::canWeaponTargetUnit(WeaponId::StasisField, target, caster))
        return false;

      if (Unit::BaseProperty[target->id] & UnitProperty::Building)
        return false;

      CUnit *targetOfTarget = target->orderTarget.unit;
      if (!targetOfTarget)
        return false;

      if (targetOfTarget->playerId >= 8)
        return false;

      if (!scbw::isAlliedTo(caster->playerId, targetOfTarget->getLastOwnerId()))
        return false;

      const int totalEnemyLife = getTotalEnemyLifeInArea(target->getX(), target->getY(), 96, caster, WeaponId::StasisField);
      if (!isUnderAttack && totalEnemyLife < 250)
        return false;

      const int totalAllyLife = getTotalAllyLifeInArea(target->getX(), target->getY(), 96, caster, WeaponId::StasisField);
      if (totalAllyLife * 2 >= totalEnemyLife)
        return false;

      return true;
    }
};

CUnit* findBestStasisFieldTarget(const CUnit *caster, bool isUnderAttack) {
  int bounds;
  if (isUnderAttack)
    bounds = 32 * 9;
  else if (isUmsMode(caster->playerId))
    bounds = 32 * 64;
  else
    bounds = 32 * 32;

  return scbw::UnitFinder::getNearest(caster->getX(), caster->getY(),
    caster->getX() - bounds, caster->getY() - bounds,
    caster->getX() + bounds, caster->getY() + bounds,
    StasisFieldTargetFinderProc(caster, isUnderAttack));
}

} //AI
