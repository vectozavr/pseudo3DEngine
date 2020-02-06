//
// Created by ivan- on 18.01.2020.
//

#ifndef PSEUDO3DENGINE_CAMERA_H
#define PSEUDO3DENGINE_CAMERA_H

#include <chrono>
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
    double d_elapsedTime;

    sf::Texture T_weapon_arm;
    sf::Sprite S_weapon_arm;
    sf::Texture T_weapon_handle;
    sf::Sprite S_weapon_handle;
    sf::Texture T_weapon_trunk;
    sf::Sprite S_weapon_trunk;

    sf::Sprite S_aim;
    sf::Texture T_aim;
    sf::Sprite S_fire;
    sf::Texture T_fire;
    double d_speed = 1;
    int i_amount = 0;

    double fireShift = 0;
    double d_amplitude = 100;
    double d_fireAnimTime = 0;

    double d_lastFireTime = 0;
public:
    Weapon(int amount) : i_amount(amount) { };

    Weapon(const Weapon& weapon) {
        this->i_amount = weapon.i_amount;
        this->d_speed = weapon.d_speed;

        this->S_weapon_arm = weapon.S_weapon_arm;
        this->T_weapon_arm = weapon.T_weapon_arm;
        this->S_weapon_handle = weapon.S_weapon_handle;
        this->T_weapon_handle = weapon.T_weapon_handle;
        this->S_weapon_trunk = weapon.S_weapon_trunk;
        this->T_weapon_trunk = weapon.T_weapon_trunk;

        this->S_aim = weapon.S_aim;
        this->T_aim = weapon.T_aim;
        this->S_fire = weapon.S_fire;
        this->T_fire = weapon.T_fire;

        this->d_amplitude = weapon.d_amplitude;

        S_weapon_handle.setTexture(T_weapon_handle);
    }

    void choiceWeapon(std::string name) {
        if(name == "shotgun") {
            T_weapon_arm.loadFromFile(SHOTGUN_ARM_TEXTURE);
            T_weapon_handle.loadFromFile(SHOTGUN_HANDLE_TEXTURE);
            T_weapon_trunk.loadFromFile(SHOTGUN_TRUNK_TEXTURE);

            T_aim.loadFromFile(AIM_TEXTURE);
            T_fire.loadFromFile(FIRE_SHOTGUN_TEXTURE);
            d_speed = 1;
            S_aim.scale(.1, .1);
        }
    }

    bool fire() {
        if((i_amount > 0) && (d_lastFireTime == 0)) {
            d_lastFireTime = d_speed;
            d_fireAnimTime = .1;
            --i_amount;
            fireShift = d_amplitude;
            return true;
        }
        return false;
    }
    double speed() const {
        return d_speed;
    }

    void draw(sf::RenderWindow& window) override {
        auto tp = std::chrono::system_clock::now();
        std::chrono::duration <double> elapsedTime = tp.time_since_epoch();
        double diff = elapsedTime.count() - d_elapsedTime;
        d_elapsedTime = elapsedTime.count();


        if(fireShift > 0)
            fireShift -= 4*d_amplitude*diff/d_speed;
        else
            fireShift = 0;
        double shift = 15*(1 + cos(3*d_elapsedTime)); // Motion of the weapon

        Point2D weaponPosition = {SCREEN_WIDTH - S_weapon_handle.getTextureRect().width + shift + fireShift, SCREEN_HEIGHT - S_weapon_handle.getTextureRect().height + shift + fireShift};

        // FIRE EFFECT HERE
        d_lastFireTime -= diff; // Time when you can't fire
        if(d_lastFireTime < 0)
            d_lastFireTime = 0;

        d_fireAnimTime -= diff;
        if(d_fireAnimTime > 0) {
            S_fire.setTexture(T_fire);
            S_fire.setPosition(sf::Vector2f(weaponPosition.x - 490, weaponPosition.y - 792)); // абсолютная позиция
            window.draw(S_fire);
        }

        // All about weapon

        double d_armShift = 50*sin(PI*4*d_lastFireTime/(3*d_speed));
        if(d_lastFireTime <= 0 || d_lastFireTime > 3*d_speed/4)
            d_armShift = 0;

        S_weapon_arm.setTexture(T_weapon_arm);
        S_weapon_handle.setTexture(T_weapon_handle);
        S_weapon_trunk.setTexture(T_weapon_trunk);
        S_weapon_arm.setPosition(sf::Vector2f(weaponPosition.x + d_armShift, weaponPosition.y + d_armShift)); // Shift arm when we shoot
        S_weapon_handle.setPosition(sf::Vector2f(weaponPosition.x, weaponPosition.y));
        S_weapon_trunk.setPosition(sf::Vector2f(weaponPosition.x, weaponPosition.y));

        window.draw(S_weapon_trunk);
        window.draw(S_weapon_arm);
        window.draw(S_weapon_handle);

        //

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

    bool b_collision = true;

    World& W_world;

    sf::Vector2i localMousePosition;

    bool b_textures = true;
    bool smooth = false;

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
