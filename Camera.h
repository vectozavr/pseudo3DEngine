//
// Created by ivan- on 18.01.2020.
//

#ifndef PSEUDO3DENGINE_CAMERA_H
#define PSEUDO3DENGINE_CAMERA_H

#include "Object2D.h"
#include "World.h"
#include "settings.h"
#include "Menu.h"

struct RGB {
    int RED = 255;
    int GREEN = 255;
    int BLUE = 255;
};

struct TextureParameters {
    double distance;    // How far is this texture
    double progress;    // progress defines the point of texture we should load
    std::string object; // particular object. We need this to show particular texture.
};

struct CollisionInformation {
    double distance;
    Point2D collisionPoint;
    std::pair<Point2D, Point2D> edge;
};

class Weapon : virtual public Idrawable {
private:
    sf::Texture T_weapon;
    sf::Sprite S_weapon;
    double d_speed = 1;
    int i_amount = 0;

    sf::Sprite S_aim;
    sf::Texture T_aim;
public:
    Weapon(int amount) : i_amount(amount) { };

    Weapon(const Weapon& weapon) {
        this->i_amount = weapon.i_amount;
        this->d_speed = weapon.d_speed;
        this->S_weapon = weapon.S_weapon;
        this->T_weapon = weapon.T_weapon;
        this->T_aim = weapon.T_aim;
        this->S_aim = weapon.S_aim;

        S_weapon.setTexture(T_weapon);
        S_weapon.setPosition(sf::Vector2f(SCREEN_WIDTH - S_weapon.getTextureRect().width,SCREEN_HEIGHT - S_weapon.getTextureRect().height)); // абсолютная позиция
    }

    void choiceWeapon(std::string name) {
        if(name == "shotgun") {
            T_weapon.loadFromFile(SHOTGUN_TEXTURE);
            T_aim.loadFromFile(AIM_TEXTURE);
            d_speed = 1;
            S_aim.scale(.1, .1);
        }
    }

    bool fire() {
        if(i_amount > 0) {
            --i_amount;
            return true;
        }
        return false;
    }
    double speed() const {
        return d_speed;
    }

    void draw(sf::RenderWindow& window) override {
        S_weapon.setTexture(T_weapon);
        S_weapon.setPosition(sf::Vector2f(SCREEN_WIDTH - S_weapon.getTextureRect().width, SCREEN_HEIGHT - S_weapon.getTextureRect().height)); // абсолютная позиция
        window.draw(S_weapon);

        S_aim.setTexture(T_aim);
        S_aim.setPosition(sf::Vector2f(SCREEN_WIDTH/2 - S_aim.getTextureRect().width/20, SCREEN_HEIGHT/2 - S_aim.getTextureRect().height/20)); // абсолютная позиция
        window.draw(S_aim);
    }
};


class Camera : public Object2D {
private:
    std::vector<TextureParameters> v_distances;

    //For collision detection
    std::vector<CollisionInformation> allCollisions;

    double d_direction;
    double d_fieldOfView;
    double d_depth;

    double d_walkSpeed;
    double d_viewSpeed;

    bool b_collision = false;

    World& W_world;

    sf::Vector2i localMousePosition;

    bool b_textures = true;
    bool smooth = false;

    void checkCollisions();

    std::vector<Weapon> weapons;
    int selectedWeapon = 0;
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

    void previousWeapon() {
        if(selectedWeapon > 0)
            selectedWeapon--;
        else
            selectedWeapon = weapons.size()-1;
    }
    void nextWeapon() {
        if(selectedWeapon < weapons.size()-1)
            selectedWeapon++;
        else
            selectedWeapon = 0;
    }
};


#endif //PSEUDO3DENGINE_CAMERA_H
