//
// Created by Neirokan on 11.05.2020.
//

#ifndef PSEUDO3DENGINE_PLAYER_H
#define PSEUDO3DENGINE_PLAYER_H

#include "Circle2D.h"

class Player : public Circle2D
{
private:
    double d_health; // player health
    double d_vPos;   // player vertical (height) position

public:
    explicit Player(Point2D position, double vPos = 0, double height = 0.6, double health = 100, std::string texture = SKIN);
    Player(const Player&) = delete;//Player(const Player& player);

    // Return ObjectType::Player
    ObjectType type() override;

    // Check the intersection of the ray with player.
    // Returns true if ray intersects with player, false otherwise.
    // wall - hitted segment.
    // point - hit point.
    // uv - distance from the beginning of segment to the hit point.
    bool cross(const std::pair<Point2D, Point2D>& ray, std::pair<Point2D, Point2D>& wall, Point2D& point, double& uv) override;

    double vPos() const;
    void setVPos(double vPos);

    [[nodiscard]] double health() const;
    bool reduceHealth(double damage);
    void fullHealth();
    void setHealth(double h);
};


#endif //PSEUDO3DENGINE_PLAYER_H
