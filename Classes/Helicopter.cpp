#include "Helicopter.h"

USING_NS_CC;

void Helicopter::init(cocos2d::Layer *layer_in, Seed *seed_in, Tool *tool_in) {

    layer = layer_in;
    seed = seed_in;
    tool = tool_in;

    //// HELICOPTER INCLUDES ////

    currentSelected = tool->getUDef()->getIntegerForKey("CURRENT SELECTED", 0);

    int isPurchased[helisCount];
    char str[32];

    for (int i = 0; i < helisCount; i++) {
        
        std::sprintf(str, "IS HELICOPTER %d PURCHASED", i + 1);
        isPurchased[i] = tool->getUDef()->getIntegerForKey(str, 0);
    }

    int k = 0;

    for (int i = 0; i < helisCount; i++) {

        std::sprintf(str, "HELICOPTER/heli_body_%d.png", i + 1);
        heli[i] = tool->mySprite(str);
        heli[i]->setPosition(tool->ctrPos());
        heli[i]->setTag(0); // determine current used weapon // '0' = bullet & '1' = missile

        bool isInvisible = false;

        if (i == currentSelected && isPurchased[i] == true) heli[i]->setVisible(true);
        else {
            
            isInvisible = true;
            heli[i]->setVisible(false);
        }

        layer->addChild(heli[i], 4);

        // main fan //

        std::sprintf(str, "HELICOPTER/heli_main_fan_%d.png", i + 1);
        heliMainFan[i] = tool->mySprite(str);
        heliMainFan[i]->setPosition(
            
            Vec2(heli[i]->getContentSize().width * 0.682, heli[i]->getContentSize().height * 0.928) +
            Vec2(0, heliMainFan[i]->getContentSize().height / 2) + Director::getInstance()->getVisibleOrigin()
        );
        heli[i]->addChild(heliMainFan[i], 1);

        heliMainFan[i]->runAction(RepeatForever::create(
            
            Sequence::create(

                ScaleTo::create(0.1, 0.6, 1.0),
                ScaleTo::create(0.1, 0.3, 1.0),
                ScaleTo::create(0.1, 0.6, 1.0),
                ScaleTo::create(0.1, 1.0, 1.0),
                nullptr
            )
        ));

        // tail fan //

        for (int j = 0; j < 2; j++) {

            float rotation = j == 0 ? 0.0 : 90;
            
            std::sprintf(str, "HELICOPTER/heli_tail_fan_%d.png", i + 1);
            heliTailFan[i][j] = tool->mySprite(str);
            heliTailFan[i][j]->setPosition(
                
                Vec2(heli[i]->getContentSize().width * 0.061, heli[i]->getContentSize().height * 0.713) +
                Director::getInstance()->getVisibleOrigin()
            );
            heliTailFan[i][j]->setRotation(rotation);
            heli[i]->addChild(heliTailFan[i][j], 1);

            heliTailFan[i][j]->runAction(RepeatForever::create(RotateBy::create(0.1, 45.0)));
        }

        if (isInvisible) k++;
    }

    if (k == helisCount) {
        
        heli[0]->setVisible(true);
        tool->setUserValue("CURRENT SELECTED", 0, &currentSelected);
    }

    //// HELICOPTER INCLUDES ////

    // bullet sparks init
    for (int i = 0; i < bulletSparksCount; i++) {
        
        tool->mySpriteFrames(bulletPartBG[i], bulletSparkFramesCount, "GENERAL/spark_%d.png", 4, layer);
        tool->mySpriteFrames(bulletPart[i], bulletSparkFramesCount, "GENERAL/spark_%d.png", 0, bulletPartBG[i][0]);
        for (int j = 0; j < bulletSparkFramesCount; j++) bulletPart[i][j]->setScale(0.95);
    }

    // explosion
    for (int i = 0; i < explosionsCount; i++) {

        tool->mySpriteFrames(explosion[i], explosionFramesCount, "GENERAL/explode_%d.png", 5, layer);

        explosionBody[i] = PhysicsBody::createCircle(explosion[i][(explosionFramesCount - 1) / 2]->getContentSize().width / 2);
        explosionBody[i]->setDynamic(false);
        explosionBody[i]->setContactTestBitmask(true);
        explosionBody[i]->setCollisionBitmask(seed->getColMask("explosion"));
        explosion[i][(explosionFramesCount - 1) / 2]->setPhysicsBody(explosionBody[i]);
    }

    zombifiedHelicopterInit();

    // data //

    heliData[0].HP = 100;
    heliData[1].HP = 200;
    heliData[2].HP = 300;
    heliData[3].HP = 400;
    heliData[4].HP = 500;

    heliData[0].fireSpeed[0] = 500;
    heliData[1].fireSpeed[0] = 600;
    heliData[2].fireSpeed[0] = 700;
    heliData[3].fireSpeed[0] = 800;
    heliData[4].fireSpeed[0] = 900;

    heliData[0].fireSpeed[1] = 500 / 2;
    heliData[1].fireSpeed[1] = 600 / 2;
    heliData[2].fireSpeed[1] = 700 / 2;
    heliData[3].fireSpeed[1] = 800 / 2;
    heliData[4].fireSpeed[1] = 900 / 2;

    if (tool->getUDef()->getBoolForKey("SOUND ON")) tool->playFanSound(true);
}

// npc zombified helis //

void Helicopter::zombifiedHelicopterInit() {

    Vec2 points[7] = {

        Vec2( heli[currentSelected]->getContentSize().width * 0.500,  heli[currentSelected]->getContentSize().height * 0.075),
        Vec2( heli[currentSelected]->getContentSize().width * 0.500,  heli[currentSelected]->getContentSize().height * 0.500),
        Vec2(-heli[currentSelected]->getContentSize().width * 0.213,  heli[currentSelected]->getContentSize().height * 0.427),
        Vec2(-heli[currentSelected]->getContentSize().width * 0.398,  heli[currentSelected]->getContentSize().height * 0.259),
        Vec2(-heli[currentSelected]->getContentSize().width * 0.500, -heli[currentSelected]->getContentSize().height * 0.367),
        Vec2(-heli[currentSelected]->getContentSize().width * 0.394, -heli[currentSelected]->getContentSize().height * 0.500),
        Vec2( heli[currentSelected]->getContentSize().width * 0.054, -heli[currentSelected]->getContentSize().height * 0.500)
    };

    for (int i = 0; i < zombifiedHelisCount; i++) {

        zombifiedHeli[i] = tool->mySprite("HELICOPTER/heli_infected_1.png");
        zombifiedHeli[i]->setPosition(tool->pos(0.5, -2.0));
        zombifiedHeliBody[i] = PhysicsBody::createPolygon(points, 7);
        zombifiedHeliBody[i]->setDynamic(false);
        zombifiedHeliBody[i]->setContactTestBitmask(true);
        zombifiedHeliBody[i]->setCollisionBitmask(seed->getColMask("heli zombified"));
        zombifiedHeli[i]->setPhysicsBody(zombifiedHeliBody[i]);
        zombifiedHeli[i]->setFlippedX(true);
        zombifiedHeli[i]->setTag(i);
        layer->addChild(zombifiedHeli[i], 2);

        // main fan //

        zombifiedHeliMainFan[i] = tool->mySprite("HELICOPTER/heli_main_fan_1.png");
        zombifiedHeliMainFan[i]->setPosition(
            
            Vec2(zombifiedHeli[i]->getContentSize().width * 0.315, zombifiedHeli[i]->getContentSize().height * 0.928) +
            Vec2(0, zombifiedHeliMainFan[i]->getContentSize().height / 2) + Director::getInstance()->getVisibleOrigin()
        );
        zombifiedHeli[i]->addChild(zombifiedHeliMainFan[i], 1);

        zombifiedHeliMainFan[i]->runAction(RepeatForever::create(
            
            Sequence::create(

                ScaleTo::create(0.1, 0.6, 1.0),
                ScaleTo::create(0.1, 0.3, 1.0),
                ScaleTo::create(0.1, 0.6, 1.0),
                ScaleTo::create(0.1, 1.0, 1.0),
                nullptr
            )
        ));

        // tail fan //

        for (int j = 0; j < 2; j++) {

            float rotation = j == 0 ? 0.0 : 90;

            zombifiedHeliTailFan[i][j] = tool->mySprite("HELICOPTER/heli_tail_fan_1.png");
            zombifiedHeliTailFan[i][j]->setPosition(
                
                Vec2(zombifiedHeli[i]->getContentSize().width * 0.939, zombifiedHeli[i]->getContentSize().height * 0.713) +
                Director::getInstance()->getVisibleOrigin()
            );
            zombifiedHeliTailFan[i][j]->setRotation(rotation);
            zombifiedHeli[i]->addChild(zombifiedHeliTailFan[i][j], 1);

            zombifiedHeliTailFan[i][j]->runAction(RepeatForever::create(RotateBy::create(0.1, 45.0)));
        }

        zombifiedHeliData[i].HP = 100;
    }
}

void Helicopter::zombifiedHelicopterSpawn(cocos2d::Vec2 obsPos, cocos2d::Size obsSize, int posRate) {

    int currBuff = currentZombiefiedHeli;
    zombifiedHeli[currBuff]->setPosition(Vec2(
            
        obsPos.x - obsSize.width / 2 + obsSize.width / 4 * posRate,
        (obsPos.y + obsSize.height / 2 + tool->getBSz().height * 12) / 2
    ));
    zombifiedHeli[currBuff]->setVisible(true);
    zombifiedHeli[currBuff]->stopAllActions();
    zombifiedHeli[currBuff]->runAction(MoveBy::create(9, tool->getInstanceMovement()));
    tool->currentObjectCounter(&currentZombiefiedHeli, zombifiedHelisCount);
}

// playable heli //

void Helicopter::swap(int valSign) {
    
    if ((currentSelected != helisCount - 1 && valSign == 1)
        || (currentSelected != 0 && valSign == -1)) {

        heli[currentSelected]->setVisible(false);
        heli[currentSelected + valSign]->setVisible(true);

        tool->setUserValue("CURRENT SELECTED", currentSelected + valSign, &currentSelected);
    }
}

void Helicopter::attachPhysicsBody() { // similar to say start the game

    for (int i = 0; i < helisCount; i++)
        if (i != currentSelected) heli[i]->runAction(RemoveSelf::create());

    Vec2 points[7] = {

        Vec2(-heli[currentSelected]->getContentSize().width * 0.500,  heli[currentSelected]->getContentSize().height * 0.075),
        Vec2(-heli[currentSelected]->getContentSize().width * 0.500,  heli[currentSelected]->getContentSize().height * 0.500),
        Vec2( heli[currentSelected]->getContentSize().width * 0.213,  heli[currentSelected]->getContentSize().height * 0.427),
        Vec2( heli[currentSelected]->getContentSize().width * 0.398,  heli[currentSelected]->getContentSize().height * 0.259),
        Vec2( heli[currentSelected]->getContentSize().width * 0.500, -heli[currentSelected]->getContentSize().height * 0.367),
        Vec2( heli[currentSelected]->getContentSize().width * 0.394, -heli[currentSelected]->getContentSize().height * 0.500),
        Vec2(-heli[currentSelected]->getContentSize().width * 0.054, -heli[currentSelected]->getContentSize().height * 0.500)
    };

    heliBody = PhysicsBody::createPolygon(points, 7);
    heliBody->setDynamic(false);
    heliBody->setContactTestBitmask(true);
    heliBody->setCollisionBitmask(seed->getColMask("heli"));
    heli[currentSelected]->setPhysicsBody(heliBody);
    heliBody->setVelocity(Vec2(0, -125));
    layer->reorderChild(heli[currentSelected], 2);

    char str[32];
    std::sprintf(str, "HELICOPTER/heli_infected_%d.png", currentSelected + 1);
    infection = tool->mySprite(str);
    infection->setPosition(heli[currentSelected]->getContentSize() / 2);
    infection->setVisible(false);
    heli[currentSelected]->addChild(infection);
}

void Helicopter::fly() {

    heli[currentSelected]->stopAllActions();
    heli[currentSelected]->runAction(RotateTo::create(0.5, 22.5));
    heliBody->setVelocity(Vec2(0, 250));
}

void Helicopter::fall() {

    heli[currentSelected]->stopAllActions();
    heli[currentSelected]->runAction(RotateTo::create(1.0, 0.0));
    heliBody->setVelocity(Vec2(0, -125));
}

void Helicopter::fire(std::string designation) {

    bool isMissile = false, isPinalty = false;

    if (designation == "heli") {
        
        isMissile = heli[currentSelected]->getTag();
    }
    else if (designation == "pinalty") {
        
        isMissile = true;
        isPinalty = true;
    }

    if (isMissile) {

        bool isAvailable = false;
        if (designation == "heli" && isMissileAvailable[0]) isAvailable = true;
        else if (designation == "pinalty" && isMissileAvailable[1]) isAvailable = true;

        if (isAvailable) {

            int frspd;

            if (designation == "heli") isMissileAvailable[0] = false;
            else if (designation == "pinalty") isMissileAvailable[1] = false;
 
            auto missile = tool->mySprite("GENERAL/missile.png");
            auto missileBody = PhysicsBody::createBox(missile->getContentSize());
            missileBody->setDynamic(false);
            missileBody->setContactTestBitmask(true);
            missileBody->setCollisionBitmask(seed->getColMask(isPinalty == true ? "missile pinalty" : "missile heli"));
            missile->setPhysicsBody(missileBody);

            if (isPinalty) {

                frspd = heliData[0].fireSpeed[1];
                missile->setPosition(tool->ctrPos() + Vec2(tool->getBSz().width * -6, tool->getBSz().height * 6));
                missile->setRotation(22.5);
            }
            else {

                frspd = heliData[currentSelected].fireSpeed[1];
                missile->setPosition(
                    
                    heli[currentSelected]->getPosition() + Vec2(
                        
                        heli[currentSelected]->getContentSize().width / 4,
                        -heli[currentSelected]->getContentSize().height / 4
                    )
                );
                missile->setRotation(heli[currentSelected]->getRotation());
            }

            layer->addChild(missile, 2);
            missileBody->setVelocity(Vec2(

                std::cos(missile->getRotation() / 180 * M_PI) * frspd,
                std::sin(missile->getRotation() / 180 * M_PI) * -frspd
            ));

            int currBuff = currentMissileBuffer;
            missileBuffer[currBuff] = missile;
            missile->runAction(RepeatForever::create(Sequence::create(
                
                DelayTime::create(0.125),
                CallFunc::create([=]() {missileSmoke(currBuff);}),
                nullptr
            )));
            tool->currentObjectCounter(&currentMissileBuffer, missileBuffersCount);

            tool->playSFX("missile launch");
        }
    }
    else {
        
        Size bulletSize = Size(tool->getBSz().width / 4, tool->getBSz().height / 8);

        auto bullet = DrawNode::create();
        bullet->drawSolidRect(-Vec2(bulletSize / 2), Vec2(bulletSize / 2), Color4F::YELLOW);
        auto bulletBody = PhysicsBody::createBox(bulletSize);
        bulletBody->setDynamic(false);
        bulletBody->setContactTestBitmask(true);
        bulletBody->setCollisionBitmask(seed->getColMask("bullet"));
        bullet->setPhysicsBody(bulletBody);

        bullet->setPosition(
        
            heli[currentSelected]->getPosition() + Vec2(
                
                heli[currentSelected]->getContentSize().width / 4,
                -heli[currentSelected]->getContentSize().height / 4
            )
        );

        bullet->setRotation(heli[currentSelected]->getRotation());
        layer->addChild(bullet, 2);

        bulletBody->setVelocity(Vec2(

            std::cos(bullet->getRotation() / 180 * M_PI) * heliData[currentSelected].fireSpeed[0],
            std::sin(bullet->getRotation() / 180 * M_PI) * -heliData[currentSelected].fireSpeed[0]
        ));

        tool->playSFX("bullet fire");
    }
}

void Helicopter::bulletSpark(cocos2d::Vec2 pos, std::string designation) {

    Color3B color[2];
    bool isActivated = true;

    if (designation == "fire") {

        color[0] = Color3B::YELLOW; // back
        color[1] = Color3B::WHITE;  // front
        tool->playSFX("bullet receive");
    }
    else if (designation == "carbon") {

        color[0] = Color3B(95, 95, 95);
        color[1] = Color3B(127, 127, 127);
    }
    else if (designation == "zombie blood") {

        color[0] = Color3B(65, 123, 4);
        color[1] = Color3B(97, 155, 36);
        tool->playSFX("bullet receive");
    }
    else if (designation == "victim blood") {

        color[0] = Color3B(223, 95, 0);
        color[1] = Color3B(255, 127, 0);
        tool->playSFX("bullet receive");
    }
    else if (designation == "coin") {

        color[0] = Color3B(255, 218, 24);
        color[1] = Color3B(255, 237, 111);
        tool->playSFX("coin");
    }
    else if (designation == "zombie pick") {

        color[0] = Color3B(97, 155, 36);
        color[1] = Color3B(129, 187, 48);
        tool->playSFX("pick");
    }
    else if (designation == "victim pick") {

        color[0] = Color3B(255, 127, 32);
        color[1] = Color3B(255, 159, 32);
        tool->playSFX("pick");
    }
    else if (designation == "change weapon") {

        color[0] = Color3B(155, 0, 155);
        color[1] = Color3B(255, 0, 255);
        tool->playSFX("pick");
    }
    else isActivated = false;

    if (isActivated) {
        
        bulletPartBG[currentBulletSpark][0]->setRotation(45.0 * (std::rand() % 9));
        tool->mySpriteAnimation(bulletPartBG[currentBulletSpark], bulletSparkFramesCount, 0.05, pos, true, color[0]);
        tool->mySpriteAnimation(bulletPart[currentBulletSpark], bulletSparkFramesCount, 0.05, bulletPartBG[currentBulletSpark][0]->getContentSize() / 2, true, color[1]);
        tool->currentObjectCounter(&currentBulletSpark, bulletSparksCount);
    }
}

void Helicopter::offset(std::string designation, bool isUp) {

    if (designation == "edge") {

        if (isUp) {
            
            fly();
            heli[currentSelected]->setPosition(tool->pos(0.5, 0.0) + Vec2(0, heli[currentSelected]->getContentSize().height / 2));
            heli[currentSelected]->runAction(Sequence::create(DelayTime::create(0.1), CallFunc::create([=](){fall();}), nullptr));
        }
        else {
            
            heli[currentSelected]->setPosition(tool->pos(0.5, 1.0) - Vec2(0, heli[currentSelected]->getContentSize().height / 2));
            fall();
        }
    }
    else if (designation == "building") {

        if (isUp) {
            
            fly();
            heli[currentSelected]->setPosition(heli[currentSelected]->getPosition() + Vec2(0, heli[currentSelected]->getContentSize().height / 2));
            heli[currentSelected]->runAction(Sequence::create(DelayTime::create(0.1), CallFunc::create([=](){fall();}), nullptr));
        }
        else {
            
            heli[currentSelected]->setPosition(heli[currentSelected]->getPosition() - Vec2(0, heli[currentSelected]->getContentSize().height / 2));
            fall();
        }
    }
}

bool Helicopter::isWreck(int tag, int hpDecrease) {

    zombifiedHeliData[tag].HP -= hpDecrease;

    if (zombifiedHeliData[tag].HP <= 0) {

        Vec2 posBuf = zombifiedHeli[tag]->getPosition();
        zombifiedHeli[tag]->stopAllActions();
        zombifiedHeli[tag]->setPosition(tool->pos(0.5, -2.0));
        explode(posBuf);
        zombifiedHeliData[tag].HP = 100;
        return true;
    }

    return false;
}

bool Helicopter::isWreck(int hpDecrease, cocos2d::Vec2 sparkPos) {

    heliData[currentSelected].HP -= hpDecrease;

    if (heliData[currentSelected].HP <= 0) {

        Vec2 posBuf = heli[currentSelected]->getPosition();

        heli[currentSelected]->setVisible(false);
        heliMainFan[currentSelected]->setVisible(false);
        heliTailFan[currentSelected][0]->setVisible(false);
        heliTailFan[currentSelected][1]->setVisible(false);
        heliBody->setVelocity(Vec2::ZERO);

        explode(posBuf);

        return true;
    }
    else if (hpDecrease != 10 && hpDecrease > 0) {

        if (sparkPos == Vec2::ZERO)
            sparkPos = heli[currentSelected]->getPosition() -
                Vec2(0, heli[currentSelected]->getContentSize().height / 2);

        for (int i = 0; i < 2; i++) {
            
            bulletSpark(sparkPos - Vec2(tool->getBSz().width / 2, 0), "fire");
            bulletSpark(sparkPos, "fire");
            bulletSpark(sparkPos + Vec2(tool->getBSz().width / 2, 0), "fire");
        }

        tool->playSFX("frict");
    }

    return false;
}

void Helicopter::explode(cocos2d::Vec2 pos) {
    
    int currBuff = currentExplosion;
    explosion[currBuff][0]->setRotation(45.0 * (std::rand() % 9));
    explosion[currBuff][0]->runAction(Sequence::create(
        
        DelayTime::create(0.48),
        CallFunc::create([=]() {explosion[currBuff][0]->setPosition(tool->pos(-0.5, 2.0));}),
        nullptr
    ));

    tool->mySpriteAnimation(explosion[currBuff], 8, 0.06, pos);
    tool->currentObjectCounter(&currentExplosion, explosionsCount);

    for (int i = 0; i < 2; i++) bulletSpark(pos, "carbon");

    tool->playSFX("explode");
}

void Helicopter::bulletRemove(
    
    std::string bulletDesignation,
    std::string sparkDesignation,
    cocos2d::PhysicsBody *a,
    cocos2d::PhysicsBody *b
) {

    auto instance = tool->getCollidedInstance(bulletDesignation, a, b, seed);
    instance->getPhysicsBody()->removeFromWorld();
    bulletSpark(instance->getPosition(), sparkDesignation);
    instance->stopAllActions();
    instance->runAction(RemoveSelf::create());
}

void Helicopter::missileExplode(
    
    std::string designation,
    cocos2d::PhysicsBody *a,
    cocos2d::PhysicsBody *b
) {

    auto instance = tool->getCollidedInstance(designation, a, b, seed);
    explode(instance->getPosition());
    bulletRemove(designation, "", a, b);
    if (designation == "missile heli") isMissileAvailable[0] = true;
    else if (designation == "missile pinalty") isMissileAvailable[1] = true;
}

void Helicopter::missileSmoke(int tag) {

    auto smoke = DrawNode::create();
    smoke->drawSolidCircle(Vec2::ZERO, tool->getBSz().width / 8, 0.0, 100, Color4F::WHITE);
    smoke->setPosition(missileBuffer[tag]->getPosition());
    layer->addChild(smoke, 1);

    smoke->runAction(Sequence::create(

        Spawn::create(
            
            ScaleTo::create(0.25, 0.0),
            FadeOut::create(0.25),
            nullptr
        ),
        RemoveSelf::create(),
        nullptr
    ));
}

void Helicopter::infected(bool isInfected) {

    if (isInfected) {
        
        infection->setVisible(true);
        infection->runAction(RepeatForever::create(
            
            Sequence::create(

                FadeOut::create(0.25),
                FadeIn::create(0.25),
                nullptr
            )
        ));
    }
    else {
        
        infection->stopAllActions();
        infection->setVisible(false);
    }
}

int Helicopter::getZombifiedHelisCount() {return zombifiedHelisCount;}
int Helicopter::getFR() {return heliData[currentSelected].fireSpeed[0];}
int Helicopter::getHP() {return heliData[currentSelected].HP;}