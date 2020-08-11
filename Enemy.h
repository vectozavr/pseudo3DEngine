//
// Created by Иван Ильин on 23.07.2020.
//

#ifndef FPSONLINE_ENEMY_H
#define FPSONLINE_ENEMY_H

#include "Camera.h"
#include "ClientUDP.h"
#include "ServerUDP.h"
#include "NeuralNetwork.h"

class Enemy : public Player {
private:

    std::vector<std::vector<RayCastStructure>> v_distances;
    std::vector<CollisionInformation> allCollisions;

    double d_direction = 0;
    double d_fieldOfView;
    double d_eyesHeight;
    double d_depth;
    double d_vSpeed = 0;
    double d_wastedAmmun = 0;
    double d_path = 0;
    Point2D p_path = {0, 0};

    double d_jumpSpeed;
    double d_walkSpeed;

    World& W_world;

    std::vector<Weapon> v_weapons;
    int i_selectedWeapon = 0;

    double d_time = 0;

    void objectsRayCrossed(const std::pair<Point2D, Point2D>& ray, std::vector<RayCastStructure>& v_rayCastStruct);
    void hiddenObjectsRayCrossed(const std::pair<Point2D, Point2D>& ray, const std::string& name);

    void makeDecision(double elapsedTime);

    NeuralNetwork N_network;

public:
    explicit Enemy(World& world, Point2D position, double vPos = 0, double height = 0.6, double health = 100, std::string texture = SKIN, double fieldOfView = 3*PI/6, double eyesHeight = 0.5, double depth = 40, double walkSpeed = 3, double jumpSpeed = 2.75, double viewSpeed = .005);
    Enemy(const Enemy&) = delete;

    void update(double dt, double elapsedTime);
    void disconnect();

    void fire();

    void updateDistances();
    void connect(std::string clientIp, sf::Uint16 clientPort);

    [[nodiscard]] bool connected() const;

    double distanceAndAngleToEnemy();
    double distanceToBonus(BonusType type);
    double angleToBonus(BonusType type);

    void shiftPrecise(Point2D vector);

    void fullAmmunition() {
        v_weapons[i_selectedWeapon].set(30000);
    }

    void setInitStats() {
        d_walkSpeed = 3;
        p_path = {0, 0};
        d_path = 0;
        d_wastedAmmun = 0;
    }

    [[nodiscard]] double pathLength() const {return p_path.abs();}
    [[nodiscard]] double wastedAmmun() const {return d_wastedAmmun;}

    ClientUDP* client = nullptr;

    [[nodiscard]] const NeuralNetwork&    network() const { return N_network; }
    [[nodiscard]]       NeuralNetwork&    network()       { return N_network; }

    [[nodiscard]] double angleBetween(std::string obj) const;

    [[nodiscard]] double fov() const { return d_fieldOfView; }
};

#endif //FPSONLINE_ENEMY_H
