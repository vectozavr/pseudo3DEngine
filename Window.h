//
// Created by ivan- on 02.02.2020.
//

#ifndef PSEUDO3DENGINE_WINDOW_H
#define PSEUDO3DENGINE_WINDOW_H
#include <SFML/Graphics.hpp>
#include "settings.h"

struct Button {
    sf::Texture T_Texture;
    sf::Texture T_PressedTexture;
    sf::Sprite button;
    std::string name;

    double time; // When this button was pressed

    bool selected = false;
    bool pressed = false;

    void select() {
        if(!selected && !pressed) {
            button.setTexture(T_PressedTexture);
            selected = true;
            //button.scale({1.01, 1.01});
        }
    }
    void unSelect() {
        if(selected && !pressed) {
            button.setTexture(T_Texture);
            selected = false;
            //button.scale({1/1.01, 1/1.01});
        }
    }

    void press() {
        selected = false;
        pressed = true;
    }
};

class Window {
private:
std::vector<Button> buttons;

public:
    Window();

    void checkActions(double time, sf::RenderWindow& window) {

        for(auto button : buttons) {
            if(abs(time - button.time) > TIMEQUANTA){
                if(button.button.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y)) {
                    button.select();
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                    {
                        button.press();
                    }
                }
            }
        }
    }
};


#endif //PSEUDO3DENGINE_WINDOW_H
