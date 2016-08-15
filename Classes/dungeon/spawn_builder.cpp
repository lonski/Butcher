#include "spawn_builder.h"
#include <dungeon/dungeon_description.h>
#include <dungeon/room.h>
#include <utils/path.h>
#include <data/levels_generated.h>

namespace cc = cocos2d;

namespace butcher {

bool SpawnBuilder::generateSpawns(DungeonDescription& dungeon)
{
  bool result = true;

  _objects.clear();
  _dungeon = &dungeon;

  if(_dungeon->tmx == nullptr)
  {
      cc::log("%s: Tile map is null.", __PRETTY_FUNCTION__);
      return false;
  }

  _objectsLayer = _dungeon->tmx->getObjectGroup("Objects");
  if(_objectsLayer == nullptr)
  {
      cc::log("%s: Tile map has no objects object layer.", __PRETTY_FUNCTION__);
      return false;
  }

  addPredefinedSpawns();
  result &= addStairs();
  addMobs();

  //debugMapPrint();

  _objectsLayer->setObjects(_objects);

  return result;
}

void SpawnBuilder::setMobIntroduction(const std::multimap<int, ActorID> &mobIntroduction)
{
  _mobIntroduction = mobIntroduction;
//  std::sort(_mobIntroduction.begin(), _mobIntroduction.end(), [](const std::pair<ActorID, int>& r, const std::pair<ActorID, int>& l){
//    return r.second > l.second;
//  });
}

void SpawnBuilder::addPredefinedSpawns()
{
  for ( auto& kv : _dungeon->spawns )
  {
    addActorSpawn((int)kv.second, kv.first);
  }
}

bool SpawnBuilder::addStairs()
{
  bool added = false;

  enum {
    TRY_COUNT = 100
  };

  cc::Vec2 upStairs;
  cc::Vec2 downStairs;

  for(int i = 0; i < TRY_COUNT; ++i)
  {
    upStairs = _dungeon->rooms.front()->getRandomCoord();

    if ( addActorSpawn(4, upStairs ) )
    {
      added = true;
      _dungeon->spawns[upStairs] = ActorID::STAIRS_UP;
      break;
    }

    upStairs = cc::Vec2::ZERO;
  }

  if  ( added )
  {
    added = false;
    for(int i = 0; i < TRY_COUNT; ++i)
    {
      downStairs = _dungeon->rooms.back()->getRandomCoord();

      if ( addActorSpawn(3,  downStairs ) )
      {
        _dungeon->spawns[downStairs] = ActorID::STAIRS_DOWN;
        added = true;
        break;
      }

      downStairs = cc::Vec2::ZERO;
    }
  }

  return added;
}

void SpawnBuilder::addMobs()
{
  //Grid g = _dungeon->grid;

  int mobCount(0);

  for ( std::shared_ptr<Rect> room : _dungeon->rooms )
  {
    int maxMobsInRoom = room->getFloorSize(_dungeon->grid) / _dungeon->settings->max_mob_density();
    while ( maxMobsInRoom-- )
    {
      if ( cc::RandomHelper::random_int(0, 100) <= _dungeon->settings->mob_spawn_chance() )
      {
        cc::Vec2 coord = room->getRandomFloorCoord(_dungeon->grid);
        if ( addActorSpawn( (int)getRandomMobID(), coord ) )
        {
          //g.set(coord, 'M');
          ++mobCount;
        }
      }
    }
  }

  cc::log("Spawned %d mobs in %u rooms.", mobCount, (unsigned)_dungeon->rooms.size());
  //cc::log("%s", g.toStr().c_str());
}

bool SpawnBuilder::addActorSpawn(int id, int y, int x)
{
  if ( _dungeon->grid.get(cc::Vec2(x,y)) != Tiles::FLOOR )
    return false;

  cc::ValueMap spawn;
  spawn["name"] = cc::Value("actor_spawn");
  spawn["id"] = cc::Value(id);
  spawn["x"] = cc::Value(x);
  spawn["y"] = cc::Value(y);

  if ( !canInsertObject(spawn) )
    return false;

  _objects.push_back(cc::Value(spawn));

  return true;
}

bool SpawnBuilder::addActorSpawn(int id, cocos2d::Vec2 pos)
{
  return addActorSpawn(id, pos.y, pos.x);
}

std::shared_ptr<Rect> SpawnBuilder::getRandomRoom() const
{
  return _dungeon->rooms[ cc::RandomHelper::random_int(0, (int)_dungeon->rooms.size() - 1) ];
}

void SpawnBuilder::debugMapPrint()
{
  Grid tmp = _dungeon->grid;
  tmp.floodfill(getRandomRoom()->getRandomCoord(), '+');
  cc::log("%s", tmp.toStr().c_str());
}

ActorID SpawnBuilder::getRandomMobID()
{
  int tries = 100;

  while( tries-- )
  {
    int lastLevel = 0;
    for( auto kv : _mobIntroduction)
    {
      if ( kv.first != lastLevel )
      {
        lastLevel = kv.first;

        if ( lastLevel <= _dungeon->level )
        {
          float chance = (float)lastLevel / (float)_dungeon->level * 0.5;
          if ( cc::RandomHelper::random_real<float>(0, 1) < chance )
          {
            //Get all mobs that can spawn on this level
            std::pair<MobIntroductionMap::iterator, MobIntroductionMap::iterator> levelGroup =
                _mobIntroduction.equal_range(kv.first);

            std::vector<ActorID> mobsOnThisLevel;
            for(auto i = levelGroup.first; i != levelGroup.second; ++i)
              mobsOnThisLevel.push_back(i->second);

            if ( !mobsOnThisLevel.empty() )
              return mobsOnThisLevel[ cc::RandomHelper::random_int(0, (int)mobsOnThisLevel.size() - 1) ];
          }
        }
      }
    }
  }

  return ActorID::COW;
}

bool SpawnBuilder::canInsertObject(cocos2d::ValueMap obj)
{
  for ( cc::Value& v : _objects )
  {
    cc::ValueMap vm = v.asValueMap();
    if ( vm["x"] == obj["x"] && vm["y"] == obj["y"] )
      return false;
  }
  return true;
}

}
