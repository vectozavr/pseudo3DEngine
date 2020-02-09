//
// Created by ivan- on 07.02.2020.
//

#ifndef PSEUDO3DENGINE_WEAPON_H
#define PSEUDO3DENGINE_WEAPON_H
#include <SFML/Audio.hpp>
#include <chrono>
#include "settings.h"
#include "Object2D.h"

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

    sf::SoundBuffer fireSoundBuffer;
    sf::Sound fireSound;

    double d_speed = 1;
    int i_amount = 0;

    double fireShift = 0;
    double d_amplitude = 100;
    double d_fireAnimTime = 0;

    double d_lastFireTime = 0;
public:
    Weapon(int amount) : i_amount(amount) { };

    Weapon(const Weapon& weapon);

    void choiceWeapon(std::string name);

    bool fire();

    double speed() const { return d_speed; }

    void draw(sf::RenderWindow& window) override;
};

#endif //PSEUDO3DENGINE_WEAPON_H
