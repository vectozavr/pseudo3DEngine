//
// Created by ivan- on 18.01.2020.
//

#include "World.h"
#include "settings.h"

void World::draw(sf::RenderWindow &window) {

    sf::ConvexShape background;
    background.setPointCount(4);
    background.setPoint(0, sf::Vector2f(0, 0));
    background.setPoint(1, sf::Vector2f(SCALE_WINDOW*SCALE/100, 0));
    background.setPoint(2, sf::Vector2f(SCALE_WINDOW*SCALE/100, SCALE_WINDOW*SCALE/100));
    background.setPoint(3, sf::Vector2f(0, SCALE_WINDOW*SCALE/100));
    background.setOutlineColor(OUTLINE_BACK_COLOR);
    background.setFillColor(FILL_BACK_COLOR);
    background.setOutlineThickness(OUTLINE_BACK_THICKNESS);
    background.setPosition(0, 0);
    window.draw(background);

    for(auto object : map_objects) {
        object.second.draw(window);
    }
}
