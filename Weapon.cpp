//
// Created by ivan- on 07.02.2020.
//
#include <cmath>
#include "Weapon.h"
#include "ResourceManager.h"
#include "Point2D.h"
#include <chrono>

Weapon::Weapon(int amount) : i_amount(amount)
{
}

Weapon::Weapon(const Weapon& weapon)
{
    this->i_amount = weapon.i_amount;
    this->d_speed = weapon.d_speed;

    this->S_weapon_arm = weapon.S_weapon_arm;
    this->S_weapon_handle = weapon.S_weapon_handle;
    this->S_weapon_trunk = weapon.S_weapon_trunk;

    this->S_aim = weapon.S_aim;
    this->S_fire = weapon.S_fire;

    this->fireSoundBuffer = weapon.fireSoundBuffer;
    this->fireSound = weapon.fireSound;
    this->fireSound.setBuffer(this->fireSoundBuffer);

    this->d_amplitude = weapon.d_amplitude;

    this->i_damage = weapon.i_damage;
}

double Weapon::speed() const
{
    return d_speed;
}

int Weapon::damage() const
{
    return i_damage;
}

void Weapon::choiceWeapon(std::string name)
{
    if (name == "shotgun")
    {
        S_weapon_arm.setTexture(*ResourceManager::loadTexture(SHOTGUN_ARM_TEXTURE));
        S_weapon_handle.setTexture(*ResourceManager::loadTexture(SHOTGUN_HANDLE_TEXTURE));
        S_weapon_trunk.setTexture(*ResourceManager::loadTexture(SHOTGUN_TRUNK_TEXTURE));
        S_aim.setTexture(*ResourceManager::loadTexture(AIM_TEXTURE));
        S_fire.setTexture(*ResourceManager::loadTexture(FIRE_SHOTGUN_TEXTURE));
        d_speed = 0.4;
        S_aim.scale(.05f, .05f);

        fireSoundBuffer.loadFromFile(GUN_SHOT_SOUND);
        fireSound.setBuffer(fireSoundBuffer);
        fireSound.setVolume(10.f);
        i_damage = 50;
    }
}

bool Weapon::fire()
{
    if ((i_amount > 0) && (d_lastFireTime == 0))
    {
        d_lastFireTime = d_speed;
        d_fireAnimTime = .1;
        --i_amount;
        fireShift = d_amplitude;
        fireSound.play();
        return true;
    }
    return false;
}

void Weapon::draw(sf::RenderTarget& window)
{
    auto tp = std::chrono::system_clock::now();
    std::chrono::duration <double> elapsedTime = tp.time_since_epoch();
    double diff = elapsedTime.count() - d_elapsedTime;
    d_elapsedTime = elapsedTime.count();


    if (fireShift > 0)
        fireShift -= 2 * d_amplitude * diff / d_speed;
    else
        fireShift = 0;
    double shift = 15 * (1 + cos(3 * d_elapsedTime)); // Motion of the weapon

    Point2D weaponPosition = { SCREEN_WIDTH - S_weapon_handle.getTextureRect().width + shift + fireShift, SCREEN_HEIGHT - S_weapon_handle.getTextureRect().height + shift + fireShift };

    // FIRE EFFECT HERE
    d_lastFireTime -= diff; // Time when you can't fire
    if (d_lastFireTime < 0)
        d_lastFireTime = 0;

    d_fireAnimTime -= diff;
    if (d_fireAnimTime > 0)
    {
        S_fire.setPosition(sf::Vector2f((float)(weaponPosition.x - 490), (float)(weaponPosition.y - 792))); // абсолютная позиция
        window.draw(S_fire);
    }

    // All about weapon

    double d_armShift = 50 * sin(PI * 2 * d_lastFireTime / d_speed);
    if (d_lastFireTime <= 0 || d_lastFireTime > d_speed / 2)
        d_armShift = 0;

    S_weapon_arm.setPosition(sf::Vector2f((float)(weaponPosition.x + d_armShift), (float)(weaponPosition.y + d_armShift))); // Shift arm when we shoot
    S_weapon_handle.setPosition(sf::Vector2f((float)weaponPosition.x, (float)weaponPosition.y));
    S_weapon_trunk.setPosition(sf::Vector2f((float)weaponPosition.x, (float)weaponPosition.y));

    window.draw(S_weapon_trunk);
    window.draw(S_weapon_arm);
    window.draw(S_weapon_handle);

    S_aim.setPosition(sf::Vector2f((float)(SCREEN_WIDTH / 2 - S_aim.getTextureRect().width / 50), (float)(SCREEN_HEIGHT / 2 - S_aim.getTextureRect().height / 50))); // абсолютная позиция
    window.draw(S_aim);
}
