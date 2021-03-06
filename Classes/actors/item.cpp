#include "item.h"
#include <data/actors_generated.h>
#include "cocos2d.h"
#include <utils/utils.h>
#include <butcher.h>
#include <data/actors_database.h>

namespace cc = cocos2d;

namespace butcher {

Item::Item(const ActorData* data)
  : Actor(data)
  , _slot(ItemSlotType::NONE)
  , _breakChance(0)
  , _category(ItemCategory::None)
  , _ammoId(ActorID::INVALID)
  , _usable(false)
  , _effect(EffectID::None)
  , _useTarget(UseTarget::None)
  , _hp(0)
  , _radius(0)
{
  if ( data )
  {
    _slot = static_cast<ItemSlotType>(data->body_slot());

    if ( data->attack() )
      setAttribute(AttributeType::Attack, data->attack());

    if ( data->defense() )
      setAttribute(AttributeType::Defense, data->defense());

    if ( data->damage_reduction() )
      setAttribute(AttributeType::DamageReduction, data->damage_reduction());

    if ( data->damage() )
      _damage.parse(data->damage()->c_str());

    _breakChance = data->break_chance();
    _level = data->level();
    _category = static_cast<ItemCategory>(data->category());
    _ammoId = static_cast<ActorID>(data->ammo_id());
    _usable = data->is_usable();
    _effect = static_cast<EffectID>(data->effect_id());
    _useTarget = static_cast<UseTarget>(data->use_target());
    _hp = data->hp();
    _radius = data->radius();
  }
}

int Item::getZ() const
{
  return 11;
}

std::unique_ptr<Actor> Item::clone(std::unique_ptr<Actor> allocated)
{
  Item* o = dynamic_cast<Item*>(allocated.release());
  if ( o == nullptr )
    o = new Item(nullptr);

  o->_slot = _slot;
  o->_attributes = _attributes;
  o->_damage = _damage;
  o->_breakChance = _breakChance;
  o->_level = _level;
  o->_category = _category;
  o->_ammoId = _ammoId;
  o->_usable = _usable;
  o->_effect = _effect;
  o->_useTarget = _useTarget;
  o->_hp = _hp;
  o->_radius = _radius;

  return std::move(Actor::clone(std::unique_ptr<Actor>{o}));
}

bool Item::isUsable() const
{
  return _usable;
}

ItemSlotType Item::getItemSlotType() const
{
  return _slot;
}

Damage Item::getDamage() const
{
  return _damage;
}

int Item::getAttribute(AttributeType type)
{
  return _attributes[type];
}

void Item::setAttribute(AttributeType type, int value)
{
  _attributes[type] = value;
}

bool Item::rollBreak() const
{
  int roll = cc::RandomHelper::random_int(1,1000000);
  return  roll <= _breakChance;
}

int Item::getLevel() const
{
  return _level;
}

std::vector<std::string> Item::getItemInfo()
{
  std::vector<std::string> info;

  if ( _category != ItemCategory::None )
    info.push_back("Category: " + ItemCategory2Str(_category));

  if ( _level > 0 )
    info.push_back("Level: " + toStr(_level));

  if ( _damage.toInt() != 0 )
    info.push_back("Damage: " + _damage.toString());

  if ( _ammoId != ActorID::INVALID )
    info.push_back( "Ammo: " + BUTCHER.actorsDatabase().getName(_ammoId) );

  if ( _usable && _useTarget != UseTarget::None )
    info.push_back( "Usable (" + UseTarget2Str(_useTarget) + ")" );

  for ( auto a : AttributeType() )
  {
    int val = getAttribute(a);
    if ( val != 0 )
      info.push_back(AttributeType2Str(a) + ": " + (val > 0 ? "+" : "") + toStr(val));
  }

  if ( _hp > 0 || _effect != EffectID::None)
  {
    info.push_back(" ");
    info.push_back("Provides effect:");

    if ( _effect != EffectID::None )
    {
      Effect e = BUTCHER.effectsDatabase().createEffect(_effect);
      std::string duration = e.getTurns() > 0 ? " for " + toStr(e.getTurns()) + " turns" : "";
      for ( Modifier m : e.getModifiers() )
      {
        if ( m.attribute != AttributeType::None && m.value != 0 )
          info.push_back(
                AttributeType2Str(m.attribute) + ": " + ( m.value > 0 ? "+" : "" ) + toStr(m.value) + duration );
        if ( m.special != SpecialModifierType::None )
          info.push_back(SpecialModifierType2Str(m.special));
      }
    }
  }

  if ( !_effects.empty() )
  {
    info.push_back(" ");
    info.push_back("Effects applied:");

    for ( auto& kv : _effects )
    {
      std::string duration = kv.second.getTurns() > 0 ? " (" + toStr(kv.second.getTurns()) + " turns)" : "";
      info.push_back(kv.second.getName() + duration);
    }
  }

  return info;
}

ItemCategory Item::getCategory() const
{
  return _category;
}

ActorID Item::getAmmoId() const
{
  return _ammoId;
}

EffectID Item::getEffectID() const
{
  return _effect;
}

UseTarget Item::getUseTarget() const
{
  return _useTarget;
}

int Item::getHp() const
{
  return _hp;
}

int Item::getRadius() const
{
  return _radius;
}

}
