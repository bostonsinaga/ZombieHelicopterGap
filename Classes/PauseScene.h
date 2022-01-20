#ifndef __PAUSE_SCENE_H__
#define __PAUSE_SCENE_H__

#include "cocos2d.h"
#include "Tool.h"
#include "GUI.h"

class PauseScene : public cocos2d::Layer {
    
    public:

        // there's no 'id' in cpp, so we recommend returning the class instance pointer
        static cocos2d::Scene* createScene(cocos2d::RenderTexture *renTex);

        // here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
        virtual bool init();
        
        // implement the "static create()" method manually
        CREATE_FUNC(PauseScene);

    private:

        // METHODS //

        void touching();
        void delayTouch(float dt);
        void set_isTouchable(float dt);
        void onKeyReleasing();

        // gui //

        void gui_noticeBoardChange(
    
            std::string designationBefore,
            std::string designationNext,
            float dt
        );

        void gui_showNoticeBoard(float dt);

        // pop game scene //

        void prepareToPopGameScene();
        void popGameScene(float dt);

        // OBJECTS //

        cocos2d::EventListenerTouchOneByOne *touchListener;
        cocos2d::EventListenerKeyboard *backButtonListener;

        Tool tool;
        GUI gui;

        // VARIABLES //

        std::string gui_noticeBoardDesignation;

        int pausedStartTime = std::time(0);

        bool
            isTouchable = false,
            isTouched = false,
            isPreparedPop = false;

        cocos2d::Vec2 touchBegan, touchEnded;
};

#endif // __PAUSE_SCENE_H__