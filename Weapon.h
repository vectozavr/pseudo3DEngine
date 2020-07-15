//
// Created by ivan- on 07.02.2020.
//

#ifndef PSEUDO3DENGINE_WEAPON_H
#define PSEUDO3DENGINE_WEAPON_H

#include <SFML/Audio.hpp>
#include "Idrawable.h"
#include "settings.h"

class Weapon : virtual public Idrawable
{
private:
    double d_elapsedTime = 0;

    sf::Sprite S_weapon_arm;
    sf::Sprite S_weapon_handle;
    sf::Sprite S_weapon_trunk;

    sf::Sprite S_aim;
    sf::Sprite S_fire;

    sf::SoundBuffer fireSoundBuffer;
    sf::Sound fireSound;

    double d_speed = 1;
    int i_amount = 0;
    int i_damage = 0;

    double fireShift = 0;
    double d_amplitude = 100;
    double d_fireAnimTime = 0;

    double d_lastFireTime = 0;
public:
    Weapon(int amount);
    Weapon(const Weapon& weapon);

    void draw(sf::RenderTarget& window) override;

    double speed() const;
    int damage() const;

    int balance() const;

    void choiceWeapon(std::string name);
    bool fire();

    void add(double d) {
        i_amount += d;
    }
};


#endif //PSEUDO3DENGINE_WEAPON_H
