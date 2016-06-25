#include "hud_layer.h"
#include <butcher.h>
#include <dungeon/tmx_builder.h>
#include <actors/player.h>
#include <utils/utils.h>
#include <dungeon/dungeon_state.h>
#include <view/loading_scene.h>
#include <dungeon/dungeon_generator.h>
#include <view/hud_log.h>

namespace cc = cocos2d;

namespace butcher {

HudLayer::HudLayer()
  : _log(nullptr)
  , _expBar(nullptr)
  , _hpBar(nullptr)
  , _hpValue(nullptr)
  , _lvValue(nullptr)
  , _dungeonLevelLabel(nullptr)
{
}

HudLayer::~HudLayer()
{
}

bool HudLayer::init()
{
  if (Layer::init())
  {
    removeAllChildren();

    cc::Vec2 origin = cc::Director::getInstance()->getVisibleOrigin();
    auto visibleSize = cc::Director::getInstance()->getVisibleSize();
    int margin = 10;

    auto menuBtn = cc::MenuItemImage::create("button/menu.png",
                                             "button/menu_click.png",
                                             CC_CALLBACK_1(HudLayer::showMenu, this));
    menuBtn->setAnchorPoint(cc::Vec2(0,0));
    menuBtn->setPosition(cc::Vec2(origin.x + visibleSize.width - menuBtn->getBoundingBox().size.width - margin, origin.y + margin));

    auto menu = cc::Menu::create(menuBtn, NULL);
    menu->setPosition(cc::Vec2::ZERO);
    this->addChild(menu, 1);

    _log = new HudLog;
    addChild(_log, 1);

    initHpBar();
    initExpBar();
    initDungeonLevelCounter();

    onNotify(BUTCHER.getPlayer().get());
  }

  return true;
}

void HudLayer::initExpBar()
{
  int margin = 10;
  cc::Vec2 origin = cc::Director::getInstance()->getVisibleOrigin();
  auto visibleSize = cc::Director::getInstance()->getVisibleSize();

  cc::Sprite* exp_glyph = cc::Sprite::create();
  exp_glyph->initWithFile("images/arrow_up.png");
  exp_glyph->setAnchorPoint(cc::Vec2(1,1));
  exp_glyph->setPosition(cc::Vec2(origin.x + visibleSize.width - margin,
                              origin.y + visibleSize.height - margin -
                              _hpBar->getBoundingBox().size.height*1.2));
  exp_glyph->setLocalZOrder(1);
  addChild(exp_glyph);

  _expBar = cc::ui::LoadingBar::create();
  _expBar->loadTexture("images/progress_bar_cyan.png");
  _expBar->setPercent(100);
  _expBar->setAnchorPoint(cc::Vec2(1,1));
  _expBar->setPosition(cc::Vec2(exp_glyph->getPositionX() - exp_glyph->getBoundingBox().size.width*1.2,
                                exp_glyph->getPositionY()));
  _expBar->setLocalZOrder(2);
  addChild(_expBar);

  cc::Sprite* barBg = cc::Sprite::create();
  barBg->initWithFile("images/progress_bar_bg.png");
  barBg->setAnchorPoint(cc::Vec2(1,1));
  barBg->setPosition(_expBar->getPosition());
  barBg->setLocalZOrder(1);
  addChild(barBg);

  _lvValue = make_label("", cc::Color4B::WHITE, 14);
  _lvValue->setPosition( barBg->getPositionX() - barBg->getBoundingBox().size.width / 2,
                      barBg->getPositionY() - barBg->getBoundingBox().size.height / 2);

  addChild(_lvValue, 3);
}

void HudLayer::initHpBar()
{
  int margin = 10;
  cc::Vec2 origin = cc::Director::getInstance()->getVisibleOrigin();
  auto visibleSize = cc::Director::getInstance()->getVisibleSize();

  cc::Sprite* hp_glyph = cc::Sprite::create();
  hp_glyph->initWithFile("images/heart.png");
  hp_glyph->setAnchorPoint(cc::Vec2(1,1));
  hp_glyph->setPosition(cc::Vec2(origin.x + visibleSize.width - margin,
                              origin.y + visibleSize.height - margin));
  hp_glyph->setLocalZOrder(1);
  addChild(hp_glyph);

  _hpBar = cc::ui::LoadingBar::create();
  _hpBar->loadTexture("images/progress_bar_red.png");
  _hpBar->setPercent(100);
  _hpBar->setAnchorPoint(cc::Vec2(1,1));
  _hpBar->setPosition(cc::Vec2(hp_glyph->getPositionX() - hp_glyph->getBoundingBox().size.width*1.2,
                                hp_glyph->getPositionY()));
  addChild(_hpBar, 2);

  cc::Sprite* barBg = cc::Sprite::create();
  barBg->initWithFile("images/progress_bar_bg.png");
  barBg->setAnchorPoint(cc::Vec2(1,1));
  barBg->setPosition(_hpBar->getPosition());
  addChild(barBg,1 );

  _hpValue = make_label("", cc::Color4B::WHITE, 14);
  _hpValue->setPosition( barBg->getPositionX() - barBg->getBoundingBox().size.width / 2,
                      barBg->getPositionY() - barBg->getBoundingBox().size.height / 2);

  addChild(_hpValue, 3);
}

void HudLayer::initDungeonLevelCounter()
{
  int margin = 10;
  cc::Vec2 origin = cc::Director::getInstance()->getVisibleOrigin();
  auto visibleSize = cc::Director::getInstance()->getVisibleSize();

  cc::Sprite* stairs_glyph = cc::Sprite::create();
  stairs_glyph->initWithFile("images/stairs_up_glyph.png");

  stairs_glyph->setAnchorPoint(cc::Vec2(1,1));
  stairs_glyph->setPosition(cc::Vec2(origin.x + visibleSize.width - margin,
                                     _expBar->getPositionY() - _expBar->getBoundingBox().size.height*1.2));
  stairs_glyph->setLocalZOrder(1);
  addChild(stairs_glyph);

  _dungeonLevelLabel = make_label( toStr(BUTCHER.getDungeonLevel()),
                                   cc::Color4B(65,140,150,255),
                                   24,
                                   cc::Vec2(1.1,1.1) );

  _dungeonLevelLabel->setPosition(cc::Vec2(stairs_glyph->getPositionX() - stairs_glyph->getBoundingBox().size.width*1.2,
                                           stairs_glyph->getPositionY()));

  addChild(_dungeonLevelLabel, 1);
}

void HudLayer::print(const std::string &str, cc::Color4B color)
{
  cc::Vec2 origin = cc::Director::getInstance()->getVisibleOrigin();
  auto visibleSize = cc::Director::getInstance()->getVisibleSize();

  _log->addMessage(str, color);

  _log->setAnchorPoint( cc::Vec2(0, 0) );
  _log->setPosition( origin.x + 16, origin.y + visibleSize.height - 16 );
}

void HudLayer::onNotify(Subject *subject)
{
  Player* player = dynamic_cast<Player*>(subject);
  if ( player )
  {
    if ( _expBar )
    {
      _expBar->setPercent((float)player->getExp() / (float)player->getExpForNextLevel() * 100);
      _lvValue->setString( "Level " + toStr(player->getLevel()) );
      //cc::log("%d %d", player->getExp(), player->getExpForNextLevel());
    }

    if ( _hpBar )
    {
      _hpBar->setPercent((float)player->getHp() / (float)player->getMaxHp() * 100);
      _hpValue->setString( toStr(player->getHp()) + "/" + toStr(player->getMaxHp()));
      //cc::log("hpBar%d %d", player->getHp(), player->getMaxHp());
    }
  }
}

void HudLayer::showMenu(Ref *)
{
  LoadingScreen::run([&](){
    BUTCHER.goToLevel(BUTCHER.getDungeonLevel() + 1);
  }, "Generating level..");
}

}
