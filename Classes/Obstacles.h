#ifndef __OBSTACLES_H__
#define __OBSTACLES_H__

#include "cocos2d.h"
#include "Seed.h"
#include "Tool.h"
#include "Helicopter.h"

class Obstacles {

    public :

        // METHODS //

        void init(cocos2d::Layer *layer_in, Seed *seed_in, Tool *tool_in, Helicopter *heli_in);
        void spawn(bool isObs);
        void carnage(cocos2d::Sprite *target, int targetTag);

        void zombieInit();
        void zombieSpawn(cocos2d::Vec2 obsPos, cocos2d::Size obsSize, int posRate);
        bool zombieCarnage();

        void victimInit();
        void victimSpawn(cocos2d::Vec2 obsPos, cocos2d::Size obsSize, int posRate);
        bool victimCarnage();

        void oilDrumInit();
        void oilDrumSpawn(cocos2d::Vec2 obsPos, cocos2d::Size obsSize, int posRate);

        void changeWeaponInit();
        void changeWeaponSpawn(cocos2d::Vec2 obsPos, cocos2d::Size obsSize, int posRate);
        
    private:

        // OBJECTS //

        static const int
            instanceCarnagesCount = 3,
            zombiesCount = 21,
            victimsCount = 21,
            oilDrumsCount = 7,
            changeWeaponsCount = 7;

        static const int
            instanceCarnageFramesCount = 6;

        cocos2d::Layer *layer;

        cocos2d::Sprite
            *instanceCarnage_bg[instanceCarnagesCount][instanceCarnageFramesCount],
            *instanceCarnage[instanceCarnagesCount][instanceCarnageFramesCount],
            *zombie[zombiesCount],
            *victim[victimsCount],
            *oilDrum[oilDrumsCount],
            *changeWeapon[changeWeaponsCount];

        cocos2d::PhysicsBody
            *zombieBody[zombiesCount],
            *victimBody[victimsCount],
            *oilDrumBody[oilDrumsCount],
            *changeWeaponBody[changeWeaponsCount];

        cocos2d::Color3B instanceCarnageColor[2][2] = {

            {Color3B( 97 - 32, 155 - 32,  36 - 32), Color3B( 97, 155,  36)}, // 0 = bg, 1 = fr
            {Color3B(255 - 32, 127 - 32,  32 - 32), Color3B(255, 127,   0)}, // ~
        };

        Seed *seed;
        Tool *tool;
        Helicopter *heli;

        // VARIABLES //

        int
            currentInstanceCarnage = 0,
            currentZombie = 0,
            currentVictim = 0,
            currentOilDrum = 0,
            currentChangeWeapon = 0;
};

#endif // __OBSTACLES_H__