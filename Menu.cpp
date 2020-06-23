//
// Created by ivan- on 22.01.2020.
//

#include "Menu.h"

Menu::Menu()
{
    buttons.assign(7, {});

    buttons[0].name = "PLAYGAME";
    buttons[1].name = "SETTINGS";
    buttons[2].name = "ABOUT";
    buttons[3].name = "QUIT";
    buttons[0].T_Texture = ResourceManager::loadTexture(PLAYGAME_TEXTURE);
    buttons[1].T_Texture = ResourceManager::loadTexture(SETTINGS_TEXTURE);
    buttons[2].T_Texture = ResourceManager::loadTexture(ABOUT_TEXTURE);
    buttons[3].T_Texture = ResourceManager::loadTexture(QUIT_TEXTURE);

    buttons[0].T_PressedTexture = ResourceManager::loadTexture(PLAYGAME_PRESSED_TEXTURE);
    buttons[1].T_PressedTexture = ResourceManager::loadTexture(SETTINGS_PRESSED_TEXTURE);
    buttons[2].T_PressedTexture = ResourceManager::loadTexture(ABOUT_PRESSED_TEXTURE);
    buttons[3].T_PressedTexture = ResourceManager::loadTexture(QUIT_PRESSED_TEXTURE);

    buttons[4].name = "TEXTURING";
    buttons[5].name = "SMOOTHING";
    buttons[6].name = "COLLISION";
    buttons[4].T_Texture = ResourceManager::loadTexture(TEXTURING_SELECT);
    buttons[5].T_Texture = ResourceManager::loadTexture(SMOOTHING_SELECT);
    buttons[6].T_Texture = ResourceManager::loadTexture(COLLISION_SELECT);

    buttons[4].T_PressedTexture = ResourceManager::loadTexture(TEXTURING_SELECT_S);
    buttons[5].T_PressedTexture = ResourceManager::loadTexture(SMOOTHING_SELECT_S);
    buttons[6].T_PressedTexture = ResourceManager::loadTexture(COLLISION_SELECT_S);

    for (size_t i = 0; i < buttons.size(); i++)
    {
        buttons[i].button.setTexture(*buttons[i].T_Texture);
        if (i < 4)
            buttons[i].button.setPosition((float)SCREEN_WIDTH / 2 - 170, (float)50 + 150 * i);
        else
            buttons[i].button.setPosition((float)SCREEN_WIDTH / 2 - 170, (float)50 + 150 * (i - 4));
    }

    if (b_textures)
        buttons[4].press();
    if (b_smooth)
        buttons[5].press();
    if (b_collision)
        buttons[6].press();
}

void Menu::drawMenu(sf::RenderWindow& window, double elapsedTime)
{
    sf::ConvexShape back;
    back.setPointCount(4);
    back.setPoint(0, sf::Vector2f(0, 0));
    back.setPoint(1, sf::Vector2f(SCREEN_WIDTH, 0));
    back.setPoint(2, sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
    back.setPoint(3, sf::Vector2f(0, SCREEN_HEIGHT));
    back.setPosition({ 0, 0 });
    back.setFillColor({ 255, 255, 255 });
    window.draw(back);

    float mouseX = (float)sf::Mouse::getPosition(window).x * window.getView().getSize().x / window.getSize().x;
    float mouseY = (float)sf::Mouse::getPosition(window).y * window.getView().getSize().y / window.getSize().y;

    bool b_pressing = sf::Mouse::isButtonPressed(sf::Mouse::Left) && !b_pressed;
    b_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    for (size_t i = 0; i < buttons.size(); i++)
    {
        if (!buttons[i].button.getGlobalBounds().contains(mouseX, mouseY))
            buttons[i].unSelect();
        else
        {
            buttons[i].select();
            if (window.hasFocus() && b_pressing)
            {
                buttons[i].unSelect();
                if (!b_settings)
                {
                    if (buttons[i].name == "PLAYGAME")
                    {
                        b_pause = false;
                        b_pressing = false;
                    }
                    else if (buttons[i].name == "SETTINGS")
                    {
                        b_settings = true;
                        b_pressing = false;
                    }
                    else if (buttons[i].name == "ABOUT")
                    {
                        b_about = true;
                        b_pressing = false;
                    }
                    else if (buttons[i].name == "QUIT")
                    {
                        window.close();
                        b_pressing = false;
                    }
                }
                else
                {
                    if (buttons[i].name == "TEXTURING")
                    {
                        buttons[i].press();
                        b_textures = buttons[i].pressed;
                        b_pressing = false;
                    }
                    else if (buttons[i].name == "SMOOTHING")
                    {
                        buttons[i].press();
                        b_smooth = buttons[i].pressed;
                        b_pressing = false;
                    }
                    else if (buttons[i].name == "COLLISION")
                    {
                        buttons[i].press();
                        b_collision = buttons[i].pressed;
                        b_pressing = false;
                    }
                }
            }
        }

        if (!b_settings && !b_about && i < 4)
            window.draw(buttons[i].button);
    }

    settings(window);
    about(window);
}

void Menu::about(sf::RenderTarget& window)
{
    if (!b_about)
        return;

    sf::Sprite button;
    button.setTexture(*ResourceManager::loadTexture(ABOUT_INFO));

    button.scale((float)SCREEN_WIDTH / 1920, (float)SCREEN_WIDTH / 1920);

    window.draw(button);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        b_about = false;
}

void Menu::settings(sf::RenderTarget& window)
{
    if (!b_settings)
        return;

    for (size_t i = 4; i < buttons.size(); i++)
        window.draw(buttons[i].button);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        b_settings = false;
}

bool Menu::isPaused()
{
    return b_pause;
}

void Menu::setPause(bool active)
{
    b_pause = active;
}

bool Menu::isSmooth()
{
    return b_smooth;
}

void Menu::setSmooth(bool active)
{
    b_smooth = active;
}

bool Menu::isCollision()
{
    return b_collision;
}

void Menu::setCollision(bool active)
{
    b_collision = active;
}

bool Menu::isTextures()
{
    return b_textures;
}

void Menu::setTextures(bool active)
{
    b_textures = active;
}
