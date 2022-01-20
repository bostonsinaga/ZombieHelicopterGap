#ifndef __SEED_H__
#define __SEED_H__

#include "cocos2d.h"

USING_NS_CC;

class Seed {

    public:

        int getInstanceTag(int member) {

            static int randInt;

            if (member == 0) {

                int
                    timeDifference = std::time(0) - UserDefault::getInstance()->getIntegerForKey("START TIME"),
                    whatPat =
                         timeDifference <=  25 ? 0:
                        (timeDifference <=  50 ? 1:
                        (timeDifference <=  75 ? 2:
                        (timeDifference <= 100 ? 3:
                        (timeDifference <= 125 ? 4:4))));

                std::srand(std::time(0));
                randInt = std::rand() % patternWrap[whatPat];
            }
            
            return obsTag[randInt][member];
        }

        int getColMask(std::string designation) const {

            if      (designation == "edge")              return edge_colMask;
            else if (designation == "building")          return building_colMask;
            else if (designation == "heli")              return heli_colMask;
            else if (designation == "bullet")            return bullet_colMask;
            else if (designation == "zombie")            return zombie_colMask;
            else if (designation == "victim")            return victim_colMask;
            else if (designation == "missile pinalty")   return missilePinalty_colMask;
            else if (designation == "missile heli")      return missileHeli_colMask;
            else if (designation == "oil drum")          return oilDrum_colMask;
            else if (designation == "change weapon")     return changeWeapon_colMask;
            else if (designation == "explosion")         return explosion_colMask;
            else if (designation == "detect")            return detect_colMask;
            else if (designation == "heli zombified")    return heliZombified_colMask;
            else return 0;
        }

    private:

        static const int
            patternsCount = 50,
            instanceCountPattern = 3;

        const int
            edge_colMask = 0,
            building_colMask = 1,
            heli_colMask = 2,
            bullet_colMask = 3,
            zombie_colMask = 4,
            victim_colMask = 5,
            missilePinalty_colMask = 6,
            missileHeli_colMask = 7,
            oilDrum_colMask = 8,
            changeWeapon_colMask = 9,
            explosion_colMask = 10,
            detect_colMask = 11,
            heliZombified_colMask = 12;

        const int patternWrap[5] = {10, 20, 30, 40, 50};

        // obstacles patterns (seeds)
        const short int obsTag[patternsCount][instanceCountPattern] = {

            /*
                EXPLANATION:

                0 = blank
                1 = zombie
                2 = victim
                3 = oil drum
                4 = change weapon
                5 = zombified helicopter
            */

            // level 1
            {0, 1, 1},
            {2, 2, 0},
            {1, 0, 1},
            {2, 0, 2},
            {1, 1, 0},
            {0, 2, 2},
            {0, 2, 0},
            {0, 0, 1},
            {0, 0, 2},
            {0, 1, 0},

            // level 2
            {1, 2, 0},
            {1, 2, 0},
            {0, 1, 2},
            {0, 1, 2},
            {1, 0, 2},
            {1, 0, 2},
            {1, 1, 1},
            {2, 2, 2},
            {1, 1, 1},
            {2, 2, 2},

            // level 3
            {3, 1, 1},
            {2, 2, 2},
            {1, 3, 1},
            {1, 1, 2},
            {1, 1, 3},
            {1, 2, 2},
            {1, 3, 1},
            {1, 1, 1},
            {3, 1, 1},
            {2, 2, 2},

            // level 4
            {4, 1, 1},
            {1, 1, 1},
            {1, 4, 1},
            {1, 1, 2},
            {1, 1, 4},
            {1, 2, 2},
            {1, 4, 1},
            {1, 2, 4},
            {4, 1, 1},
            {2, 2, 2},

            // level 5
            {0, 5, 0},
            {1, 1, 1},
            {0, 5, 0},
            {2, 2, 2},
            {0, 5, 0},
            {1, 1, 1},
            {0, 5, 0},
            {2, 2, 2},
            {0, 5, 0},
            {1, 1, 1}
        };
};

#endif // __SEED_H__