#include "melee_attack_state.h"
#include <actors/actions/attack_action.h>
#include <actors/ai/ai.h>
#include <actors/actor.h>
#include <utils/profiler.h>

namespace butcher {

namespace state{

MeleeAttack::MeleeAttack()
{
}

FSMStateType MeleeAttack::getType() const
{
  return FSMStateType::MELEE_ATTACK;
}

void MeleeAttack::update()
{
  if ( validatePreconditions() )
  {
    _ai->getActor()->performAction( new AttackAction(_ai->getTarget()) );
  }
}

bool MeleeAttack::canEnter()
{
  return true;
}

}}
