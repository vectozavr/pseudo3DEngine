//
// Created by ivan- on 18.01.2020.
//

#include "World.h"
#include "settings.h"

World::World(double length, double width, const std::string& sky_texture, const std::string& floor_texture)
    : d_length(length), d_width(width), T_floor_texture(*ResourceManager::loadTexture(floor_texture)), T_sky_texture(*ResourceManager::loadTexture(sky_texture))
{
    S_floor.setTexture(floorTexture());
}

void World::draw(sf::RenderTarget& window)
{
    sf::ConvexShape background;
    background.setPointCount(4);
    background.setPoint(0, sf::Vector2f(0, 0));
    background.setPoint(1, sf::Vector2f(SCALE_WINDOW * SCALE / 100, 0));
    background.setPoint(2, sf::Vector2f(SCALE_WINDOW * SCALE / 100, SCALE_WINDOW * SCALE / 100));
    background.setPoint(3, sf::Vector2f(0, SCALE_WINDOW * SCALE / 100));
    background.setOutlineColor(OUTLINE_BACK_COLOR);
    background.setFillColor(FILL_BACK_COLOR);
    background.setOutlineThickness(OUTLINE_BACK_THICKNESS);
    background.setPosition(0, 0);
    window.draw(background);

    for (auto object : map_objects)
        object.second.draw(window);
}

sf::Sprite& World::floor()
{
    return S_floor;
}

const sf::Texture& World::skyTexture() const
{
    return T_sky_texture;
}

const sf::Texture& World::floorTexture() const
{
    return T_floor_texture;
}

const std::map<std::string, Object2D&>& World::objects() const
{
    return map_objects;
}

bool World::addObject2D(Object2D& object, std::string name)
{
    object.setName(name);
    return map_objects.insert({ name, object }).second;
}

bool World::removeObject2D(const std::string& name)
{
    return map_objects.erase(name) > 0;
}

bool World::isExist(const std::string& name) const
{
    return map_objects.count(name) != 0;
}

Object2D& World::findObject2D(const std::string& name)
{
    return map_objects.at(name);
}

const Object2D& World::findObject2D(const std::string& name) const
{
    return map_objects.at(name);
}

Object2D& World::operator[](const std::string& name)
{
    return findObject2D(name);
}

const Object2D& World::operator[](const std::string& name) const
{
    return findObject2D(name);
}

double World::width() const
{
    return d_width;
}

double World::length() const
{
    return d_length;
}
