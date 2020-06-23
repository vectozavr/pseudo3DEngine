//
// Created by Neirokan on 11.05.2020
//

#ifndef PSEUDO3DENGINE_BUTTON_H
#define PSEUDO3DENGINE_BUTTON_H

#include <SFML/Graphics.hpp>

struct Button
{
    sf::Texture* T_Texture;
    sf::Texture* T_PressedTexture;
    sf::Sprite button;
    std::string name;

    bool selected = false;
    bool pressed = false;

    void select();
    void unSelect();
    void press();
};


#endif //PSEUDO3DENGINE_BUTTON_H
