//
// Created by ivan- on 07.02.2020.
//

#include "Weapon.h"

Weapon::Weapon(const Weapon &weapon) {
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

void Weapon::choiceWeapon(std::string name) {
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

bool Weapon::fire() {
    if((i_amount > 0) && (d_lastFireTime == 0)) {
        d_lastFireTime = d_speed;
        d_fireAnimTime = .1;
        --i_amount;
        fireShift = d_amplitude;
        return true;
    }
    return false;
}

void Weapon::draw(sf::RenderWindow &window) {
    auto tp = std::chrono::system_clock::now();
    std::chrono::duration <double> elapsedTime = tp.time_since_epoch();
    double diff = elapsedTime.count() - d_elapsedTime;
    d_elapsedTime = elapsedTime.count();


    if(fireShift > 0)
        fireShift -= 2*d_amplitude*diff/d_speed;
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

    double d_armShift = 50*sin(PI*2*d_lastFireTime/d_speed);
    if(d_lastFireTime <= 0 || d_lastFireTime > d_speed/2)
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
