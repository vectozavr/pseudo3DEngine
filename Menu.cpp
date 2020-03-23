//
// Created by ivan- on 22.01.2020.
//

#include "Menu.h"

Menu::Menu() {

    buttons.assign(7, {});

    buttons[0].name = "PLAYGAME";
    buttons[1].name = "SETTINGS";
    buttons[2].name = "ABOUT";
    buttons[3].name = "QUIT";
    buttons[0].T_Texture.loadFromFile(PLAYGAME_TEXTURE);
    buttons[1].T_Texture.loadFromFile(SETTINGS_TEXTURE);
    buttons[2].T_Texture.loadFromFile(ABOUT_TEXTURE);
    buttons[3].T_Texture.loadFromFile(QUIT_TEXTURE);

    buttons[0].T_PressedTexture.loadFromFile(PLAYGAME_PRESSED_TEXTURE);
    buttons[1].T_PressedTexture.loadFromFile(SETTINGS_PRESSED_TEXTURE);
    buttons[2].T_PressedTexture.loadFromFile(ABOUT_PRESSED_TEXTURE);
    buttons[3].T_PressedTexture.loadFromFile(QUIT_PRESSED_TEXTURE);

    buttons[4].name = "TEXTURING";
    buttons[5].name = "SMOOTHING";
    buttons[6].name = "COLLISION";
    buttons[4].T_Texture.loadFromFile(TEXTURING_SELECT);
    buttons[5].T_Texture.loadFromFile(SMOOTHING_SELECT);
    buttons[6].T_Texture.loadFromFile(COLLISION_SELECT);

    buttons[4].T_PressedTexture.loadFromFile(TEXTURING_SELECT_S);
    buttons[5].T_PressedTexture.loadFromFile(SMOOTHING_SELECT_S);
    buttons[6].T_PressedTexture.loadFromFile(COLLISION_SELECT_S);

    buttons[4].press();

    for(int i = 0; i < buttons.size(); i++) {
        buttons[i].button.setTexture(buttons[i].T_Texture);
        if(i < 4)
            buttons[i].button.setPosition(SCREEN_WIDTH/2 - 170, 50 + 150*i);
        else
            buttons[i].button.setPosition(SCREEN_WIDTH/2 - 170, 50 + 150*(i-4));
    }
}

void Menu::drawMenu(sf::RenderWindow &window, double elapsedTime) {

    sf::ConvexShape back;
    back.setPointCount(4);
    back.setPoint(0, sf::Vector2f(0, 0));
    back.setPoint(1, sf::Vector2f(SCREEN_WIDTH, 0));
    back.setPoint(2, sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
    back.setPoint(3, sf::Vector2f(0, SCREEN_HEIGHT));
    back.setPosition({0, 0});
    back.setFillColor({255, 255, 255});
    window.draw(back);

	if(!sf::Mouse::isButtonPressed(sf::Mouse::Left) && b_pressed)
		b_pressed = false;

    for(int i = 0; i < buttons.size(); i++) {

        if(buttons[i].button.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y)) {
            buttons[i].select();
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                buttons[i].unSelect();
                if(!b_settings) {
                    if (buttons[i].name == "PLAYGAME") {
                        b_pause = false;
                    }
                    if (buttons[i].name == "SETTINGS") {
                        b_settings = true;
                    }
                    if (buttons[i].name == "ABOUT") {
                        b_about = true;
                    }
                    if (buttons[i].name == "QUIT") {
                        window.close();
                    }
                } else {
                    if (buttons[i].name == "TEXTURING") {
                        if(!b_pressed) {
							buttons[i].press();
							b_textures = buttons[i].pressed;
							b_pressed = true;
						}
                    }
                    if (buttons[i].name == "SMOOTHING") {
						if(!b_pressed) {
							buttons[i].press();
							smooth = buttons[i].pressed;
							b_pressed = true;
						}
                    }
                    if (buttons[i].name == "COLLISION") {
                        if(!b_pressed) {
							buttons[i].press();
							b_collision = buttons[i].pressed;
							b_pressed = true;
						}
                    }
                }
            }
        } else {
            buttons[i].unSelect();
        }


        if(!b_settings && !b_about && i < 4)
            window.draw(buttons[i].button);
    }

    settings(window);
    about(window);
}

void Menu::about(sf::RenderWindow &window) {
    if(!b_about) return;

    sf::Texture T_Texture;
    T_Texture.loadFromFile(ABOUT_INFO);
    sf::Sprite button;
    button.setTexture(T_Texture);

    button.scale((double)SCREEN_WIDTH/1920, (double)SCREEN_WIDTH/1920);

    window.draw(button);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        b_about = false;
}

void Menu::settings(sf::RenderWindow &window) {
    if(!b_settings) return;

    for(int i = 4; i < buttons.size(); i++) {
        window.draw(buttons[i].button);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        b_settings = false;
}
