//
// Created by Иван Ильин on 23.07.2020.
//

#include "Enemy.h"
#include <utility>
#include "Time.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

Enemy::Enemy(World& world, Point2D position, double vPos, double height, double health, std::string texture, double fieldOfView, double eyesHeight, double depth, double walkSpeed, double jumpSpeed, double viewSpeed) :
Player(position, vPos, height, health, std::move(texture)), W_world(world), d_fieldOfView(fieldOfView), d_eyesHeight(eyesHeight), d_jumpSpeed(jumpSpeed), d_walkSpeed(walkSpeed), d_depth(depth)
{
    Weapon weapon1(30);
    weapon1.choiceWeapon("shotgun");
    v_weapons.push_back(weapon1);
}

void Enemy::connect(std::string clientIp, sf::Uint16 clientPort) {
    client = new ClientUDP(W_world);
    client->connect(clientIp, clientPort);
}

[[nodiscard]] bool Enemy::connected() const {
    return client->connected();
}

void Enemy::disconnect() {
    if(client->isWorking())
        client->disconnect();
}

void Enemy::updateDistances() {
    v_distances.clear();
    allCollisions.clear();

    Point2D forward = { cos(d_direction), sin(d_direction) };
    Point2D left = { -forward.y, forward.x };
    double halfWidth = tan(d_fieldOfView / 2) * ((double)SCREEN_WIDTH / SCREEN_HEIGHT);

    // Visible for player segments
    for (int i = 0; i < DIS_SEG_FOR_BOTS; i++) // Need top-down map? Set limit to segs. No need? DISTANCE_SEGMENTS.
    {
        pair<Point2D, Point2D> segment1;

        //double offset = ((i * 2.0 / (DIS_SEG_FOR_BOTS - 1.0)) - 1.0) * halfWidth;
        segment1 = { {x(), y()}, {x() + d_depth * cos(i*2*PI/DIS_SEG_FOR_BOTS), y() + d_depth * sin(i*2*PI/DIS_SEG_FOR_BOTS)} };

        std::vector<RayCastStructure> v_rayCastStructure;

        objectsRayCrossed(segment1, v_rayCastStructure);

        // first 16 neirons are distances to objects around

        if (!v_rayCastStructure.empty()) {
            v_distances.push_back(v_rayCastStructure);

            double dist = v_rayCastStructure.back().distance / 10;
            dist = (dist > 1) ? 1 : dist;
            N_network.addInput(dist);
        }
        else {
            v_distances.push_back({{d_depth, 0, nullptr, 1}});
            N_network.addInput(1);
        }
    }
}
// Fire at the player
void Enemy::fire()
{
    if (v_weapons[i_selectedWeapon].fire()) {
        d_wastedAmmun++;
        client->localPlayer()->weapon().fire();
        client->localPlayer()->weapon().set(v_weapons[i_selectedWeapon].balance());

        client->localPlayer()->chDir(d_direction);
        client->localPlayer()->fire();
    }
}

void Enemy::objectsRayCrossed(const pair<Point2D, Point2D>& ray, std::vector<RayCastStructure>& v_rayCastStruct)
{
    pair<Point2D, Point2D> edge;
    Point2D nearCross;
    double len = 0;
    double closest = d_depth;
    string nearObject;
    for (auto& object : W_world.objects())
    {
        if (object.first == getName() || object.second == nullptr || object.second.get()->nodes().size() < 2)
            continue;

        // Check crossing

        Point2D crossPoint = ray.second;
        std::vector<RayCastStructure> v_reflectedRayCastStructure;

        pair<Point2D, Point2D> wall;
        // If ray crossed with object
        if (object.second.get()->cross(ray, wall, crossPoint, len))
        {
            v_rayCastStruct.push_back({ (ray.first - crossPoint).abs(), len, object.second.get(), object.second.get()->height(), v_reflectedRayCastStructure });
            // for collision
            double dist = (crossPoint - position()).abs();
            if (dist < closest)
            {
                edge = std::move(wall);
                closest = dist;
                nearCross = crossPoint;

                nearObject = object.first;
            }
        }
    }
    // Sort hits by descending of distance
    std::sort(v_rayCastStruct.begin(), v_rayCastStruct.end(), [](const RayCastStructure& lh, const RayCastStructure& rh) { return lh.distance > rh.distance; });

    // collision
    if (!nearObject.empty() && COLLISION_AREA >= closest)
    {
        CollisionInformation newCollision;
        newCollision.distance = (nearCross - position()).abs();
        newCollision.edge = std::move(edge);
        newCollision.collisionPoint = nearCross;
        newCollision.height = W_world[nearObject]->height();
        allCollisions.push_back(newCollision);
    }
    // Bonus collision
    if (!nearObject.empty() && (W_world[nearObject]->position() - position()).abs() <= COLLISION_AREA && W_world[nearObject]->type() == ObjectType::Bonus)
    {
        if(reinterpret_cast<Bonus*>(W_world[nearObject].get())->bonusType() == BonusType::TreatmentBonus)
            client->shoot(getName(), -100, 1);
        if(reinterpret_cast<Bonus*>(W_world[nearObject].get())->bonusType() == BonusType::AmmunitionBonus)
            v_weapons[i_selectedWeapon].add(30);
        if(reinterpret_cast<Bonus*>(W_world[nearObject].get())->bonusType() == BonusType::SpeedBonus) {
            if (d_walkSpeed < 7)
                d_walkSpeed += 0.5;
            if(d_jumpSpeed < 10)
                d_jumpSpeed++;
        }

        W_world.freeBonusPoint(W_world[nearObject]->position());      // free this place for another bonus
        W_world[nearObject]->setPosition(W_world.getBonusPoint());     // change the position of this bonus and mark this
    }
}

void Enemy::hiddenObjectsRayCrossed(const pair<Point2D, Point2D>& ray, const std::string& name)
{
    Object2D* obj = nullptr;
    std::pair<Point2D, Point2D> edge;
    double len = 0;
    Point2D nearCross = ray.second;
    for (auto object : W_world.objects())
    {
        if (object.second == nullptr || object.first == name || object.second.get()->nodes().size() < 2)
            continue;

        // Check collision

        Point2D crossPoint = ray.second;

        // If object hitted and near closer than already finded - rember it
        pair<Point2D, Point2D> wall;
        if (object.second.get()->cross(ray, wall, crossPoint, len) && (nearCross - ray.first).abs() > (crossPoint - ray.first).abs())
        {
            nearCross = std::move(crossPoint);
            obj = object.second.get();
            edge = std::move(wall);
        }
    }
    // If something was hitted close enough - write it
    if (obj)
    {
        CollisionInformation newCollision;
        newCollision.distance = (nearCross - position()).abs();
        newCollision.edge = std::move(edge);
        newCollision.collisionPoint = nearCross;
        newCollision.height = obj->height();
        allCollisions.push_back(newCollision);
    }
}

void Enemy::makeDecision(double elapsedTime) {

    vector<double> outLayer = N_network.getOutput();
    if(outLayer.empty())
        return;

    // -------- MAKE DECISION --------

    // enemy displacement
    double dx = 0;
    double dy = 0;
    double d_sin = sin(d_direction);
    double d_cos = cos(d_direction);

    // change direction
    d_direction += (outLayer[2] - 0.5) * d_fieldOfView;
    client->localPlayer()->chDir(d_direction);

    // fire in d_direction
    if(outLayer[3] >= 0.5)
        fire();


    dx +=  d_sin * (outLayer[0] - 0.5) * 2 + d_cos * (outLayer[1] - 0.5) * 2;
    dy += -d_cos * (outLayer[0] - 0.5) * 2 + d_sin * (outLayer[1] - 0.5) * 2;

    shiftPrecise({ dx * d_walkSpeed * elapsedTime * (health() / 100), dy * d_walkSpeed * elapsedTime * (client->localPlayer()->health() / 100) });
}

double Enemy::distanceToBonus(BonusType type) {
    double l = d_depth;

    for(auto& b : W_world.bonuses())
        if(reinterpret_cast<Bonus*>(W_world[b].get())->bonusType() == type)
            if((position() - W_world[b]->position()).abs() < l)
                l = (position() - W_world[b]->position()).abs();

    return l;
}

double Enemy::angleToBonus(BonusType type) {
    double angle = -d_fieldOfView/2;
    double l = d_depth;

    for(auto& b : W_world.bonuses())
        if(reinterpret_cast<Bonus*>(W_world[b].get())->bonusType() == type)
            if((position() - W_world[b]->position()).abs() < l) {
                l = (position() - W_world[b]->position()).abs();
                angle = (angleBetween(b) + d_fieldOfView/2)/d_fieldOfView;
            }

    return angle;
}

double Enemy::angleBetween(std::string obj) const {

    if(W_world[obj] == nullptr)
        return -d_fieldOfView;

    Point2D viewDir = {cos(d_direction), sin(d_direction)};
    Point2D objDir  = (W_world[obj]->position() - position()).normalize();

    if(viewDir*objDir < 0)
        return -d_fieldOfView/2;

    double angle = viewDir.getAngle(objDir);

    if(angle < -d_fieldOfView/2)
        angle = -d_fieldOfView/2;
    if(angle > d_fieldOfView/2)
        angle = d_fieldOfView/2;

    return angle;
}

void Enemy::update(double dt, double elapsedTime) {

    if(!client->isWorking())
        return;
    v_weapons[i_selectedWeapon].setTimeMult(dt/elapsedTime );

    N_network.clearInput();

    // 16 distances
    updateDistances();

    double near = d_depth;
    double enemyAngle = -d_fieldOfView/2;
    for(auto& p : client->localPlayer()->players()) {
        //double dist = distanceTo(p.first);

        if(client->localPlayer()->getName() == p.second->getName())
            continue;
        if((p.second->position() - position()).abs() > d_depth)
            continue;

        bool canSee = false;
        Point2D enemyDirection = (p.second->position() - position()).normalize();

        pair<Point2D, Point2D> segment1 = { {x(), y()}, {x() + d_depth*enemyDirection.x, y() + d_depth*enemyDirection.y} };
        std::vector<RayCastStructure> v_rayCastStructure;
        objectsRayCrossed(segment1, v_rayCastStructure);
        if(!v_rayCastStructure.empty() && v_rayCastStructure.back().object->getName() == p.first)
            canSee = true;

        double dist = (p.second->position() - position()).abs();

        if(dist < near && canSee) {
            near = dist;
            //enemyAngle = (angleBetween(p.first) + d_fieldOfView/2)/d_fieldOfView;
            enemyAngle = angleBetween(p.first);
        }
        if(enemyAngle == -d_fieldOfView/2 || enemyAngle == d_fieldOfView/2)
            near = d_depth;
    }
    if(enemyAngle != enemyAngle)
        enemyAngle = 0;
    /*
    // distance to the ammunition
    N_network.addInput(distanceToBonus(BonusType::AmmunitionBonus) / d_depth);
    // distance to the speedBonus
    N_network.addInput(distanceToBonus(BonusType::SpeedBonus) / d_depth);
    // distance to the health
    N_network.addInput(distanceToBonus(BonusType::TreatmentBonus) / d_depth);

    // angle to the ammunition
    N_network.addInput(angleToBonus(BonusType::AmmunitionBonus));
    // angle to the speedBonus
    N_network.addInput(angleToBonus(BonusType::SpeedBonus));
    // angle to the health
    N_network.addInput(angleToBonus(BonusType::TreatmentBonus));

    // health
    N_network.addInput((double)client->localPlayer()->health() / 100.0);
    */
    // distance to the near enemy
    N_network.addInput(near / d_depth);
    // angle to the near enemy
    N_network.addInput((enemyAngle + d_fieldOfView/2)/d_fieldOfView);
    // ammunition balance
    N_network.addInput((NeuralNetwork::sigmoid((double)v_weapons[i_selectedWeapon].balance()/10.0) - 0.5)*2);

    //d_direction += enemyAngle;

    // Based on resulting v_inLayer enemy needs to makeDecision
    makeDecision(dt);


    if(client != nullptr && client->isWorking()) {
        client->update();
    }

    if(client->localPlayer() != nullptr) {
        //client->localPlayer()->setPosition(position());
        setPosition(client->localPlayer()->position());
    }
}

void Enemy::shiftPrecise(Point2D vector) {

    for (auto& c : allCollisions)
    {
        Point2D edgeVector = c.edge.second - c.edge.first;
        Point2D normal = { edgeVector.y, -edgeVector.x };
        normal = normal.normalize();

        Point2D toWallVector = c.edge.first + c.edge.second - p_position * 2;
        if(normal * toWallVector > 0)
            normal = normal * (-1);

        double scalar = vector * normal;

        if (scalar < 0 && abs(c.distance - abs(scalar)) < COLLISION_DISTANCE && vPos() + d_eyesHeight < c.height)
        {
            vector.x -= normal.x * scalar;
            vector.y -= normal.y * scalar;
        }

    }

    p_path += vector;
    d_path += vector.abs();
    shift(vector);
    client->localPlayer()->shift(vector);
    //client->localPlayer()->setPosition(position());
}