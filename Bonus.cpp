//
// Created by Иван Ильин on 15.07.2020.
//

#include "Bonus.h"



Bonus::Bonus(Point2D position, BonusType bonusType, const std::string texture) : Object2D(position, {{-0.3, -0.3}, {0.3, -0.3}, {0.3, 0.3}, {-0.3, 0.3}}, 0.5, texture), bType(bonusType) {

}

ObjectType Bonus::type() {
    return ObjectType::Bonus;
}