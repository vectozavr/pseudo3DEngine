//
// Created by ivan- on 22.01.2020.
//

#ifndef PSEUDO3DENGINE_MENU_H
#define PSEUDO3DENGINE_MENU_H

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <map>
#include <iostream>
#include <chrono>
#include "settings.h"
#include "Poligon2D.h"
#include "Circle2D.h"
#include "Object2D.h"
#include "Camera.h"

struct Button {
    sf::Texture T_Texture;
    sf::Texture T_PressedTexture;
    sf::Sprite button;
    std::string name;

    bool selected = false;
    bool pressed = false;

    void select() {
        if(!selected && !pressed) {
            button.setTexture(T_PressedTexture);
            selected = true;
            button.scale({1.01, 1.01});
        }
    }
    void unSelect() {
        if(selected && !pressed) {
            button.setTexture(T_Texture);
            selected = false;
            button.scale({1/1.01, 1/1.01});
        }
    }

    void press() {
        if(!pressed) {
            button.setTexture(T_PressedTexture);
            pressed = true;
        } else {
            button.setTexture(T_Texture);
            pressed = false;
        }
    }
};

class Menu {
private:
    std::vector<Button> buttons;

    bool b_pause = true;

    bool b_settings = false;
    bool b_about = false;

    bool b_textures = true;
    bool smooth = false;
    bool b_collision = true;
    double lastTime = 0;
    
    bool b_pressed = false;
    
public:
    Menu();

    void drawMenu(sf::RenderWindow& window, double elapsedTime);

    void settings(sf::RenderWindow& window);

    void about(sf::RenderWindow& window);

    bool isPaused() { return b_pause; }
    void setPause() { b_pause = true; }

    bool isSmooth() { return smooth; }
    void switchSmooth() { smooth = !smooth; }
    bool isCollision() { return b_collision; }
    void switchCollision() { b_collision = !b_collision; }
    bool isTextures() { return b_textures; }
    void switchTextures() { b_textures = !b_textures; }
};


#endif //PSEUDO3DENGINE_MENU_H
