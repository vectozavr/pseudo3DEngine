//
// Created by ivan- on 18.01.2020.
//

#ifndef PSEUDO3DENGINE_WORLD_H
#define PSEUDO3DENGINE_WORLD_H

#include "Object2D.h"
#include <map>
#include <memory>
#include "Bonus.h"

class World : virtual public Idrawable
{
private:
    mutable std::map<std::string, std::shared_ptr<Object2D>> map_objects;

    std::vector<std::pair<Point2D, bool>> v_bonus_positions; // It is points in the world, where bonuses can be spawned.

    double d_length = 0;
    double d_width = 0;

    sf::Texture& T_sky_texture;
    sf::Texture& T_floor_texture;
    sf::Sprite S_floor;

    sf::Texture& T_health_texture;
    sf::Sprite S_health_sprite;
    sf::Texture& T_shoot_texture;
    sf::Sprite S_shoot_sprite;

    sf::Font F_font;

    std::vector<std::weak_ptr<Bonus>> v_bonuses;
    bool init_bonuses();

public:
    World(double length = 100, double width = 100, const std::string& sky_texture = SKY_TEXTURE, const std::string& floor_texture = FLOOR_TEXTURE);

    void draw(sf::RenderTarget& window) override;

    sf::Sprite& floor();
    sf::Sprite& health();
    sf::Sprite& shoot();

    sf::Font& font();

    const sf::Texture& skyTexture() const;
    const sf::Texture& floorTexture() const;

    const std::map<std::string, std::shared_ptr<Object2D>>& objects() const;
    std::map<std::string, std::shared_ptr<Object2D>>& objects();

    bool addObject2D(std::shared_ptr<Object2D> object, std::string name);
    bool removeObject2D(const std::string& name);

    bool addBonusPoint(Point2D p);
    Point2D getBonusPoint();
    void freeBonusPoint(Point2D p);

    bool isExist(const std::string& name) const;
    std::shared_ptr<Object2D> findObject2D(const std::string& name);
    const std::shared_ptr<Object2D> findObject2D(const std::string& name) const;

    std::shared_ptr<Object2D> operator[](const std::string& name);
    const std::shared_ptr<Object2D> operator[](const std::string& name) const;

    double width() const;
    double length() const;

    // NEW

    bool load3DObj(std::string filename, std::string texture = FLOOR_TEXTURE, double scale = 1.0, Point2D position = {0, 0});

    void rotateAllBonuses(double angle);
    const std::vector<std::weak_ptr<Bonus>>& bonuses() const {
        return v_bonuses;
    }
};


#endif //PSEUDO3DENGINE_WORLD_H