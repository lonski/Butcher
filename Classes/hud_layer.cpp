#include "hud_layer.h"
#include <butcher.h>
#include <dungeon/generators/cave_grid_generator.h>
#include <dungeon/generators/tmx_builder.h>
#include <actors/player.h>
#include <utils/utils.h>
#include <dungeon/dungeon_state.h>
#include <dungeon/generators/DungeonMaker.h>
#include <loading_scene.h>

namespace cc = cocos2d;

namespace butcher {

HudLayer::HudLayer()
{

}

HudLayer::~HudLayer()
{
}

bool HudLayer::init()
{
    if (Layer::init())
    {

        auto menuBtn = cc::MenuItemImage::create(  "button/menu.png",
                                               "button/menu_click.png",
                                               CC_CALLBACK_1(HudLayer::showMenu, this));

        cc::Vec2 origin = cc::Director::getInstance()->getVisibleOrigin();
        auto visibleSize = cc::Director::getInstance()->getVisibleSize();

        int margin = 10;
        menuBtn->setAnchorPoint(cc::Vec2(0,0));
        menuBtn->setPosition(cc::Vec2(origin.x + visibleSize.width - menuBtn->getBoundingBox().size.width - margin, origin.y + margin));

        auto menu = cc::Menu::create(menuBtn, NULL);
        menu->setPosition(cc::Vec2::ZERO);
        this->addChild(menu, 1);

        auto menuBtn2 = cc::MenuItemImage::create(  "button/menu.png",
                                               "button/menu_click.png",
                                               CC_CALLBACK_1(HudLayer::showMenu2, this));

        menuBtn2->setAnchorPoint(cc::Vec2(0,0));
        menuBtn2->setPosition(cc::Vec2(origin.x + visibleSize.width - menuBtn2->getBoundingBox().size.width * 2 - margin, origin.y + margin));

        auto menu2 = cc::Menu::create(menuBtn2, NULL);
        menu2->setPosition(cc::Vec2::ZERO);
        this->addChild(menu2, 1);

    }

    return true;
}

void HudLayer::showMenu(Ref *)
{
  LoadingScreen::run([&](){
    BUTCHER.goToLevel(BUTCHER.getDungeonLevel() + 1);
  }, "Generating level..");
}

void HudLayer::showMenu2(Ref *)
{
}

}
