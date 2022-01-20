#include "GameOverScene.h"
#include "GameScene.h"

USING_NS_CC;

Node *screenShot_gos;

Scene* GameOverScene::createScene(cocos2d::RenderTexture *renTex) {

    screenShot_gos = renTex;

    auto scene = Scene::create();
    auto layer = GameOverScene::create();
    scene->addChild(layer);
    return scene;
}

// on "init" you need to initialize your instance
bool GameOverScene::init() {

    //////////////////////////////
    // 1. super init first
    if (!Layer::init()) {

        return false;
    }

    gui.init(this, &tool, nullptr, false);
    gui.generate("game over");
    gui.showNoticeBoard(true, "game over");

    screenShot_gos->setPosition(tool.ctrPos());
    this->addChild(screenShot_gos);

    float timeToLeave = gui.gameOverSequence(
        
        tool.getUDef()->getIntegerForKey("CURRENT COINS"),
        tool.getUDef()->getIntegerForKey("CURRENT SCORE")
    );
    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameOverScene::goToMainMenuScene), timeToLeave);
    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameOverScene::set_isAvailableToLeave), timeToLeave / 2);

    touching();
    onKeyReleasing();

    return true;
}

void GameOverScene::touching() {

    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = [=] (Touch *touch, Event *event) {
            
        touchBegan = Vec2(-1, -1);

        if (isTouched == false) {

            isTouched = true;
            touchBegan = touch->getLocation();

            // touch began //

            std::string str = gui.buttonDetection('b', touchBegan);
            gui.press(str, 'b');

            // touch began //
        }

        return true;
    };
    touchListener->onTouchEnded = [=] (Touch *touch, Event *event) {

        isTouched = false;
        touchEnded = touch->getLocation();

        // touch ended //

        std::string str = gui.buttonDetection('e', touchBegan, touchEnded);
        gui.press(str, 'e');

        if (str == "home") {

            this->unschedule(static_cast<SEL_SCHEDULE>(&GameOverScene::goToMainMenuScene));
            goToMainMenuScene(0.0);
        }

        // touch ended //

        touchBegan = Vec2(-1, -1);
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

// main menu //

void GameOverScene::goToMainMenuScene(float dt) {

    tool.setUserValue(

        "TOTAL COINS",
        tool.getUDef()->getIntegerForKey("TOTAL COINS", 0) +
            tool.getUDef()->getIntegerForKey("CURRENT COINS")
    );

    int currentScore = tool.getUDef()->getIntegerForKey("CURRENT SCORE");

    if (currentScore > tool.getUDef()->getIntegerForKey("TOTAL SCORE", 0))
        tool.setUserValue("TOTAL SCORE", currentScore);

    if (tool.getUDef()->getBoolForKey("IS TUTORIAL NEEDED 2", true) && currentScore > 300)
        tool.setUserValue("IS TUTORIAL NEEDED 2", false);

    screenShot_gos->runAction(RemoveSelf::create());

    auto scene = GameScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(0.25, scene));
}

void GameOverScene::set_isAvailableToLeave(float dt) {isAvailableToLeave = true;}

// back button ANDROID
void GameOverScene::onKeyReleasing() {

    auto backButtonListener = EventListenerKeyboard::create();
    backButtonListener->onKeyReleased = [=](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {

        if (keyCode == EventKeyboard::KeyCode::KEY_BACK) {
                
            if (isAvailableToLeave) goToMainMenuScene(0.0);
            tool.playSFX("select off");
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(backButtonListener, this);
}