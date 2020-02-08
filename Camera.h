//
// Created by ivan- on 18.01.2020.
//

#ifndef PSEUDO3DENGINE_CAMERA_H
#define PSEUDO3DENGINE_CAMERA_H

#include <algorithm>
#include "Object2D.h"
#include "World.h"
#include "settings.h"
#include "Menu.h"
#include "Weapon.h"

struct RGB {
    int RED = 255;
    int GREEN = 255;
    int BLUE = 255;
};

struct RayCastStructure {
    double distance;    // How far is this texture
    double progress;    // progress defines the point of texture we should load
    std::string object; // particular object. We need this to show particular texture.
    double height;      // objects has different height

    std::vector<RayCastStructure> v_mirrorRayCast; // When we have mirror, we should know about all objects we can see
};

struct CollisionInformation {
    double distance;
    Point2D collisionPoint;
    std::pair<Point2D, Point2D> edge;
};

class Camera : public Object2D {
private:
    std::vector<std::vector<RayCastStructure>> v_distances;

    //For collision detection
    std::vector<CollisionInformation> allCollisions;

    double d_direction;
    double d_fieldOfView;
    double d_depth;

    double d_walkSpeed;
    double d_viewSpeed;

    bool b_collision = true;

    World& W_world;

    sf::Vector2i localMousePosition;

    bool b_textures = true;
    bool smooth = false;

    std::vector<Weapon> weapons;
    int selectedWeapon = 0;

    void objectsRayCrossed(std::pair<Point2D, Point2D> ray, std::vector<RayCastStructure>& v_rayCastStruct, World& world, std::string name);
    void drawVerticalStrip(sf::RenderWindow& window, const RayCastStructure& obj, double distance, int shift, double s);
    void recursiveDrawing(sf::RenderWindow& window, const std::vector<RayCastStructure>& v_RayCastStructure, double distance, int shift);

    static double scalarWithNormal(Point2D edge, Point2D vector);
public:
    explicit Camera(World& world, Point2D position, double direction = 0, double fieldOfView = PI/2, double depth = 14, double walkSpeed = 1.5, double viewSpeed = .01)
    : W_world(world), Object2D(position), d_direction(direction), d_fieldOfView(fieldOfView), d_depth(depth), d_walkSpeed(walkSpeed), d_viewSpeed(viewSpeed) {
        Weapon weapon1(100);
        weapon1.choiceWeapon("shotgun");
        weapons.push_back(weapon1);
    }

    void updateDistances(World& world);
    void drawCameraView(sf::RenderWindow& window);

    void draw(sf::RenderWindow& window) override;

    bool keyboardControl(double elapsedTime, sf::RenderWindow& window);

    void shiftPrecise(Point2D vector);

    bool isSmooth() { return smooth; }
    void switchSmooth() { smooth = !smooth; }
    bool isCollision() { return b_collision; }
    void switchCollision() { b_collision = !b_collision; }
    bool isTextures() { return b_textures; }
    void switchTextures() { b_textures = !b_textures; }

    void previousWeapon();

    void nextWeapon();
};


#endif //PSEUDO3DENGINE_CAMERA_H
