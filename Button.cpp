//
// Created by Neirokan on 11.05.2020
//

#include "Button.h"

void Button::select()
{
    if (!selected && !pressed)
    {
        button.setTexture(*T_PressedTexture);
        selected = true;
        button.scale({ 1.01f, 1.01f });
    }
}

void Button::unSelect()
{
    if (selected && !pressed)
    {
        button.setTexture(*T_Texture);
        selected = false;
        button.scale({ 1 / 1.01f, 1 / 1.01f });
    }
}

void Button::press()
{
    if (!pressed)
    {
        button.setTexture(*T_PressedTexture);
        pressed = true;
    }
    else
    {
        button.setTexture(*T_Texture);
        pressed = false;
    }
}
