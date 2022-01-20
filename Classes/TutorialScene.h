#ifndef __TUTORIAL_SCENE_H__
#define __TUTORIAL_SCENE_H__

#include "cocos2d.h"
#include "Tool.h"
#include "GUI.h"

class TutorialScene : public cocos2d::Layer {
    
    public:

        // there's no 'id' in cpp, so we recommend returning the class instance pointer
        static cocos2d::Scene* createScene(cocos2d::RenderTexture *renTex, int tutorialTag_in);

        // here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
        virtual bool init();
        
        // implement the "static create()" method manually
        CREATE_FUNC(TutorialScene);

    private:

        // METHODS //

        void actionLabel(float dt);
        void actionFinger(float dt);

        void passNotice(float dt);
        void passAction(float dt);

        void generateVisualTut();
        void removeVisualTut();

        void touching();
        void popGameScene();
        void delayTouch(float dt);
        void set_isTouchable(float dt);
        void onKeyReleasing();

        // OBJECTS //

        cocos2d::EventListenerTouchOneByOne *touchListener;
        cocos2d::EventListenerKeyboard *backButtonListener;

        cocos2d::DrawNode *darkScreen, *bullet;
        cocos2d::Label *label, *passLabel;

        cocos2d::Sprite 
            *finger,
            *heliTut,
            *heliMainFanTut,
            *heliTailFanTut[2],
            *victimTut,
            *zombieTut,
            *trueSign,
            *falseSign;

        Tool tool;
        GUI gui;

        // VARIABLES //

        bool isTouchable = false, isVictim = true;
        int pausedStartTime = std::time(0);
};

#endif // __TUTORIAL_SCENE_H__