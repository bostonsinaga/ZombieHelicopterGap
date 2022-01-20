#include "SplashScene.h"
#include "TitleScene.h"
#include "AudioEngine.h"

USING_NS_CC;

Scene* SplashScene::createScene() {

    auto scene = Scene::create();
    auto layer = SplashScene::create();
    scene->addChild(layer);
    return scene;
}

// on "init" you need to initialize your instance
bool SplashScene::init() {

    //////////////////////////////
    // 1. super init first
    if (!LayerColor::initWithColor(Color4B::WHITE)) {

        return false;
    }

    AudioEngine::preload("SOUNDS/zombies_horde.mp3");
    AudioEngine::preload("SOUNDS/bullet_fire.mp3");
    AudioEngine::preload("SOUNDS/bullet_receive.mp3");
    AudioEngine::preload("SOUNDS/carnage.mp3");
    AudioEngine::preload("SOUNDS/change_weapon.mp3");
    AudioEngine::preload("SOUNDS/coin.mp3");
    AudioEngine::preload("SOUNDS/explode.mp3");
    AudioEngine::preload("SOUNDS/fan.mp3");
    AudioEngine::preload("SOUNDS/frict.mp3");
    AudioEngine::preload("SOUNDS/missile_launch.mp3");
    AudioEngine::preload("SOUNDS/new_high_score.mp3");
    AudioEngine::preload("SOUNDS/notice.mp3");
    AudioEngine::preload("SOUNDS/pick.mp3");
    AudioEngine::preload("SOUNDS/punishment.mp3");
    AudioEngine::preload("SOUNDS/purchased.mp3");
    AudioEngine::preload("SOUNDS/select_heli.mp3");
    AudioEngine::preload("SOUNDS/select_off.mp3");
    AudioEngine::preload("SOUNDS/select_on.mp3");

    // data //

    tool.setUserValue("MUSIC ON", true);
    tool.setUserValue("SOUND ON", true);

    // view //

    auto splashScreen = tool.mySprite("BACKGROUND/developer_logo.png");
    splashScreen->setPosition(tool.ctrPos());
    this->addChild(splashScreen);

    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&SplashScene::goToTitleScene), 2.0f);

    return true;
}

void SplashScene::goToTitleScene(float dt) {

    tool.setUserValue("ID BG MUSIC", AudioEngine::play2d("SOUNDS/zombies_horde.mp3", true, 1.0f));
    auto scene = TitleScene::createScene();
    Director::getInstance()->replaceScene(TransitionCrossFade::create(0.25, scene));
}