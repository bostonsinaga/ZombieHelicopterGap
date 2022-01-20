#include "GameScene.h"
#include "GameOverScene.h"
#include "PauseScene.h"
#include "TutorialScene.h"

USING_NS_CC;

Scene* GameScene::createScene() {

    auto scene = Scene::createWithPhysics();
    scene->getPhysicsWorld()->setGravity(Vec2::ZERO);
    // scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL); // debug only

    auto layer = GameScene::create();
    layer->setPhysicsWorld(scene->getPhysicsWorld());
    scene->addChild(layer);

    return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init() {

    //////////////////////////////
    // 1. super init first
    if (!LayerGradient::initWithColor(Color4B(225, 180, 90, 255), Color4B(85, 75, 130, 255), Vec2(0, -1))) {

        return false;
    }

    //// inits ////

    tool.setUserValue("CURRENT COINS", 0);
    tool.setUserValue("CURRENT SCORE", 0);

    heli.init(this, &seed, &tool);
    obs.init(this, &seed, &tool, &heli);

    gui.init(this, &tool, &heli, true);
    gui.generate("main menu");

    // check tutorial
    if (tool.getUDef()->getBoolForKey("IS TUTORIAL NEEDED 1", true)
        || tool.getUDef()->getBoolForKey("IS TUTORIAL NEEDED 2", true)) isTutoring = true;
    else isTutoring = false;

    // listeners
    delayTouch(0.1);
    touching();
    contacting();
    onKeyReleasing();

    // schedule
    this->schedule(static_cast<SEL_SCHEDULE>(&GameScene::obs_spawnBG), 1.75f);
    
    return true;
}

// obs //

void GameScene::obs_spawn(float dt) {obs.spawn(true);}
void GameScene::obs_spawnBG(float dt) {obs.spawn(false);}

// heli //

void GameScene::heli_fire(float dt) {heli.fire("heli");}
void GameScene::heli_punishmentMissile(float dt) {heli.fire("pinalty");}

void GameScene::setHeliStatus(int statusAsInteger) {

    /*
        EXPLANATION 'statusAsInteger': 
        0 = victim safed
        1 = victim killed
        2 = zombie intered
    */

    int *a = &heliStatus[0], *b = &heliStatus[1], *c = &heliStatus[2], *d = &statusAsInteger;

    if (*a == *d && *b != *d && *c != *d) {

        *c = *b;
        *b = *d;
    }
    else if (*a == *d && *b != *d && *c == *d) {

        *b = *d;
    }
    else if (*a != *d && *b == *d && *c == *d) {

        *a = *d;
    }
    else if (*a == *d && *b == *d && *c != *d) {

        *c = *d;
    }
    else {

        *a = *b;
        *b = *c;
        *c = *d;
    }
}

// gui //

void GameScene::gui_noticeBoardChange(
            
    std::string designationBefore,
    std::string designationNext,
    float dt
) {

    gui_noticeBoardDesignation = designationNext;
    gui.showNoticeBoard(false, designationBefore);

    delayTouch(dt);
    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::gui_showNoticeBoard), dt);
}

void GameScene::gui_showNoticeBoard(float dt) {gui.showNoticeBoard(true, gui_noticeBoardDesignation);}

// scene //

void GameScene::startGame() {

    tool.setUserValue("START TIME", int(std::time(0)));
    isGameStarted = true;
    
    heli.attachPhysicsBody();

    heliTotalHP = heli.getHP();

    gui.heli_HPBar_update(heliTotalHP, heliTotalHP);
    gui.heli_PinaltyBar_update(heliStatus);

    this->schedule(static_cast<SEL_SCHEDULE>(&GameScene::obs_spawn), 1.0f);
    this->schedule(static_cast<SEL_SCHEDULE>(&GameScene::scoreAddition), 0.1f);

    startGameTouchListener();
    checkingTutorialNeeds();
}

void GameScene::checkingTutorialNeeds() {

    if (tool.getUDef()->getBoolForKey("IS TUTORIAL NEEDED 1", true)) {

        this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::tut_tut1), 0.5f);
        this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::tut_tut2), 1.0f);
        this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::tut_tut3), 1.5f);
        this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::tut_tut4), 2.0f);
    }
    else if (tool.getUDef()->getBoolForKey("IS TUTORIAL NEEDED 2", true)) {

        this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::tut_tut3), 0.5f);
        this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::tut_tut4), 1.0f);
    }
}

void GameScene::tut_tut1(float dt) {

    heli.fall();
    this->unschedule(static_cast<SEL_SCHEDULE>(&GameScene::heli_fire));

    if (tool.getUDef()->getBoolForKey("SOUND ON")) tool.playFanSound(false);

    auto scene = TutorialScene::createScene(getRenTex(), 1);
    Director::getInstance()->pushScene(scene);
    scene->runAction(RemoveSelf::create());
}

void GameScene::tut_tut2(float dt) {

    heli.fall();
    this->unschedule(static_cast<SEL_SCHEDULE>(&GameScene::heli_fire));

    auto scene = TutorialScene::createScene(getRenTex(), 2);
    Director::getInstance()->pushScene(scene);
    scene->runAction(RemoveSelf::create());

    tool.setUserValue("IS TUTORIAL NEEDED 1", false);
}

void GameScene::tut_tut3(float dt) {

    heli.fall();
    this->unschedule(static_cast<SEL_SCHEDULE>(&GameScene::heli_fire));

    if (tool.getUDef()->getBoolForKey("SOUND ON")) tool.playFanSound(false);

    auto scene = TutorialScene::createScene(getRenTex(), 3);
    Director::getInstance()->pushScene(scene);
    scene->runAction(RemoveSelf::create());
}

void GameScene::tut_tut4(float dt) {

    isTutoring = false;
    heli.fall();
    this->unschedule(static_cast<SEL_SCHEDULE>(&GameScene::heli_fire));

    auto scene = TutorialScene::createScene(getRenTex(), 4);
    Director::getInstance()->pushScene(scene);
    scene->runAction(RemoveSelf::create());
}

void GameScene::goToPauseScene(float dt) {

    if (tool.getUDef()->getBoolForKey("SOUND ON")) tool.playFanSound(false);
    auto scene = PauseScene::createScene(getRenTex());
    Director::getInstance()->pushScene(scene);
}

void GameScene::goToGameOverScene(float dt) {

    gui.remove("game");
    auto scene = GameOverScene::createScene(getRenTex());
    Director::getInstance()->pushScene(scene);
}

cocos2d::RenderTexture *GameScene::getRenTex() {

    auto renTex = RenderTexture::create(tool.getVSz().width, tool.getVSz().height);
    renTex->setPosition(tool.ctrPos());

    renTex->begin();
    this->getParent()->visit();
    renTex->end();

    return renTex;
}

// coin //

void GameScene::coinCollect(cocos2d::Vec2 pos) {

    int currentCoins = tool.getUDef()->getIntegerForKey("CURRENT COINS");
    tool.setUserValue("CURRENT COINS", currentCoins + 1);

    Size bSz = tool.getBSz() * 5/4;

    gui.coinCollectAnimation(pos);
    gui.setBoardString(
        
        0, std::to_string(currentCoins + 1),
        Size(bSz.width * 2/5, bSz.height * 2/3),
        tool.pos(1.0, 1.0) - gui.getBoardSize(std::to_string(currentCoins)) / 2 - bSz / 2
    );

    scoreAddition(20);
}

// scoring //

void GameScene::scoreAddition(int addition) {

    int currentScore = tool.getUDef()->getIntegerForKey("CURRENT SCORE");
    tool.setUserValue("CURRENT SCORE", currentScore + addition);

    Size bSz = tool.getBSz() * 5/4;
    
    gui.setBoardString(
            
        1, std::to_string(currentScore),
        Size(bSz.width * 2/5, bSz.height * 2/3),
        tool.pos(1.0, 1.0) - Vec2(gui.getBoardSize(std::to_string(currentScore)).width / 2,
            gui.getBoardSize(std::to_string(currentScore)).height * 7/4) - bSz / 2
    );
}

void GameScene::scoreAddition(float dt) {scoreAddition();}

//// EVENT LISTENERS ////

// touch event
void GameScene::touching() {

    if (isGameStarted) {

        touchListener_multi = EventListenerTouchAllAtOnce::create();
        touchListener_multi->onTouchesBegan = [=](const std::vector<Touch*> &touches, Event *event) {
                
            if (isTouchable) {
                
                touchBegan = touches.back()->getLocation();

                std::string str = gui.buttonDetection('b', touchBegan);
                if (isTutoring == false) gui.press(str, 'b');

                if (str != "pause" && isControlable) {
                    
                    if (touchBegan.x < tool.ctrPos().x) heli.fly();
                    else this->schedule(static_cast<SEL_SCHEDULE>(&GameScene::heli_fire), 0.1f);
                }
            }

            return true;
        };
        touchListener_multi->onTouchesEnded = [=](const std::vector<Touch*> &touches, Event *event) {
                
            if (isTouchable) {
                
                touchEnded = touches.back()->getLocation();

                std::string str = gui.buttonDetection('e', touchBegan, touchEnded);
                gui.press(str, 'e');

                if (str == "pause" && isTutoring == false) {

                    delayTouch(0.1);
                    this->unschedule(static_cast<SEL_SCHEDULE>(&GameScene::heli_fire));
                    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::goToPauseScene), 0.1f);
                }
                else if (touchEnded.x < tool.ctrPos().x) heli.fall();
                else this->unschedule(static_cast<SEL_SCHEDULE>(&GameScene::heli_fire));
            }
        };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener_multi, this);
    }
    else {

        touchListener_single = EventListenerTouchOneByOne::create();
        touchListener_single->setSwallowTouches(true);
        touchListener_single->onTouchBegan = [=](Touch *touch, Event *event) {

            if (isTouchable) {
                
                if (isTouched == false) {

                    isTouched = true;
                    touchBegan = touch->getLocation();

                    //// touch began ////

                    std::string str = gui.buttonDetection('b', touchBegan);
                    gui.press(str, 'b');

                    //// touch began ////
                }
            }

            return true;
        };
        touchListener_single->onTouchEnded = [=](Touch *touch, Event *event) {

            if (isTouchable) {
                
                isTouched = false;
                touchEnded = touch->getLocation();

                // exclusive for GUI
                auto pressedArrow = [=](int valSign) {

                    heli.swap(valSign);
                    gui.syncHeliPrice();
                    
                    if (gui.get_heli_isPurchased()) { // unhide
                                        
                        gui.hidePrebuildButton(false, 0); // play button
                        gui.hideBoard(false, 0); // play board
                    }
                    else { // hide
                        
                        gui.hidePrebuildButton(true, 0); // play button
                        gui.hideBoard(true, 0); // play board
                    }
                };

                //// touch ended ////

                std::string str = gui.buttonDetection('e', touchBegan, touchEnded);
                gui.press(str, 'e');

                if (str == "play") {

                    gui.remove("main menu");
                    gui.generate("game");
                    startGame();
                }
                else if (str == "arrow left") {

                    pressedArrow(-1);
                }
                else if (str == "arrow right") {

                    pressedArrow(1);
                }
                else if (str == "buy") {

                    gui.showNoticeBoard(true, "payment");
                }
                else if (str == "payment buy") {

                    int change = tool.getUDef()->getIntegerForKey("TOTAL COINS", 0) - gui.getHeliPrice();

                    if (change >= 0) {

                        gui_noticeBoardChange("payment", "heli purchased", 0.1);
                        tool.setUserValue("TOTAL COINS", change);
                        gui.set_heli_isPurchased();
                    }
                    else gui_noticeBoardChange("payment", "not enough", 0.1);
                }
                else if (str == "payment cancel") {

                    gui.showNoticeBoard(false, "payment");
                }
                else if (str == "exit yes") {

                    Director::getInstance()->end();
                }
                else if (str == "exit no") {

                    gui.showNoticeBoard(false, "exit");
                }
                else if (str == "not enough ok") {

                    gui.showNoticeBoard(false, "not enough");
                }
                else if (str == "heli purchased ok") {

                    gui.showNoticeBoard(false, "heli purchased");
                }
                else if (str == "settings") {

                    gui.showNoticeBoard(true, "settings");
                }
                else if (str == "settings music") {

                    bool isOn = tool.getUDef()->getBoolForKey("MUSIC ON");
                    tool.setUserValue("MUSIC ON", !tool.getUDef()->getBoolForKey("MUSIC ON"));
                    gui_noticeBoardChange("settings", "settings", 0.1);
                    tool.playBGMusic(!isOn);
                    if (isOn) tool.playSFX("select off");
                    else tool.playSFX("select on");
                }
                else if (str == "settings sound") {

                    bool isOn = tool.getUDef()->getBoolForKey("SOUND ON");
                    tool.setUserValue("SOUND ON", !isOn);
                    tool.playFanSound(!isOn);
                    gui_noticeBoardChange("settings", "settings", 0.1);
                    if (isOn == false) {
                        
                        tool.playSFX("select on");
                    }
                }

                //// touch ended ////

                touchBegan = Vec2(-1, -1);
            }
        };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener_single, this);
    }
}

void GameScene::startGameTouchListener() {

    _eventDispatcher->removeEventListener(touchListener_single);
    touching();
}

void GameScene::delayTouch(float dt) {

    isTouchable = false;
    this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::set_isTouchable), dt);
}

void GameScene::set_isTouchable(float dt) {isTouchable = true;}

// physics contact //

void GameScene::contacting() {

    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = [=](PhysicsContact &contact) {

        PhysicsBody *a = contact.getShapeA()->getBody();
        PhysicsBody *b = contact.getShapeB()->getBody();

        // checks whether are bodies have collided //

        // detect
        if (tool.isAtContact(seed.getColMask("detect"), seed.getColMask("building"), a, b)) {

            auto instance = tool.getCollidedInstance("building", a, b, &seed);
            instance->getPhysicsBody()->removeFromWorld();
        }

        // heli
        else if (tool.isAtContact(seed.getColMask("heli"), seed.getColMask("edge"), a, b)) {

            if (isContactable) {
                
                float obsWidth = tool.getVSz().width;
                checkingCollisionObs(a, b, obsWidth);
            }
        }
        else if (tool.isAtContact(seed.getColMask("heli"), seed.getColMask("building"), a, b)) {

            if (isContactable) {

                float obsWidth = tool.getBSz().width * 3;
                checkingCollisionObs(a, b, obsWidth);
            }
        }
        else if (tool.isAtContact(seed.getColMask("heli"), seed.getColMask("zombie"), a, b)) {

            if (isContactable) {

                setHeliStatus(2);
                gui.heli_PinaltyBar_update(heliStatus);
                hideCollidedInstance("zombie", a, b);

                if (isBeingPunished) {

                    isBeingPunished = false;
                    this->unschedule(static_cast<SEL_SCHEDULE>(&GameScene::heli_punishmentMissile));
                }

                int j = 0;
                for (int i = 0; i < 3; i++) j += heliStatus[i] == 2 ? 1 : 0;
                if (j == 3) {
                    
                    if (isControlable) {
                        
                        isControlable = false;
                        heli.infected(true);
                    }
                }
            }
        }
        else if (tool.isAtContact(seed.getColMask("heli"), seed.getColMask("victim"), a, b)) {

            if (isContactable) {

                if (isControlable == false) {
                    
                    isControlable = true;
                    heli.infected(false);
                }

                if (isBeingPunished) {

                    isBeingPunished = false;
                    this->unschedule(static_cast<SEL_SCHEDULE>(&GameScene::heli_punishmentMissile));
                }

                auto instance = tool.getCollidedInstance("victim", a, b, &seed);
                coinCollect(instance->getPosition());
                hideCollidedInstance("victim", a, b);

                setHeliStatus(0);
                gui.heli_PinaltyBar_update(heliStatus);
            }
        }

        // zombified heli
        else if (tool.isAtContact(seed.getColMask("heli zombified"), seed.getColMask("heli"), a, b)) {

            if (isContactable) {
                
                setHeliStatus(2);
                gui.heli_PinaltyBar_update(heliStatus);
                checkingZombifiedHeliWreck(heliTotalHP, a, b);
            }
        }
        else if (tool.isAtContact(seed.getColMask("heli zombified"), seed.getColMask("bullet"), a, b)) {

            checkingZombifiedHeliWreck(10, a, b);
            heli.bulletRemove("bullet", "fire", a, b);
        }
        else if (tool.isAtContact(seed.getColMask("heli zombified"), seed.getColMask("missile pinalty"), a, b)) {

            heli.missileExplode("missile pinalty", a, b);
        }
        else if (tool.isAtContact(seed.getColMask("heli zombified"), seed.getColMask("missile heli"), a, b)) {

            heli.missileExplode("missile heli", a, b);
        }

        // bullet
        else if (  tool.isAtContact(seed.getColMask("bullet"), seed.getColMask("edge"), a, b)
                || tool.isAtContact(seed.getColMask("bullet"), seed.getColMask("building"), a, b)
                || tool.isAtContact(seed.getColMask("bullet"), seed.getColMask("oil drum"), a, b)
                || tool.isAtContact(seed.getColMask("bullet"), seed.getColMask("change weapon"), a, b)
                || tool.isAtContact(seed.getColMask("bullet"), seed.getColMask("missile pinalty"), a, b)) {

            heli.bulletRemove("bullet", "fire", a, b);
        }
        else if (tool.isAtContact(seed.getColMask("bullet"), seed.getColMask("zombie"), a, b)) {

            checkingFiredTarget("zombie", -1, a, b);
        }
        else if (tool.isAtContact(seed.getColMask("bullet"), seed.getColMask("victim"), a, b)) {

            if (isControlable == false) {
                
                isControlable = true;
                heli.infected(false);
            }

            checkingFiredTarget("victim", -1, a, b);
        }

        // missile
        else if (tool.isAtContact(seed.getColMask("missile pinalty"), seed.getColMask("edge"), a, b)) {

            auto instance = tool.getCollidedInstance("missile pinalty", a, b, &seed);
            if (instance->getPosition().x >= tool.ctrPos().x) heli.missileExplode("missile pinalty", a, b);
        }
        if (tool.isAtContact(seed.getColMask("missile pinalty"), seed.getColMask("heli"), a, b)) {

            if (isContactable) heli.missileExplode("missile pinalty", a, b);
        }
        else if (  tool.isAtContact(seed.getColMask("missile pinalty"), seed.getColMask("building"), a, b)
                || tool.isAtContact(seed.getColMask("missile pinalty"), seed.getColMask("zombie"), a, b)
                || tool.isAtContact(seed.getColMask("missile pinalty"), seed.getColMask("victim"), a, b)
                || tool.isAtContact(seed.getColMask("missile pinalty"), seed.getColMask("bullet"), a, b)
                || tool.isAtContact(seed.getColMask("missile pinalty"), seed.getColMask("oil drum"), a, b)
                || tool.isAtContact(seed.getColMask("missile pinalty"), seed.getColMask("change weapon"), a, b)) {

            heli.missileExplode("missile pinalty", a, b);
        }
        else if (  tool.isAtContact(seed.getColMask("missile heli"), seed.getColMask("edge"), a, b)
                || tool.isAtContact(seed.getColMask("missile heli"), seed.getColMask("building"), a, b)
                || tool.isAtContact(seed.getColMask("missile heli"), seed.getColMask("zombie"), a, b)
                || tool.isAtContact(seed.getColMask("missile heli"), seed.getColMask("victim"), a, b)
                || tool.isAtContact(seed.getColMask("missile heli"), seed.getColMask("oil drum"), a, b)
                || tool.isAtContact(seed.getColMask("missile heli"), seed.getColMask("change weapon"), a, b)) {

            heli.missileExplode("missile heli", a, b);
        }

        // explosion
        else if (tool.isAtContact(seed.getColMask("explosion"), seed.getColMask("heli"), a, b)) {

            if (isContactable) checkingHeliWreck(50, Vec2::ZERO);
        }
        else if (tool.isAtContact(seed.getColMask("explosion"), seed.getColMask("heli zombified"), a, b)) {

            checkingZombifiedHeliWreck(50, a, b);
        }
        else if (tool.isAtContact(seed.getColMask("explosion"), seed.getColMask("zombie"), a, b)) {

            checkingFiredTarget("zombie", -3, a, b);
        }
        else if (tool.isAtContact(seed.getColMask("explosion"), seed.getColMask("victim"), a, b)) {

            if (isControlable == false) {
                
                isControlable = true;
                heli.infected(false);
            }

            checkingFiredTarget("victim", -3, a, b);
        }

        // oil drum
        else if (tool.isAtContact(seed.getColMask("oil drum"), seed.getColMask("heli"), a, b)) {

            if (isContactable) oilDrumOnContact(a, b);
        }
        else if (  tool.isAtContact(seed.getColMask("oil drum"), seed.getColMask("bullet"), a, b)
                || tool.isAtContact(seed.getColMask("oil drum"), seed.getColMask("missile pinalty"), a, b)
                || tool.isAtContact(seed.getColMask("oil drum"), seed.getColMask("missile heli"), a, b)
                || tool.isAtContact(seed.getColMask("oil drum"), seed.getColMask("explosion"), a, b)) {

            oilDrumOnContact(a, b);
        }

        // change weapon
        else if (tool.isAtContact(seed.getColMask("change weapon"), seed.getColMask("heli"), a, b)) {

            if (isContactable) {

                auto instance = tool.getCollidedInstance("heli", a, b, &seed);
                instance->setTag(instance->getTag() == 0 ? 1 : 0); // '0' = bullet & '1' = missile
                hideCollidedInstance("change weapon", a, b);
                gui.showAlert("WEAPON SWAPPED", Color3B::YELLOW, Color4B::ORANGE);
                tool.playSFX("change weapon");
            }
        }
        else if (  tool.isAtContact(seed.getColMask("change weapon"), seed.getColMask("bullet"), a, b)
                || tool.isAtContact(seed.getColMask("change weapon"), seed.getColMask("missile pinalty"), a, b)
                || tool.isAtContact(seed.getColMask("change weapon"), seed.getColMask("missile heli"), a, b)
                || tool.isAtContact(seed.getColMask("change weapon"), seed.getColMask("explosion"), a, b)) {

            hideCollidedInstance("change weapon", a, b);
        }

        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

void GameScene::checkingCollisionObs(cocos2d::PhysicsBody *a, cocos2d::PhysicsBody *b, float obsWidth) {

    Node *node[2];
    int hpDecrease;

    if (seed.getColMask("heli") == a->getCollisionBitmask()) {

        node[0] = a->getNode();
        node[1] = b->getNode();
    }
    else {

        node[0] = b->getNode();
        node[1] = a->getNode();
    }

    if (node[0]->getPosition().x > node[1]->getPosition().x - obsWidth / 2) hpDecrease = 50;
    else hpDecrease = 1000;

    Vec2 sparkPos;

    if (obsWidth > tool.getVSz().width / 2) {
        
        if (node[0]->getPosition().y < tool.ctrPos().y) {

            sparkPos = Vec2(node[0]->getPosition().x, node[0]->getPosition().y - node[0]->getContentSize().height / 2);
            heli.offset("edge", true);
        }
        else {

            sparkPos = Vec2(node[0]->getPosition().x, node[0]->getPosition().y + node[0]->getContentSize().height / 2);
            heli.offset("edge", false);
        }
    }
    else {
        
        sparkPos = Vec2(node[0]->getPosition().x, node[0]->getPosition().y - node[0]->getContentSize().height / 2);
        heli.offset("building", true);
    }

    checkingHeliWreck(hpDecrease, sparkPos);
}

void GameScene::checkingFiredTarget(
    
    std::string designation,
    int hpCount,
    cocos2d::PhysicsBody *a,
    cocos2d::PhysicsBody *b
) {

    if (designation == "zombie") {

        scoreAddition(10);

        if (hpCount == -1) heli.bulletRemove("bullet", "zombie blood", a, b);
        auto instance = tool.getCollidedInstance("zombie", a, b, &seed);
        Vec2 instancePosBuff = instance->getPosition();
        instance->setTag(instance->getTag() + hpCount);

        if (obs.zombieCarnage()) {
            
            coinCollect(instancePosBuff);
            checkingHeliWreck(-10, Vec2::ZERO);
        }
    }
    else if (designation == "victim") {

        if (hpCount == -1) heli.bulletRemove("bullet", "victim blood", a, b);
        auto instance = tool.getCollidedInstance("victim", a, b, &seed);
        instance->setTag(instance->getTag() + hpCount);

        if (obs.victimCarnage()) {
                
            setHeliStatus(1);
            gui.heli_PinaltyBar_update(heliStatus);
            checkingHeliWreck(-10, Vec2::ZERO);
        }
            
        int j = 0;
        for (int i = 0; i < 3; i++) j += heliStatus[i] == 1 ? 1 : 0;
        if (j == 3 && isBeingPunished == false) {
            
            isBeingPunished = true;
            gui.showAlert("PUNISHMENT!!!", Color3B(255, 0, 0), Color4B(127, 0, 0, 255));
            this->schedule(static_cast<SEL_SCHEDULE>(&GameScene::heli_punishmentMissile), 0.0f);
            tool.playSFX("punishment");
        }
    }
}

void GameScene::hideCollidedInstance(std::string designation, cocos2d::PhysicsBody *a, cocos2d::PhysicsBody *b) {

    auto instance = tool.getCollidedInstance(designation, a, b, &seed);

    if (designation == "zombie") designation = "zombie pick";
    else if (designation == "victim") designation = "victim pick";

    heli.bulletSpark(instance->getPosition(), designation);

    instance->stopAllActions();
    instance->setPosition(tool.pos(0.5, 2.0));
}

void GameScene::checkingHeliWreck(int hpDecrease, cocos2d::Vec2 sparkPos) {

    if (isContactable) {
        
        int testHP = heli.getHP() - hpDecrease;
        if (testHP > heliTotalHP) hpDecrease = 0;

        if (heli.isWreck(hpDecrease, sparkPos)) {

            _eventDispatcher->removeAllEventListeners();
            isContactable = false;
            contacting();
            this->unschedule(static_cast<SEL_SCHEDULE>(&GameScene::heli_fire));
            this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::goToGameOverScene), 1.0f);
            if (tool.getUDef()->getBoolForKey("SOUND ON")) tool.playFanSound(false);
        }

        gui.heli_HPBar_update(heli.getHP(), heliTotalHP);
    }
}

void GameScene::checkingZombifiedHeliWreck(int hpDecrease, cocos2d::PhysicsBody *a, cocos2d::PhysicsBody *b) {

    auto instance = tool.getCollidedInstance("heli zombified", a, b, &seed);
    Vec2 instancePosBuff = instance->getPosition();
    if (heli.isWreck(instance->getTag(), hpDecrease)) {
        
        coinCollect(instancePosBuff);
        scoreAddition(50);
    }
}

void GameScene::oilDrumOnContact(cocos2d::PhysicsBody *a, cocos2d::PhysicsBody *b) {

    auto instance = tool.getCollidedInstance("oil drum", a, b, &seed);
    Vec2 instancePosBuff = instance->getPosition();
    hideCollidedInstance("oil drum", a, b);
    heli.explode(instancePosBuff);
}

// back button ANDROID
void GameScene::onKeyReleasing() {

    auto backButtonListener = EventListenerKeyboard::create();
    backButtonListener->onKeyReleased = [=](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {

        if (keyCode == EventKeyboard::KeyCode::KEY_BACK) {

            if (isGameStarted && isTutoring == false) {
            
                delayTouch(0.1);
                this->unschedule(static_cast<SEL_SCHEDULE>(&GameScene::heli_fire));
                this->scheduleOnce(static_cast<SEL_SCHEDULE>(&GameScene::goToPauseScene), 0.1f);
            }
            else {
                
                if      (gui.getNoticeDesignation() == "payment")        gui.showNoticeBoard(false, "payment");
                else if (gui.getNoticeDesignation() == "exit")           gui.showNoticeBoard(false, "exit");
                else if (gui.getNoticeDesignation() == "not enough")     gui.showNoticeBoard(false, "not enough");
                else if (gui.getNoticeDesignation() == "heli purchased") gui.showNoticeBoard(false, "heli purchased");
                else if (gui.getNoticeDesignation() == "settings")       gui.showNoticeBoard(false, "settings");
                else gui.showNoticeBoard(true, "exit");
            }

            tool.playSFX("select off");
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(backButtonListener, this);
}