#include "Obstacles.h"

USING_NS_CC;

void Obstacles::init(cocos2d::Layer *layer_in, Seed *seed_in, Tool *tool_in, Helicopter *heli_in) {

    layer = layer_in;
    seed = seed_in;
    tool = tool_in;
    heli = heli_in;
        
    auto edgeNode = Node::create();
    auto edgeBody = PhysicsBody::createEdgeBox(Size(tool->getVSz().width, tool->getBSz().height * 12));
    edgeBody->setDynamic(false);
    edgeBody->setContactTestBitmask(true);
    edgeBody->setCollisionBitmask(seed->getColMask("edge"));
    edgeNode->setPhysicsBody(edgeBody);
    edgeNode->setPosition(tool->ctrPos());
    layer->addChild(edgeNode);

    auto detectNode = Node::create();
    auto detectBody = PhysicsBody::createEdgeSegment(Vec2(0, tool->getBSz().height * 6), Vec2(0, tool->getBSz().height * -6));
    detectBody->setDynamic(false);
    detectBody->setContactTestBitmask(true);
    detectBody->setCollisionBitmask(seed->getColMask("detect"));
    detectNode->setPhysicsBody(detectBody);
    detectNode->setPosition(tool->ctrPos() - Vec2(tool->getBSz().width * 5, 0));
    layer->addChild(detectNode);

    for (int i = 0; i < instanceCarnagesCount; i++) {

        tool->mySpriteFrames(
            
            instanceCarnage_bg[i],
            instanceCarnageFramesCount,
            "GENERAL/carnage_%d.png",
            3, layer
        );

        tool->mySpriteFrames(
            
            instanceCarnage[i],
            instanceCarnageFramesCount,
            "GENERAL/carnage_%d.png",
            3, instanceCarnage_bg[i][0]
        );

        for (int j = 0; j < instanceCarnageFramesCount; j++) {
            
            instanceCarnage[i][j]->setPosition(instanceCarnage_bg[i][0]->getContentSize() / 2);
            instanceCarnage[i][j]->setScale(0.95);
        }
    }

    zombieInit();
    victimInit();
    oilDrumInit();
    changeWeaponInit();
}

void Obstacles::spawn(bool isObs) {

    float scaleRate;
    Color4F buildingCol, windowCol;

    if (isObs) {

        scaleRate = 1.0;
        buildingCol = Color4F(0.39, 0.42, 0.45, 1.0);
        windowCol = Color4F(1.0, 0.91, 0.24, 1.0);
    }
    else {

        scaleRate = 0.9;
        buildingCol = Color4F(0.27, 0.30, 0.32, 1.0);
        windowCol = Color4F(0.88, 0.79, 0.08, 1.0);
    }

    std::srand(std::time(0));
    int randVal = std::rand() % 8 + 1;
    if (randVal < 3) randVal = 3;

    Vec2 pos[3 * randVal];
    int h = 0;
    float decreaseBSz_H = randVal % 2 == 0 ? tool->getBSz().height * scaleRate * 0.5 : 0.0;

    for (int i = -randVal / 2; i <= randVal / 2; i++) {

        int valSign = i < 0 ? 1 : -1;

        if (randVal % 2 == 0 && i == 0) i++;
    
        for (int j = -1; j <= 1; j++) {
        
            pos[h] = Vec2(j * tool->getBSz().width * scaleRate, i * tool->getBSz().height * scaleRate + valSign * decreaseBSz_H);
            h++;
        }
    }

    Size obsSize = Size(tool->getBSz().width * scaleRate * 3, tool->getBSz().height * scaleRate * randVal);

    auto obs = DrawNode::create();
    obs->drawSolidRect(-Vec2(obsSize) / 2, Vec2(obsSize) / 2, buildingCol);
    obs->drawRect(
        
        Vec2(-obsSize.width / 2, -obsSize.height / 2),
        Vec2(-obsSize.width / 2,  obsSize.height / 2),
        Vec2( obsSize.width / 2,  obsSize.height / 2),
        Vec2( obsSize.width / 2, -obsSize.height / 2),
        Color4F::BLACK
    );

    Vec2 polyPoints[3] = {

        Vec2(-obsSize.width / 2, obsSize.height / 2 - tool->getBSz().height * 3),
        Vec2(-obsSize.width / 2, obsSize.height / 2),
        Vec2( obsSize.width / 2, obsSize.height / 2)
    };
    obs->drawSolidPoly(polyPoints, 3, Color4F(1.0, 1.0, 1.0, 0.125));
    
    for (int i = 0; i < 3 * randVal; i++) {

        obs->drawSolidRect(
            
            pos[i] - tool->getBSz() * scaleRate / 4,
            pos[i] + tool->getBSz() * scaleRate / 4,
            windowCol
        );
        obs->drawRect(
            
            pos[i] + Vec2(-tool->getBSz().width * scaleRate / 4, -tool->getBSz().height * scaleRate / 4),
            pos[i] + Vec2(-tool->getBSz().width * scaleRate / 4,  tool->getBSz().height * scaleRate / 4),
            pos[i] + Vec2( tool->getBSz().width * scaleRate / 4,  tool->getBSz().height * scaleRate / 4),
            pos[i] + Vec2( tool->getBSz().width * scaleRate / 4, -tool->getBSz().height * scaleRate / 4),
            Color4F::BLACK
        );
    }

    obs->setPosition(
        
        tool->pos(1.0, 0.5) +
        Vec2(obsSize.width / 2, obsSize.height / 2 - tool->getBSz().height * 6)
    );

    PhysicsBody *obsBody;
    float moveDT;

    if (isObs) {

        obsBody = PhysicsBody::createBox(obsSize);
        obsBody->setDynamic(false);
        obsBody->setContactTestBitmask(true);
        obsBody->setCollisionBitmask(seed->getColMask("building"));
        obs->setPhysicsBody(obsBody);
        layer->addChild(obs, 1);

        moveDT = 9.0;

        // building instance
        for (int i = 0; i < 3; i++) {

            if      (seed->getInstanceTag(i) == 1) zombieSpawn(obs->getPosition(), obsSize, i + 1);
            else if (seed->getInstanceTag(i) == 2) victimSpawn(obs->getPosition(), obsSize, i + 1);
            else if (seed->getInstanceTag(i) == 3) oilDrumSpawn(obs->getPosition(), obsSize, i + 1);
            else if (seed->getInstanceTag(i) == 4) changeWeaponSpawn(obs->getPosition(), obsSize, i + 1);
            else if (seed->getInstanceTag(i) == 5) heli->zombifiedHelicopterSpawn(obs->getPosition(), obsSize, i + 1);
        }
    }
    else {

        moveDT = 18.0;
        layer->addChild(obs, 0);
    }

    obs->runAction(Sequence::create(
            
        MoveBy::create(moveDT, tool->getInstanceMovement()),
        RemoveSelf::create(),
        nullptr
    ));
}

//// HUMANOID ////

void Obstacles::carnage(cocos2d::Sprite *target, int targetTag /* 0 = zombie, 1 = victim */) {
    
    tool->mySpriteAnimation(

        instanceCarnage_bg[currentInstanceCarnage],
        instanceCarnageFramesCount,
        0.05,
        target->getPosition(),
        true,
        instanceCarnageColor[targetTag][0]
    );
    
    tool->mySpriteAnimation(

        instanceCarnage[currentInstanceCarnage],
        instanceCarnageFramesCount,
        0.05,
        instanceCarnage_bg[currentInstanceCarnage][0]->getContentSize() / 2,
        true,
        instanceCarnageColor[targetTag][1]
    );

    tool->currentObjectCounter(&currentInstanceCarnage, instanceCarnagesCount);

    target->stopAllActions();
    target->setPosition(tool->pos(0.5, 2.0));
    target->setTag(3);
    target->setVisible(false);

    tool->playSFX("carnage");
}

// zombie //

void Obstacles::zombieInit() {

    for (int i = 0; i < zombiesCount; i++) {

        zombie[i] = tool->mySprite("GENERAL/zombie_walk.png");
        zombie[i]->setPosition(tool->pos(0.5, 2.0));
        zombie[i]->setVisible(false);
        zombie[i]->setTag(3);
        zombieBody[i] = PhysicsBody::createBox(zombie[i]->getContentSize());
        zombieBody[i]->setDynamic(false);
        zombieBody[i]->setContactTestBitmask(true);
        zombieBody[i]->setCollisionBitmask(seed->getColMask("zombie"));
        zombie[i]->setPhysicsBody(zombieBody[i]);
        layer->addChild(zombie[i], 3);
    }
}

void Obstacles::zombieSpawn(cocos2d::Vec2 obsPos, cocos2d::Size obsSize, int posRate) {

    zombie[currentZombie]->setPosition(Vec2(
            
        obsPos.x - obsSize.width / 2 + obsSize.width / 4 * posRate,
        obsPos.y + obsSize.height / 2 + zombie[currentZombie]->getContentSize().height / 2
    ));
    zombie[currentZombie]->setVisible(true);
    zombie[currentZombie]->runAction(Spawn::create(
        
        MoveBy::create(9, tool->getInstanceMovement()),
        Repeat::create(Sequence::create(RotateTo::create(0.25, -11.25), RotateTo::create(0.25, 11.25), nullptr), 12),
        nullptr
    ));
    tool->currentObjectCounter(&currentZombie, zombiesCount);
}

bool Obstacles::zombieCarnage() {

    bool isCarnage = false;
    for (int i = 0; i < zombiesCount; i++) {

        if (zombie[i]->getTag() <= 0) {
            
            carnage(zombie[i], 0);
            isCarnage = true;
        }
    }

    if (isCarnage) return true;
    else return false;
}

// victim //

void Obstacles::victimInit() {

    for (int i = 0; i < victimsCount; i++) {

        victim[i] = tool->mySprite("GENERAL/victim_sos.png");
        victim[i]->setPosition(tool->pos(0.5, 2.0));
        victim[i]->setVisible(false);
        victim[i]->setTag(3);
        victimBody[i] = PhysicsBody::createBox(victim[i]->getContentSize());
        victimBody[i]->setDynamic(false);
        victimBody[i]->setContactTestBitmask(true);
        victimBody[i]->setCollisionBitmask(seed->getColMask("victim"));
        victim[i]->setPhysicsBody(victimBody[i]);
        layer->addChild(victim[i], 3);
    }
}

void Obstacles::victimSpawn(cocos2d::Vec2 obsPos, cocos2d::Size obsSize, int posRate) {

    victim[currentVictim]->setPosition(Vec2(
            
        obsPos.x - obsSize.width / 2 + obsSize.width / 4 * posRate,
        obsPos.y + obsSize.height / 2 + victim[currentVictim]->getContentSize().height / 2
    ));
    victim[currentVictim]->setVisible(true);
    victim[currentVictim]->runAction(Spawn::create(
        
        MoveBy::create(9, tool->getInstanceMovement()),
        Repeat::create(Sequence::create(RotateTo::create(0.25, -11.25), RotateTo::create(0.25, 11.25), nullptr), 12),
        nullptr
    ));
    tool->currentObjectCounter(&currentVictim, victimsCount);
}

bool Obstacles::victimCarnage() {

    bool isCarnage = false;
    for (int i = 0; i < victimsCount; i++) {

        if (victim[i]->getTag() <= 0) {
            
            carnage(victim[i], 1);
            isCarnage = true;
        }
    }

    if (isCarnage) return true;
    else return false;
}

// oil drum //

void Obstacles::oilDrumInit() {

    for (int i = 0; i < oilDrumsCount; i++) {

        oilDrum[i] = tool->mySprite("GENERAL/oil_drum.png");
        oilDrum[i]->setPosition(tool->pos(0.5, 2.0));
        oilDrum[i]->setVisible(false);
        oilDrumBody[i] = PhysicsBody::createBox(oilDrum[i]->getContentSize());
        oilDrumBody[i]->setDynamic(false);
        oilDrumBody[i]->setContactTestBitmask(true);
        oilDrumBody[i]->setCollisionBitmask(seed->getColMask("oil drum"));
        oilDrum[i]->setPhysicsBody(oilDrumBody[i]);
        layer->addChild(oilDrum[i], 3);
    }
}

void Obstacles::oilDrumSpawn(cocos2d::Vec2 obsPos, cocos2d::Size obsSize, int posRate) {

    oilDrum[currentOilDrum]->setPosition(Vec2(
            
        obsPos.x - obsSize.width / 2 + obsSize.width / 4 * posRate,
        obsPos.y + obsSize.height / 2 + oilDrum[currentOilDrum]->getContentSize().height / 2
    ));
    oilDrum[currentOilDrum]->setVisible(true);
    oilDrum[currentOilDrum]->runAction(MoveBy::create(9, tool->getInstanceMovement()));
    tool->currentObjectCounter(&currentOilDrum, oilDrumsCount);
}

// change weapon //

void Obstacles::changeWeaponInit() {

    for (int i = 0; i < changeWeaponsCount; i++) {

        changeWeapon[i] = tool->mySprite("GENERAL/change_weapon.png");
        changeWeapon[i]->setPosition(tool->pos(0.5, 2.0));
        changeWeapon[i]->setVisible(false);
        changeWeaponBody[i] = PhysicsBody::createBox(changeWeapon[i]->getContentSize());
        changeWeaponBody[i]->setDynamic(false);
        changeWeaponBody[i]->setContactTestBitmask(true);
        changeWeaponBody[i]->setCollisionBitmask(seed->getColMask("change weapon"));
        changeWeapon[i]->setPhysicsBody(changeWeaponBody[i]);
        layer->addChild(changeWeapon[i], 3);
    }
}

void Obstacles::changeWeaponSpawn(cocos2d::Vec2 obsPos, cocos2d::Size obsSize, int posRate) {

    changeWeapon[currentChangeWeapon]->setPosition(Vec2(
            
        obsPos.x - obsSize.width / 2 + obsSize.width / 4 * posRate,
        obsPos.y + obsSize.height / 2 + changeWeapon[currentChangeWeapon]->getContentSize().height / 2
    ));
    changeWeapon[currentChangeWeapon]->setVisible(true);
    changeWeapon[currentChangeWeapon]->runAction(MoveBy::create(9, tool->getInstanceMovement()));
    tool->currentObjectCounter(&currentChangeWeapon, changeWeaponsCount);
}