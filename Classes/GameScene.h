#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Seed.h"
#include "Tool.h"
#include "GUI.h"
#include "Helicopter.h"
#include "Obstacles.h"

class GameScene : public cocos2d::LayerGradient {
    
    public:

        // there's no 'id' in cpp, so we recommend returning the class instance pointer
        static cocos2d::Scene* createScene();

        // here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
        virtual bool init();
        
        // implement the "static create()" method manually
        CREATE_FUNC(GameScene);

    private:

        ////// METHODS //////

        //// main ////

        void setPhysicsWorld(cocos2d::PhysicsWorld *world) {sceneWorld = world;}
        void contacting();
        void touching();
        void startGameTouchListener();
        void delayTouch(float dt);
        void set_isTouchable(float dt);
        void onKeyReleasing();

        // obs //

        void obs_spawn(float dt);
        void obs_spawnBG(float dt);
        void checkingCollisionObs(cocos2d::PhysicsBody *a, cocos2d::PhysicsBody *b, float obsWidth);
        void checkingFiredTarget(
    
            std::string designation,
            int hpCount,
            cocos2d::PhysicsBody *a,
            cocos2d::PhysicsBody *b
        );
        void hideCollidedInstance(std::string designation, cocos2d::PhysicsBody *a, cocos2d::PhysicsBody *b);
        void checkingHeliWreck(int hpDecrease, cocos2d::Vec2 sparkPos);
        void checkingZombifiedHeliWreck(int hpDecrease, cocos2d::PhysicsBody *a, cocos2d::PhysicsBody *b);
        void oilDrumOnContact(cocos2d::PhysicsBody *a, cocos2d::PhysicsBody *b);

        // heli //

        void heli_fire(float dt);
        void heli_punishmentMissile(float dt);
        void setHeliStatus(int statusAsInteger);

        // gui //

        void gui_noticeBoardChange(
            
            std::string designationBefore,
            std::string designationNext,
            float dt
        );

        void gui_showNoticeBoard(float dt);

        // scene //

        void startGame();
        void checkingTutorialNeeds();
        void tut_tut1(float dt);
        void tut_tut2(float dt);
        void tut_tut3(float dt);
        void tut_tut4(float dt);
        void goToPauseScene(float dt);
        void goToGameOverScene(float dt);
        cocos2d::RenderTexture *getRenTex();

        // coin //

        void coinCollect(cocos2d::Vec2 pos);

        // scoring //

        void scoreAddition(int addition = 1);
        void scoreAddition(float dt);

        ////// OBJECTS //////

        cocos2d::PhysicsWorld *sceneWorld;
        cocos2d::EventListenerTouchAllAtOnce *touchListener_multi;
        cocos2d::EventListenerTouchOneByOne *touchListener_single;

        Seed seed;
        Tool tool;
        GUI gui;
        Helicopter heli;
        Obstacles obs;

        ////// VARIABLES //////

        std::string gui_noticeBoardDesignation;

        int heliStatus[3] = {0, 0, 0}, heliTotalHP;

        bool
            isGameStarted = false,
            isTouchable = false,
            isTouched = false,
            isControlable = true,
            isBeingPunished = false,
            isContactable = true,
            isTutoring;

        cocos2d::Vec2 touchBegan, touchEnded;
};

#endif // __GAME_SCENE_H__