//
// Created by Иван Ильин on 15.07.2020.
//

#ifndef FPSONLINE_BONUS_H
#define FPSONLINE_BONUS_H

#include "Object2D.h"
#include "settings.h"

enum class BonusType
{
    TreatmentBonus,
    AmmunitionBonus,
    SpeedBonus,
    ViewBonus,
};

class Bonus : public Object2D
{
private:
    BonusType bType;

public:
    explicit Bonus(Point2D position = { 0, 0 }, BonusType bonusType = BonusType::TreatmentBonus, const std::string texture = HEALTH_TEXTURE);

    ObjectType type() override;

    [[nodiscard]] BonusType bonusType() const {
        return bType;
    }
};


#endif //FPSONLINE_BONUS_H
