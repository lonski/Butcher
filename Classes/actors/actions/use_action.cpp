#include "use_action.h"
#include "cocos2d.h"
#include <butcher.h>
#include <actors/character.h>
#include <actors/player.h>
#include <dungeon/dungeon_state.h>
#include <actors/actions/throw_action.h>

namespace cc = cocos2d;

namespace butcher {

UseAction::UseAction(std::shared_ptr<Item> item)
  : _item(item, 1)
{
}

UseAction::UseAction(const AmountedItem &amountedItem)
  : _item(amountedItem)
{
}

ActorAction::Result UseAction::perform(std::shared_ptr<Actor> user)
{
  _user = std::dynamic_pointer_cast<Character>(user);

  if ( !_user )
  {
    cc::log("%s user is not a character", __PRETTY_FUNCTION__);
    return ActorAction::Result::NOK;
  }

  if ( !_item.item )
  {
    cc::log("%s item is null", __PRETTY_FUNCTION__);
    return ActorAction::Result::NOK;
  }

  _effect = BUTCHER.effectsDatabase().createEffect(_item.item->getEffectID());

  ActorAction::Result ret = ActorAction::Result::NOK;

  switch (_item.item->getUseTarget())
  {
    case UseTarget::Self:
      ret = useOnSelf();
    break;
    case UseTarget::Weapon:
      ret = useOnWeapon();
    break;
    case UseTarget::Floor:
      ret = useOnFloor();
    break;
    case UseTarget::Range:
      ret = useRange();
    break;
    default:
      cc::log("%s invalid UseTarget: %d", __PRETTY_FUNCTION__, (int)_item.item->getUseTarget());
    break;
  }

  return ret;
}

ActorAction::Result UseAction::useOnSelf()
{
  //Apply effect
  if ( _effect.getID() != EffectID::None )
  {
    if ( _user == BUTCHER.getPlayer() )
    {
      std::string message = _effect.getName() + " fades";
      _effect.setOnRemoveFn([=](){
        BUTCHER.getPlayer()->fadeText( message, cc::Color4B::ORANGE, 1);
      });
    }

    _user->addEffect( _effect );
    _user->fadeText( _effect.getName(), cc::Color4B::ORANGE, 1, false);
  }

  return ActorAction::Result::COST_EXTRA_TURN;
}

ActorAction::Result UseAction::useOnWeapon()
{
  std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(_user);

  if ( !player )
  {
    cc::log("%s user is not a player", __PRETTY_FUNCTION__);
    return ActorAction::Result::NOK;
  }

  AmountedItem wpn = player->getInventory().equipped(ItemSlotType::WEAPON);
  if ( !wpn.item )
  {
    cc::log("%s weapon is not equipped", __PRETTY_FUNCTION__);
    return ActorAction::Result::NOK;
  }

  if ( _effect.getID() == EffectID::None )
  {
    cc::log("%s invalid effect", __PRETTY_FUNCTION__);
    return ActorAction::Result::NOK;
  }

  std::string message = "Weapon: " + _effect.getName() + " fades";
  _effect.setOnRemoveFn([=](){
    BUTCHER.getPlayer()->fadeText( message, cc::Color4B::ORANGE, 1);
  });

  wpn.item->addEffect( _effect );
  player->fadeText( "Weapon: " + _effect.getName(), cc::Color4B::ORANGE, 1, false);

  return ActorAction::Result::COST_EXTRA_TURN;
}

ActorAction::Result UseAction::useOnFloor()
{
  std::shared_ptr<Actor> item = _item.item->clone();
  item->setTileCoord( BUTCHER.getPlayer()->getTileCoord() );

  DungeonState* dung = BUTCHER.getCurrentDungeon();
  dung->addActor(item);

  return ActorAction::Result::COST_EXTRA_TURN;
}

ActorAction::Result UseAction::useRange()
{
  std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(_user);

  if ( !player )
  {
    cc::log("%s user is not a player", __PRETTY_FUNCTION__);
    return ActorAction::Result::NOK;
  }

  std::shared_ptr<ThrowAction> action(new ThrowAction(_item.item->clone()));
  player->scheduleAction(action);

  return ActorAction::Result::OK;
}

}
