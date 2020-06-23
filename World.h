//
// Created by ivan- on 18.01.2020.
//

#ifndef PSEUDO3DENGINE_WORLD_H
#define PSEUDO3DENGINE_WORLD_H

#include "Object2D.h"
#include <map>

class World : virtual public Idrawable
{
private:
    mutable std::map<std::string, Object2D&> map_objects;

    double d_length = 0;
    double d_width = 0;

    sf::Texture& T_sky_texture;
    sf::Texture& T_floor_texture;
    sf::Sprite S_floor;

public:
    World(double length, double width, const std::string& sky_texture = SKY_TEXTURE, const std::string& floor_texture = FLOOR_TEXTURE);

    void draw(sf::RenderTarget& window) override;

    sf::Sprite& floor();
    const sf::Texture& skyTexture() const;
    const sf::Texture& floorTexture() const;

    const std::map<std::string, Object2D&>& objects() const;

    bool addObject2D(Object2D& object, std::string name);
    bool removeObject2D(const std::string& name);

    bool isExist(const std::string& name) const;
    Object2D& findObject2D(const std::string& name);
    const Object2D& findObject2D(const std::string& name) const;

    Object2D& operator[](const std::string& name);
    const Object2D& operator[](const std::string& name) const;

    double width() const;
    double length() const;
};


#endif //PSEUDO3DENGINE_WORLD_H
