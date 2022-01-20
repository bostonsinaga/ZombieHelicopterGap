#include "TutorialScene.h"
#include "GameScene.h"

USING_NS_CC;

Node *screenShot_ts;
int tutorialTag;

Scene* TutorialScene::createScene(cocos2d::RenderTexture *renTex, int tutorialTag_in) {

    tutorialTag = tutorialTag_in;
    screenShot_ts = renTex;

    auto scene = Scene::create();
    auto layer = TutorialScene::create();
    scene->addChild(layer);
    return scene;
}

// on "init" you need to initialize your instance
bool TutorialScene::init() {

    //////////////////////////////
    // 1. super init first
    if (!Layer::init()) {

        return false;
    }

    screenShot_ts->setPosition(tool.ctrPos());
    this->addChild(screenShot_ts);

    ////////////// stuff /////////////////

    // dark screen //

    darkScreen = DrawNode::create();
    darkScreen->setPosition(tool.ctrPos());
    darkScreen->drawSolidRect(

        Vec2(-tool.getVSz().width / 2, -tool.getVSz().height / 2),
        Vec2(tool.getVSz().width / 2, tool.getVSz().height / 2),
        Color4F(0, 0, 0, 100)
    );
    this->addChild(darkScreen);

    // label
    label = tool.myLabel("");
    label->enableBold();
    label->setOpacity(100);
    this->addChild(label);

    // finger
    finger = tool.mySprite("GUI/finger_print.png");
    finger->setScale(0.5);
    finger->setOpacity(100);
    this->addChild(finger);

    ///////////////// directing section /////////////////

    if (tutorialTag == 1) {
        
        label->setString("TAP TO FLY");
        label->setPosition(tool.pos(0.25, 0.7));
        finger->setPosition(tool.pos(0.25, 0.4));
    }
    else if (tutorialTag == 2) {
        
        label->setString("TAP TO SHOOT");
        label->setPosition(tool.pos(0.75, 0.7));
        finger->setPosition(tool.pos(0.75, 0.4));
    }
    else {
        
        label->setString("CONSIDER!");
        label->setPosition(tool.ctrPos() + Vec2(0, tool.getBSz().height * 4));
        finger->setVisible(false);
        generateVisualTut();
    }

    if (tutorialTag <= 2) {
        
        actionFinger(0.0);
        this->schedule(static_cast<SEL_SCHEDULE>(&TutorialScene::actionFinger), 1.0f);
    }

    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&TutorialScene::passNotice),
        tutorialTag == 3 ? 3.0f : (tutorialTag == 4 ? 4.0f : 1.0f));

    delayTouch(tutorialTag == 3 ? 3.01 : (tutorialTag == 4 ? 4.01 : 1.01));

    actionLabel(0.0);
    this->schedule(static_cast<SEL_SCHEDULE>(&TutorialScene::actionLabel), 1.0f);

    // touch listener
    touching();
    onKeyReleasing();

    return true;
}

void TutorialScene::actionLabel(float dt) {

    label->runAction(Sequence::create(
        
        FadeTo::create(0.5, 255),
        FadeTo::create(0.5, 100),
        nullptr
    ));
}

void TutorialScene::actionFinger(float dt) {
        
    finger->runAction(Sequence::create(
        
        Spawn::create(
            
            ScaleTo::create(0.5, 1.0),
            FadeTo::create(0.5, 255),
            nullptr
        ),
        Spawn::create(
            
            ScaleTo::create(0.5, 0.5),
            FadeTo::create(0.5, 100),
            nullptr
        ),
        nullptr
    ));
}

void TutorialScene::passNotice(float dt) {

    passLabel = tool.myLabel("OK!", tool.getBSz().height * 3/2);
    passLabel->setPosition(tool.ctrPos() - Vec2(tutorialTag >= 3 ? Vec2(0, tool.getBSz().height * 4.5) : Vec2::ZERO));
    passLabel->setScale(0.875);
    passLabel->setOpacity(100);
    this->addChild(passLabel);

    passAction(0.0);
    this->schedule(static_cast<SEL_SCHEDULE>(&TutorialScene::passAction), 1.0f);
}

void TutorialScene::passAction(float dt) {

    passLabel->runAction(Sequence::create(

        Spawn::create (
            
            ScaleTo::create(0.5, 1.0),
            FadeTo::create(0.5, 255),
            nullptr
        ),
        Spawn::create (
            
            ScaleTo::create(0.5, 0.875),
            FadeTo::create(0.5, 100),
            nullptr
        ),
        nullptr
    ));
}

void TutorialScene::generateVisualTut() {

    // gui //

    gui.init(this, &tool, nullptr, false);
    gui.generate("tutorial");

    // heli body //

    Vec2 heliPos = tool.ctrPos() + Vec2(
        
        tutorialTag == 4 ? 0 : tool.getBSz().width * -3,
        tutorialTag == 4 ? tool.getBSz().height * 1.5 : 0
    );

    heliTut = tool.mySprite("HELICOPTER/heli_body_1.png");
    heliTut->setPosition(heliPos);
    this->addChild(heliTut, 1);

    // main fan //

    heliMainFanTut = tool.mySprite("HELICOPTER/heli_main_fan_1.png");
    heliMainFanTut->setPosition(
        
        Vec2(heliTut->getContentSize().width * 0.682, heliTut->getContentSize().height * 0.928) +
        Vec2(0, heliMainFanTut->getContentSize().height / 2) + Director::getInstance()->getVisibleOrigin()
    );
    heliTut->addChild(heliMainFanTut);

    heliMainFanTut->runAction(RepeatForever::create(
        
        Sequence::create(

            ScaleTo::create(0.1, 0.6, 1.0),
            ScaleTo::create(0.1, 0.3, 1.0),
            ScaleTo::create(0.1, 0.6, 1.0),
            ScaleTo::create(0.1, 1.0, 1.0),
            nullptr
        )
    ));

    // tail fan //

    for (int i = 0; i < 2; i++) {

        float rotation = i == 0 ? 0.0 : 90;
    
        heliTailFanTut[i] = tool.mySprite("HELICOPTER/heli_tail_fan_1.png");
        heliTailFanTut[i]->setPosition(
            
            Vec2(heliTut->getContentSize().width * 0.061, heliTut->getContentSize().height * 0.713) +
            Director::getInstance()->getVisibleOrigin()
        );
        heliTailFanTut[i]->setRotation(rotation);
        heliTut->addChild(heliTailFanTut[i]);

        heliTailFanTut[i]->runAction(RepeatForever::create(RotateBy::create(0.1, 45.0)));
    }

    // targets //

    victimTut = tool.mySprite("GENERAL/victim_sos.png");
    victimTut->setPosition(tool.ctrPos() - (tutorialTag == 4 ?

        Vec2(heliTut->getContentSize().width / -5, tool.getBSz().height * 1.5):
        Vec2(tool.getBSz().width * -3, heliTut->getContentSize().height / 4)
    ));
    this->addChild(victimTut);

    zombieTut = tool.mySprite("GENERAL/zombie_walk.png");
    zombieTut->setPosition(victimTut->getPosition());
    zombieTut->setVisible(false);
    this->addChild(zombieTut);

    // actions //

    Vec2 bulletPos;
    if (tutorialTag == 3) {
        
        bulletPos = heliTut->getPosition() + Vec2(
            
            heliTut->getContentSize().width / 4,
            -heliTut->getContentSize().height / 4
        );
        
        Size bulletSize = Size(tool.getBSz().width / 4, tool.getBSz().height / 8);        

        bullet = DrawNode::create();
        bullet->drawSolidRect(-Vec2(bulletSize / 2), Vec2(bulletSize / 2), Color4F::YELLOW);
        bullet->setPosition(bulletPos);
        this->addChild(bullet, 1);
    }

    trueSign = tool.mySprite("GUI/true.png");
    trueSign->setPosition(tool.ctrPos());
    trueSign->setVisible(false);
    this->addChild(trueSign, 1);

    falseSign = tool.mySprite("GUI/false.png");
    falseSign->setPosition(trueSign->getPosition());
    falseSign->setVisible(false);
    this->addChild(falseSign, 1);

    auto func = [=]() {

        if (isVictim) {
            
            isVictim = false;
            victimTut->setVisible(false);
            zombieTut->setVisible(true);
        }
        else {
            
            isVictim = true;
            victimTut->setVisible(true);
            zombieTut->setVisible(false);
        }

        trueSign->setVisible(false);
        falseSign->setVisible(false);
    };

    if (tutorialTag == 3) {
        
        bullet->runAction(RepeatForever::create(Sequence::create(
        
            MoveTo::create(0.5, victimTut->getPosition()),
            FadeOut::create(0),
            CallFunc::create([=]() {

                if (isVictim) {

                    trueSign->setVisible(false);
                    falseSign->setVisible(true);
                }
                else {

                    trueSign->setVisible(true);
                    falseSign->setVisible(false);
                }
            }),
            DelayTime::create(0.75),
            CallFunc::create([=]() {func();}),
            DelayTime::create(0.25),
            MoveTo::create(0, bulletPos),
            FadeIn::create(0),
            nullptr
        )));
    }
    else if (tutorialTag == 4) {

        heliTut->runAction(RepeatForever::create(Sequence::create(

            MoveBy::create(1.0, Vec2(0, tool.getBSz().height * -3)),
            CallFunc::create([=]() {

                if (isVictim) {

                    trueSign->setVisible(true);
                    falseSign->setVisible(false);
                }
                else {

                    trueSign->setVisible(false);
                    falseSign->setVisible(true);
                }
            }),
            DelayTime::create(0.75),
            CallFunc::create([=]() {func();}),
            DelayTime::create(0.25),
            MoveTo::create(0, heliPos),
            nullptr
        )));
    }
}

void TutorialScene::removeVisualTut() {

    gui.remove("tutorial");
    heliTut->runAction(RemoveSelf::create());
    heliMainFanTut->runAction(RemoveSelf::create());
    heliTailFanTut[0]->runAction(RemoveSelf::create());
    heliTailFanTut[1]->runAction(RemoveSelf::create());
    victimTut->runAction(RemoveSelf::create());
    zombieTut->runAction(RemoveSelf::create());
    if (tutorialTag == 3) bullet->runAction(RemoveSelf::create());
    trueSign->runAction(RemoveSelf::create());
    falseSign->runAction(RemoveSelf::create());
}

void TutorialScene::touching() {

    touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = [=] (Touch *touch, Event *event) {

        return true;
    };
    touchListener->onTouchEnded = [=] (Touch *touch, Event *event) {

        if (isTouchable) {
            
            _eventDispatcher->removeEventListener(touchListener);
            _eventDispatcher->removeEventListener(backButtonListener);
            popGameScene();
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void TutorialScene::popGameScene() {

    int recoveredTime = tool.getUDef()->getIntegerForKey("START TIME") + std::time(0) - pausedStartTime;
    tool.setUserValue("START TIME", recoveredTime);

    finger->runAction(RemoveSelf::create());
    label->runAction(RemoveSelf::create());

    this->unschedule(static_cast<SEL_SCHEDULE>(&TutorialScene::actionLabel));
    this->unschedule(static_cast<SEL_SCHEDULE>(&TutorialScene::passAction));

    if (tutorialTag <= 2) this->unschedule(static_cast<SEL_SCHEDULE>(&TutorialScene::actionFinger));
    else {
        
        removeVisualTut();
        if (tutorialTag == 4 && tool.getUDef()->getBoolForKey("SOUND ON")) tool.playFanSound(true);
    }

    passLabel->runAction(RemoveSelf::create());
    darkScreen->runAction(RemoveSelf::create());
    screenShot_ts->runAction(RemoveSelf::create());

    Director::getInstance()->popScene();
}

// touch and contact pacifier //

void TutorialScene::delayTouch(float dt) {

    isTouchable = false;
    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&TutorialScene::set_isTouchable), dt);
}

void TutorialScene::set_isTouchable(float dt) {isTouchable = true;}

// back button ANDROID
void TutorialScene::onKeyReleasing() {

    backButtonListener = EventListenerKeyboard::create();
    backButtonListener->onKeyReleased = [=](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {

        if (keyCode == EventKeyboard::KeyCode::KEY_BACK) {
                
            if (isTouchable) {
                
                _eventDispatcher->removeEventListener(touchListener);
                _eventDispatcher->removeEventListener(backButtonListener);
                popGameScene();
            }
            tool.playSFX("select on");
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(backButtonListener, this);
}