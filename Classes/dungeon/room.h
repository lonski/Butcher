#ifndef ROOM_H
#define ROOM_H

#include <utils/grid.h>
#include "cocos2d.h"

namespace butcher {

struct Rect
{
  Rect(int w = 0, int h = 0, cocos2d::Vec2 c = cocos2d::Vec2::ZERO);

  cocos2d::Vec2 getCenter() const;
  cocos2d::Vec2 getRandomCoord(int margin = 1) const;
  int getAreaSize() const;
  int getFloorSize(const Grid& gridPlacedOn) const;
  cocos2d::Vec2 getRandomFloorCoord(const Grid& gridPlacedOn) const;
  cocos2d::Vec2 findCoordWithPattern(const Grid& gridPlacedOn, const std::string& pattern, int tries = 100);
  bool place(Grid& grid, char c);

  int width;
  int height;
  cocos2d::Vec2 coord;
};

}

#endif // ROOM_H
