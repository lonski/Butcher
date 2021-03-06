#ifndef DROP_RULE_H
#define DROP_RULE_H

#include <actors/actor_id.h>
#include <data/actors_generated.h>

namespace butcher {

struct DropRule
{
  DropRule()
    : itemId(ActorID::INVALID)
    , chance(0)
    , amountMin(0)
    , amountMax(0)
    , randomIngridientLevel(0)
  {}

  DropRule(const DropRuleData* d)
    : itemId( static_cast<ActorID>(d->item_id()))
    , chance(d->chance() )
    , amountMin(d->amount_min())
    , amountMax(d->amount_max())
    , randomIngridientLevel(d->random_ingridient_level())
  {
  }

  ActorID itemId;
  int chance;
  int amountMin;
  int amountMax;
  int randomIngridientLevel;
};

}

#endif // DROP_RULE_H
