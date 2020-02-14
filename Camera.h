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

class Camera : public Circle2D {
private:
    std::vector<std::vector<RayCastStructure>> v_distances;

    //For collision detection
    std::vector<CollisionInformation> allCollisions;

    double d_direction;
    double d_fieldOfView;
    double d_depth;

    double d_walkSpeed;
    double d_viewSpeed;

    int i_health;

    bool b_collision = true;

    World& W_world;

    sf::Vector2i localMousePosition;

    bool b_textures = true;
    bool b_smooth = false;

    std::vector<Weapon> v_weapons;
    int i_selectedWeapon = 0;

    sf::SoundBuffer walkSoundBuffer;
    sf::Sound walkSound;

    std::string s_lastKill;

    void objectsRayCrossed(std::pair<Point2D, Point2D> ray, std::vector<RayCastStructure>& v_rayCastStruct, const std::string& name, int reflections = 0);
    void drawVerticalStrip(sf::RenderWindow& window, const RayCastStructure& obj, int shift, double s);
    void recursiveDrawing(sf::RenderWindow& window, const std::vector<RayCastStructure>& v_RayCastStructure, int shift);
    static void recursiveIncreaseDistance(std::vector<RayCastStructure>& v_RayCastStructure, double distance);

    static double scalarWithNormal(Point2D edge, Point2D vector);

    void fire();
    std::pair<std::string, double> cameraRayCheck(RayCastStructure& structure);
public:
    explicit Camera(World& world, Point2D position, double direction = 0, std::string texture = SKIN, int health = 100, double fieldOfView = PI/2, double depth = 14, double walkSpeed = 1.5, double viewSpeed = .01)
    : W_world(world), Circle2D(COLLISION_DISTANCE, position, 0.5, texture, 4), d_direction(direction), d_fieldOfView(fieldOfView), d_depth(depth), d_walkSpeed(walkSpeed), d_viewSpeed(viewSpeed), i_health(health) {
        Weapon weapon1(1000);
        weapon1.choiceWeapon("shotgun");
        v_weapons.push_back(weapon1);

        walkSoundBuffer.loadFromFile(WALK_SOUND);
        walkSound.setBuffer(walkSoundBuffer);
        walkSound.setLoop(true);
        walkSound.setVolume(50.f);
    }

    Camera(const Camera& camera) : W_world(camera.W_world) { // copy constructor
        v_distances = camera.v_distances;
        allCollisions = camera.allCollisions;
        d_direction = camera.d_direction;
        d_depth = camera.d_depth;
        d_fieldOfView = d_fieldOfView;
        d_walkSpeed = camera.d_walkSpeed;
        d_viewSpeed = camera.d_viewSpeed;
        i_health = camera.i_health;
        b_collision = camera.b_collision;
        b_textures = camera.b_textures;
        b_smooth = camera.b_smooth;
        localMousePosition = camera.localMousePosition;
        v_weapons = camera.v_weapons;
        i_selectedWeapon = camera.i_selectedWeapon;
        walkSoundBuffer = camera.walkSoundBuffer;
        walkSound = camera.walkSound;
        setName(camera.getName());
    }

    void updateDistances(const World& world);
    void drawCameraView(sf::RenderWindow& window);

    void draw(sf::RenderWindow& window) override;

    bool keyboardControl(double elapsedTime, sf::RenderWindow& window);

    void shiftPrecise(Point2D vector);

    bool isSmooth() { return b_smooth; }
    void switchSmooth() { b_smooth = !b_smooth; }
    bool isCollision() { return b_collision; }
    void switchCollision() { b_collision = !b_collision; }
    bool isTextures() { return b_textures; }
    void switchTextures() { b_textures = !b_textures; }

    void previousWeapon();
    void nextWeapon();

    int health(){ return i_health; }
    bool reduceHealth(int damage = 0);
    void fullHealth () { i_health = 100; }

    int type() override { return 1; }
    std::string lastKill() { return s_lastKill;}
    void cleanLastKill(){s_lastKill = "";}
};


#endif //PSEUDO3DENGINE_CAMERA_H
