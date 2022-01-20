#include "PauseScene.h"
#include "GameScene.h"

USING_NS_CC;

Node *screenShot_ps;

Scene* PauseScene::createScene(cocos2d::RenderTexture *renTex) {

    screenShot_ps = renTex;

    auto scene = Scene::create();
    auto layer = PauseScene::create();
    scene->addChild(layer);
    return scene;
}

// on "init" you need to initialize your instance
bool PauseScene::init() {

    //////////////////////////////
    // 1. super init first
    if (!Layer::init()) {

        return false;
    }

    onKeyReleasing();
    delayTouch(0.1);
    touching();

    gui.init(this, &tool, nullptr, false);
    gui.generate("paused");
    gui.showNoticeBoard(true, "paused");

    screenShot_ps->setPosition(tool.ctrPos());
    this->addChild(screenShot_ps);

    return true;
}

void PauseScene::touching() {

    touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = [=] (Touch *touch, Event *event) {

        if (isTouchable) {
            
            touchBegan = Vec2(-1, -1);

            if (isTouched == false) {

                isTouched = true;
                touchBegan = touch->getLocation();

                // touch began //

                std::string str = gui.buttonDetection('b', touchBegan);
                gui.press(str, 'b');

                // touch began //
            }
        }

        return true;
    };
    touchListener->onTouchEnded = [=] (Touch *touch, Event *event) {

        if (isTouchable) {
            
            isTouched = false;
            touchEnded = touch->getLocation();

            // touch ended //

            std::string str = gui.buttonDetection('e', touchBegan, touchEnded);
            gui.press(str, 'e');

            if (str == "paused resume") {

                prepareToPopGameScene();
            }
            else if (str == "paused quit") {

                gui_noticeBoardChange("paused", "quit", 0.1);
            }
            else if (str == "paused music") {

                bool isOn = tool.getUDef()->getBoolForKey("MUSIC ON");
                tool.setUserValue("MUSIC ON", !isOn);
                gui_noticeBoardChange("paused", "paused", 0.1);
                tool.playBGMusic(!isOn);
                if (isOn) tool.playSFX("select off");
                else tool.playSFX("select on");
            }
            else if (str == "paused sound") {

                bool isOn = tool.getUDef()->getBoolForKey("SOUND ON");
                tool.setUserValue("SOUND ON", !isOn);
                gui_noticeBoardChange("paused", "paused", 0.1);
                if (isOn == false) tool.playSFX("select on");
            }
            else if (str == "quit yes") {

                screenShot_ps->runAction(RemoveSelf::create());
                auto scene = GameScene::createScene();
                Director::getInstance()->replaceScene(TransitionFade::create(0.25, scene));
            }
            else if (str == "quit no") {

                gui_noticeBoardChange("quit", "paused", 0.1);
            }

            // touch ended //

            touchBegan = Vec2(-1, -1);
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

// gui //

void PauseScene::gui_noticeBoardChange(
    
    std::string designationBefore,
    std::string designationNext,
    float dt
) {

    gui_noticeBoardDesignation = designationNext;
    gui.showNoticeBoard(false, designationBefore);

    delayTouch(dt);
    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&PauseScene::gui_showNoticeBoard), dt);
}

void PauseScene::gui_showNoticeBoard(float dt) {gui.showNoticeBoard(true, gui_noticeBoardDesignation);}

// pop game scene //

void PauseScene::prepareToPopGameScene() {

    isTouchable = false;
    isPreparedPop = true;

    _eventDispatcher->removeEventListener(touchListener);
    _eventDispatcher->removeEventListener(backButtonListener);

    gui.showNoticeBoard(false, "paused");

    auto countLabel = tool.myLabel("3");
    countLabel->setPosition(tool.ctrPos());
    countLabel->enableOutline(Color4B(0, 127, 255, 127), 3);
    this->addChild(countLabel, 1);

    auto effectAct = Spawn::create(
            
        ScaleTo::create(0.75, 1.5),
        Sequence::create(
            
            DelayTime::create(0.375),
            FadeOut::create(0.375),
            nullptr
        ),
        nullptr
    );

    countLabel->runAction(Sequence::create(

        effectAct,
        CallFunc::create([=]() {

            countLabel->setScale(1.0);
            countLabel->setString(std::to_string(2));
            countLabel->setOpacity(255);
        }),
        effectAct,
        CallFunc::create([=]() {

            gui.remove("paused");
            countLabel->setScale(1.0);
            countLabel->setString(std::to_string(1));
            countLabel->setOpacity(255);
        }),
        effectAct,
        RemoveSelf::create(),
        nullptr
    ));

    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&PauseScene::popGameScene), 2.3f);
}

void PauseScene::popGameScene(float dt) {

    int recoveredTime = tool.getUDef()->getIntegerForKey("START TIME") + std::time(0) - pausedStartTime;
    tool.setUserValue("START TIME", recoveredTime);

    if (tool.getUDef()->getBoolForKey("SOUND ON")) tool.playFanSound(true);

    screenShot_ps->runAction(RemoveSelf::create());
    Director::getInstance()->popScene();
}

// touch and contact pacifier //

void PauseScene::delayTouch(float dt) {

    isTouchable = false;
    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&PauseScene::set_isTouchable), dt);
}

void PauseScene::set_isTouchable(float dt) {isTouchable = true;}

// back button ANDROID
void PauseScene::onKeyReleasing() {

    backButtonListener = EventListenerKeyboard::create();
    backButtonListener->onKeyReleased = [=](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {

        if (keyCode == EventKeyboard::KeyCode::KEY_BACK) {
                
            if (gui.getNoticeDesignation() == "quit") gui_noticeBoardChange("quit", "paused", 0.1);
            else if (isPreparedPop == false) prepareToPopGameScene();

            tool.playSFX("select on");
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(backButtonListener, this);
}