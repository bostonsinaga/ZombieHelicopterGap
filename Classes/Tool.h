#ifndef _TOOL_H_
#define _TOOL_H_

#include "cocos2d.h"
#include "AudioEngine.h"
#include "Seed.h"

USING_NS_CC;

class Tool {

    public:

        Sprite *mySprite(std::string filePath) {

            auto sprite = Sprite::create(filePath);
            sprite->setContentSize(sprite->getContentSize() * getUDef()->getFloatForKey("SCALE RATE"));
            return sprite;
        }

        Label *myLabel(std::string str, float height = 0.0) {

            if (height == 0.0) height = getBSz().height;
            auto label = Label::createWithSystemFont(str, "", height);
            return label;
        }

        void mySpriteFrames(
            
            Sprite **sprite,
            int count,
            const char *format,
            int z_order,
            Node *parent
        ) {

            char name[100];

            for (int i = 0; i < count; i++) {
                
                std::sprintf(name, format, i + 1);
                sprite[i] = mySprite(name);
                sprite[i]->setOpacity(0);

                if (i == 0) {

                    sprite[i]->setPosition(pos(-0.5, 2.0));
                    parent->addChild(sprite[i], z_order);
                }
                else {
                    
                    sprite[i]->setPosition(sprite[0]->getContentSize() / 2);
                    sprite[0]->addChild(sprite[i]);
                }
            }
        }

        void mySpriteAnimation(
            
            Sprite **sprite,
            int count,
            float dt,
            Vec2 pos,
            bool isColorize = false,
            Color3B color = Color3B::WHITE
        ) {

            for (int i = 0; i < count; i++) {

                if (isColorize) sprite[i]->setColor(color);
                if (i == 0) sprite[i]->setPosition(pos);
                sprite[i]->runAction(Sequence::create(

                    DelayTime::create(i * dt), 
                    FadeIn::create(0),
                    DelayTime::create(dt),
                    FadeOut::create(0),
                    nullptr
                ));
            }
        }

        void setUserValue(

            const char *dataName,
            int value,
            int *valueName = nullptr,
            int defaultValue = 0
        ) {

            uDef->setIntegerForKey(dataName, value);
            uDef->flush();
            if (valueName != nullptr) *valueName = uDef->getIntegerForKey(dataName, defaultValue);
        }

        void setUserValue(

            const char *dataName,
            bool value,
            bool *valueName = nullptr,
            bool defaultValue = false
        ) {

            uDef->setIntegerForKey(dataName, value);
            uDef->flush();
            if (valueName != nullptr) *valueName = uDef->getIntegerForKey(dataName, defaultValue);
        }

        void currentObjectCounter(int *currentTag, int total) {

            int tag = *currentTag;
            tag++;
            *currentTag = tag;
            if (*currentTag >= total) *currentTag = 0;
        }

        Size getBSz() {return Size(120, 120) * uDef->getFloatForKey("SCALE RATE");} // brick size
        Size getVSz() {return vSize;}

        bool isAtTouchPos(Node *node, char touchCondition, Vec2 began, Vec2 ended) {

            /*
                'b' for Began
                'e' for Began with Ended
            */
            if (touchCondition == 'b') {

                if (began.x >= node->getPosition().x - node->getContentSize().width / 2 && began.x <= node->getPosition().x + node->getContentSize().width / 2
                    && began.y >= node->getPosition().y - node->getContentSize().height / 2 && began.y <= node->getPosition().y + node->getContentSize().height / 2) {

                    return true;
                }
                else return false;
            }
            else if (touchCondition == 'e') {

                if (began.x >= node->getPosition().x - node->getContentSize().width / 2 && began.x <= node->getPosition().x + node->getContentSize().width / 2
                    && began.y >= node->getPosition().y - node->getContentSize().height / 2 && began.y <= node->getPosition().y + node->getContentSize().height / 2
                    && ended.x >= node->getPosition().x - node->getContentSize().width / 2 && ended.x <= node->getPosition().x + node->getContentSize().width / 2
                    && ended.y >= node->getPosition().y - node->getContentSize().height / 2 && ended.y <= node->getPosition().y + node->getContentSize().height / 2) {

                    return true;
                }
                else return false;
            }
            else return false;
        }

        bool isAtContact(int colMask_1, int colMask_2, PhysicsBody *a, PhysicsBody *b) {

            if ((colMask_1 == a->getCollisionBitmask() && colMask_2 == b->getCollisionBitmask())
                || (colMask_1 == b->getCollisionBitmask() && colMask_2 == a->getCollisionBitmask()))
                return true;
            else return false;
        }

        Vec2 ctrPos() {

            return Point(vSize.width / 2 + vOrigin.x, vSize.height / 2 + vOrigin.y);
        }

        Vec2 pos(float x, float y) {

            return Point(vSize.width * x + vOrigin.x, vSize.height * y + vOrigin.y);
        }

        Vec2 getInstanceMovement() {

            return Vec2(-getBSz().width * (24 + 1/3) * 3/2, 0);
        }

        Node *getCollidedInstance(std::string designation, PhysicsBody *a, PhysicsBody *b, Seed *seed) {

            Node *instance = seed->getColMask(designation) == a->getCollisionBitmask() ? a->getNode() : b->getNode();
            return instance;
        }

        UserDefault *getUDef() {return uDef;}

        // AUDIO ENGINE //

        void playBGMusic(bool isPlay) {

            if (isPlay) setUserValue("ID BG MUSIC", AudioEngine::play2d("SOUNDS/zombies_horde.mp3", true, 1.0f));
            else AudioEngine::stop(uDef->getIntegerForKey("ID BG MUSIC"));
        }

        void playFanSound(bool isPlay) {

            if (isPlay) setUserValue("ID FAN SOUND", AudioEngine::play2d("SOUNDS/fan.mp3", true, 0.5f));
            else AudioEngine::stop(uDef->getIntegerForKey("ID FAN SOUND"));
        }

        void playSFX(std::string designation, float volRate = 1.0) {

            if (uDef->getBoolForKey("SOUND ON")) {
                
                if      (designation == "bullet fire")    AudioEngine::play2d("SOUNDS/bullet_fire.mp3",    false, volRate);
                else if (designation == "bullet receive") AudioEngine::play2d("SOUNDS/bullet_receive.mp3", false, volRate);
                else if (designation == "carnage")        AudioEngine::play2d("SOUNDS/carnage.mp3",        false, volRate);
                else if (designation == "change weapon")  AudioEngine::play2d("SOUNDS/change_weapon.mp3",  false, volRate);
                else if (designation == "coin")           AudioEngine::play2d("SOUNDS/coin.mp3",           false, volRate);
                else if (designation == "explode")        AudioEngine::play2d("SOUNDS/explode.mp3",        false, volRate);
                else if (designation == "fan")            AudioEngine::play2d("SOUNDS/fan.mp3",            false, volRate);
                else if (designation == "frict")          AudioEngine::play2d("SOUNDS/frict.mp3",          false, volRate);
                else if (designation == "missile launch") AudioEngine::play2d("SOUNDS/missile_launch.mp3", false, volRate);
                else if (designation == "new high score") AudioEngine::play2d("SOUNDS/new_high_score.mp3", false, volRate);
                else if (designation == "notice")         AudioEngine::play2d("SOUNDS/notice.mp3",         false, volRate);
                else if (designation == "pick")           AudioEngine::play2d("SOUNDS/pick.mp3",           false, volRate);
                else if (designation == "punishment")     AudioEngine::play2d("SOUNDS/punishment.mp3",     false, volRate);
                else if (designation == "purchased")      AudioEngine::play2d("SOUNDS/purchased.mp3",      false, volRate);
                else if (designation == "select heli")    AudioEngine::play2d("SOUNDS/select_heli.mp3",    false, volRate);
                else if (designation == "select off")     AudioEngine::play2d("SOUNDS/select_off.mp3",     false, volRate);
                else if (designation == "select on")      AudioEngine::play2d("SOUNDS/select_on.mp3",      false, volRate);
            }
        }

    private:

        Size vSize = Director::getInstance()->getVisibleSize();
        Vec2 vOrigin = Director::getInstance()->getVisibleOrigin();
        UserDefault *uDef = UserDefault::getInstance();
};

#endif // _TOOL_H_