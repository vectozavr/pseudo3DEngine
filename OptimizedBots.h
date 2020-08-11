//
// Created by Иван Ильин on 09.08.2020.
//

#ifndef FPSONLINE_OPTIMIZEDBOTS_H
#define FPSONLINE_OPTIMIZEDBOTS_H

#include <string>
#include "World.h"
#include "Enemy.h"


class OptimizedBots {
private:
    std::shared_ptr<World> W_world;
    std::map<const int, std::shared_ptr<Enemy>> v_generation;
public:
    explicit OptimizedBots(std::string map, int number);
};


#endif //FPSONLINE_OPTIMIZEDBOTS_H
