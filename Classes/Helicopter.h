#ifndef __HELICOPTER_H__
#define __HELICOPTER_H__

#include "cocos2d.h"
#include "Seed.h"
#include "Tool.h"

class Helicopter {

    public :

        //// METHODS ////

        void init(cocos2d::Layer *layer_in, Seed *seed_in, Tool *tool_in);
        void zombifiedHelicopterInit();
        void zombifiedHelicopterSpawn(cocos2d::Vec2 obsPos, cocos2d::Size obsSize, int posRate);
        void swap(int valSign);
        void attachPhysicsBody();
        void fly();
        void fall();
        void fire(std::string designation);
        void bulletSpark(cocos2d::Vec2 pos, std::string designation);
        void offset(std::string designation, bool isUp);
        bool isWreck(int tag, int hpDecrease);
        bool isWreck(int hpDecrease, cocos2d::Vec2 sparkPos);
        void explode(cocos2d::Vec2 pos);
        void infected(bool isInfected);
        void missileSmoke(int tag);

        void bulletRemove(
    
            std::string bulletDesignation,
            std::string sparkDesignation,
            cocos2d::PhysicsBody *a,
            cocos2d::PhysicsBody *b
        );

        void missileExplode(
            
            std::string designation,
            cocos2d::PhysicsBody *a,
            cocos2d::PhysicsBody *b
        );

        int getZombifiedHelisCount();
        int getFR();
        int getHP();
        
    private:

        //// OBJECTS ////

        static const int 
            helisCount = 5,
            zombifiedHelisCount = 7,
            bulletSparksCount = 16,
            explosionsCount = 3,
            missileBuffersCount = 2;
            
        static const int
            bulletSparkFramesCount = 6,
            explosionFramesCount = 8;

        cocos2d::Sprite
            *heli[helisCount],
            *heliMainFan[helisCount],
            *heliTailFan[helisCount][2],
            *zombifiedHeli[zombifiedHelisCount],
            *zombifiedHeliMainFan[zombifiedHelisCount],
            *zombifiedHeliTailFan[zombifiedHelisCount][2],
            *bulletPart[bulletSparksCount][bulletSparkFramesCount],
            *bulletPartBG[bulletSparksCount][bulletSparkFramesCount],
            *explosion[explosionsCount][explosionFramesCount],
            *missileBuffer[missileBuffersCount],
            *infection;
            
        cocos2d::PhysicsBody *heliBody, *zombifiedHeliBody[zombifiedHelisCount], *explosionBody[explosionsCount];
        
        cocos2d::Layer *layer;
        Seed *seed;
        Tool *tool;

        struct HelicopterData {
            
            int HP, fireSpeed[2];
        } heliData[helisCount], zombifiedHeliData[zombifiedHelisCount];
        
        //// VARIABLES ////

        int
            currentSelected,
            currentZombiefiedHeli,
            currentBulletSpark = 0,
            currentExplosion = 0,
            currentMissileBuffer = 0,
            HP, fireSpeed;

        bool
            isMissileAvailable[2] = {true, true},
            isAvailableZH[zombifiedHelisCount];
};

#endif // __HELICOPTER_H__