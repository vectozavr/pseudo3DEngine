//
// Created by ivan- on 18.01.2020.
//

#ifndef PSEUDO3DENGINE_CAMERA_H
#define PSEUDO3DENGINE_CAMERA_H

#include "Player.h"
#include "World.h"
#include "settings.h"
#include "Weapon.h"
#include <SFML/System.hpp>

class ClientUDP;

struct RayCastStructure
{
    double distance;    // How far is this texture
    double progress;    // progress defines the point of texture we should load
    Object2D* object;   // hitted object. We need this to get it's texture.
    double height;      // objects has different height

    std::vector<RayCastStructure> v_mirrorRayCast; // When we have mirror, we should know about all objects we can see
};

struct CollisionInformation
{
    double distance;
    Point2D collisionPoint;
    std::pair<Point2D, Point2D> edge;
    double height = 0;
};

class Camera : public Player
{
private:
    double mirrorTop = -INFINITY;
    double mirrorBot = INFINITY;
    double directionSin = 0;
    double directionCos = 0;
    double horizontalCos[DISTANCES_SEGMENTS];
    double horizontalSin[DISTANCES_SEGMENTS];
    double verticalTan[SCREEN_HEIGHT];

    std::vector<std::vector<RayCastStructure>> v_distances;
    std::vector<CollisionInformation> allCollisions;
    std::map<std::string, std::shared_ptr<Player>> m_playersOnTheScreen;

    double d_direction;
    double d_fieldOfView;
    double d_eyesHeight;
    double d_depth;
    double d_vSpeed = 0;

    double d_jumpSpeed;
    double d_walkSpeed;
    double d_viewSpeed;

    bool b_collision = true;
    bool b_hadFocus = false;

    World& W_world;

    sf::Vector2i localMousePosition;

    bool b_textures = false;
    bool b_smooth = false;

    std::vector<Weapon> v_weapons;
    int i_selectedWeapon = 0;

    sf::Sound walkSound;

    void objectsRayCrossed(const std::pair<Point2D, Point2D>& ray, std::vector<RayCastStructure>& v_rayCastStruct, const std::string& name, int reflections = 0);
    void hiddenObjectsRayCrossed(const std::pair<Point2D, Point2D>& ray, const std::string& name);
    void drawVerticalStrip(sf::RenderTarget& window, const RayCastStructure& obj, int shift, int f);
    void recursiveDrawing(sf::RenderTarget& window, const std::vector<RayCastStructure>& v_RayCastStructure, int shift, int rec = 1);
    static void recursiveIncreaseDistance(std::vector<RayCastStructure>& v_RayCastStructure, double distance);
    std::pair<double, double> heightInPixels(double distance, double height, double vertical);

    static double scalarWithNormal(Point2D edge, Point2D vector);

    void fire();
    std::pair<Object2D*, double> cameraRayCheck(RayCastStructure& structure);

    static void drawHealth(sf::RenderTarget& window, int x, int y, int width, int health);

    void updateDistances_from_to(int from, int to);
    void updateHiddenDistances_from_to(int from, int to);

    void recursiveDrawing_from_to(sf::RenderTarget* window, int from, int to);

    std::mutex m;

public:
    ClientUDP* client = nullptr;

    explicit Camera(World& world, Point2D position, double vPos = 0, double height = 0.6, double direction = PI, double health = 100, std::string texture = SKIN, double fieldOfView = PI/4, double eyesHeight = 0.5, double depth = 40, double walkSpeed = 3, double jumpSpeed = 2.75, double viewSpeed = .005);
    Camera(const Camera&) = delete;//Camera(const Camera& camera);

    void addPlayer(std::string name, std::shared_ptr<Player> camera);
    void removePlayer(const std::string& name);

    bool isSmooth();
    void setSmooth(bool active);
    bool isCollision();
    void setCollision(bool active);
    bool isTextures();
    void setTextures(bool active);

    void setFieldOfView(double angle);

    bool keyboardControl(double elapsedTime, sf::RenderWindow& window);
    void updateDistances(const World& world);
    void drawCameraView(sf::RenderTarget& window);
    void draw(sf::RenderTarget& window) override;

    void shiftPrecise(Point2D vector, double vertical = 0);

    void previousWeapon();
    void nextWeapon();
};


#endif //PSEUDO3DENGINE_CAMERA_H
