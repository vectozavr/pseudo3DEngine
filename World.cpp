//
// Created by ivan- on 18.01.2020.
//

// Visual studio sscanf usage
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <fstream>
#include <sstream>
#include "World.h"
#include "settings.h"
#include "Point3D.h"
#include <vector>
#include "Poligon2D.h"

using namespace std;

World::World(double length, double width, const std::string& sky_texture, const std::string& floor_texture)
    : d_length(length), d_width(width), T_floor_texture(*ResourceManager::loadTexture(floor_texture)), T_sky_texture(*ResourceManager::loadTexture(sky_texture)),
      T_health_texture(*ResourceManager::loadTexture(HEALTH_TEXTURE)), T_shoot_texture(*ResourceManager::loadTexture(SHOUT_TEXTURE)),
      F_font(*ResourceManager::loadFont(TEXT_FONT))
{
    S_floor.setTexture(floorTexture());
    S_health_sprite.setTexture(T_health_texture);
    S_shoot_sprite.setTexture(T_shoot_texture);

    init_bonuses();
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
        object.second.get()->draw(window);
}

sf::Sprite& World::floor()
{
    return S_floor;
}

sf::Font& World::font() {
    return F_font;
}

sf::Sprite& World::health(){
    return S_health_sprite;
}
sf::Sprite& World::shoot(){
    return S_shoot_sprite;
}

const sf::Texture& World::skyTexture() const
{
    return T_sky_texture;
}

const sf::Texture& World::floorTexture() const
{
    return T_floor_texture;
}

const std::map<std::string, std::shared_ptr<Object2D>>& World::objects() const
{
    return map_objects;
}

std::map<std::string, std::shared_ptr<Object2D>>& World::objects()
{
    return map_objects;
}

bool World::addObject2D(std::shared_ptr<Object2D> object, std::string name)
{
    object.get()->setName(name);
    return map_objects.insert({name, object}).second;
}

bool World::removeObject2D(const std::string& name)
{
    if(map_objects[name] == nullptr) return false;
    return map_objects.erase(name) > 0;
}

bool World::isExist(const std::string& name) const
{
    return map_objects.count(name) != 0;
}

std::shared_ptr<Object2D> World::findObject2D(const std::string& name)
{
    return map_objects[name];
}

const std::shared_ptr<Object2D> World::findObject2D(const std::string& name) const
{
    return map_objects[name];
}

std::shared_ptr<Object2D> World::operator[](const std::string& name)
{
    return findObject2D(name);
}

const std::shared_ptr<Object2D> World::operator[](const std::string& name) const
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


struct Point4D {
    double x = 0;
    double y = 0;
    double z = 0;
    double w = 0;
};

bool World::load3DObj(const std::string filename, const std::string texture, double scale, Point2D position) {

    std::vector<Point3D> vertices;
    std::vector<Point4D> indices;

    std::ifstream file(filename);

    int numb = 0;

    while (file) {
        std::string str;
        std::getline(file, str);
        if (str == "" || str == "\r" || str == "\n")
            continue;

        std::string type = str.substr(0, 2);
        str = str.substr(2, str.size());

        if (type == "v ") {
            std::stringstream str_stream(str);
            float x; float y; float z;
            str_stream >> x >> y >> z;

            vertices.push_back({x, y, z});
        }
        if (type == "vn") {
            // if you need - lets get it
        }
        if (type == "vt") {
            // if you need - lets get it
        }
        /*
        # f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 v4/vt4/vn4
        # v1 - индекс из списка вершин
        # vt1 - индекс из списка текс.координат
        # vn1 - индекс из списка нормалей
        */

        if (type == "f ") {
            int i1 = 0; int i2 = 0; int i3 = 0; int i4 = 0;
            int ti = 0;
            int vi = 0;

            const char* buffer = str.c_str();

            sscanf(buffer, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &i1, &ti, &vi, &i2, &ti, &vi, &i3, &ti, &vi, &i4, &ti, &vi);

            indices.push_back({static_cast<double>(i1-1), static_cast<double>(i2-1), static_cast<double>(i3-1), static_cast<double>(i4-1)});
        }
    }

    for (size_t i = 0; i < indices.size(); i++) {

        bool collision = false;

        Poligon2D* wall = new Poligon2D({{vertices[indices[i].z].x * scale, vertices[indices[i].z].z * scale}, {vertices[indices[i].x].x * scale, vertices[indices[i].x].z * scale}}, position, vertices[indices[i].y].y * scale, texture);

        for(auto obj : map_objects)
            for (auto node1 : obj.second.get()->nodes())
                if (node1 == wall->nodes()[0])
                    for (auto node2 : obj.second.get()->nodes())
                        if (node2 == wall->nodes()[1])
                            collision = true;

        if(!collision)
            if(!this->addObject2D(shared_ptr<Object2D>(wall), "wall" + std::to_string(i + 1) + "-" + filename + "-" + texture))
                return false;
    }



    return !indices.empty();
}

bool World::init_bonuses() {

    vector<Bonus> bonuses;
    bonuses.push_back(Bonus({0, 0}, BonusType::TreatmentBonus, HEALTH_BONUS_TEXTURE));
    bonuses.push_back(Bonus({0, 1}, BonusType::AmmunitionBonus, AMMUNATION_BONUS_TEXTURE));
    bonuses.push_back(Bonus({0, 2}, BonusType::SpeedBonus, SPEED_BONUS_TEXTURE));

    bonuses.push_back(Bonus({0, 0}, BonusType::TreatmentBonus, HEALTH_BONUS_TEXTURE));
    bonuses.push_back(Bonus({0, 1}, BonusType::AmmunitionBonus, AMMUNATION_BONUS_TEXTURE));
    bonuses.push_back(Bonus({0, 2}, BonusType::SpeedBonus, SPEED_BONUS_TEXTURE));

    bonuses.push_back(Bonus({0, 0}, BonusType::TreatmentBonus, HEALTH_BONUS_TEXTURE));
    bonuses.push_back(Bonus({0, 1}, BonusType::AmmunitionBonus, AMMUNATION_BONUS_TEXTURE));
    bonuses.push_back(Bonus({0, 2}, BonusType::SpeedBonus, SPEED_BONUS_TEXTURE));

    bonuses.push_back(Bonus({0, 0}, BonusType::TreatmentBonus, HEALTH_BONUS_TEXTURE));
    bonuses.push_back(Bonus({0, 1}, BonusType::AmmunitionBonus, AMMUNATION_BONUS_TEXTURE));
    bonuses.push_back(Bonus({0, 2}, BonusType::SpeedBonus, SPEED_BONUS_TEXTURE));

    bonuses.push_back(Bonus({0, 0}, BonusType::TreatmentBonus, HEALTH_BONUS_TEXTURE));
    bonuses.push_back(Bonus({0, 1}, BonusType::AmmunitionBonus, AMMUNATION_BONUS_TEXTURE));
    bonuses.push_back(Bonus({0, 2}, BonusType::SpeedBonus, SPEED_BONUS_TEXTURE));

    bonuses.push_back(Bonus({0, 0}, BonusType::TreatmentBonus, HEALTH_BONUS_TEXTURE));
    bonuses.push_back(Bonus({0, 1}, BonusType::AmmunitionBonus, AMMUNATION_BONUS_TEXTURE));
    bonuses.push_back(Bonus({0, 2}, BonusType::SpeedBonus, SPEED_BONUS_TEXTURE));
    //bonuses.push_back(Bonus({0, 3}, BonusType::ViewBonus, VIEW_BONUS_TEXTURE));

    for(int b = 0; b < bonuses.size(); b++)
        if(addObject2D(std::make_shared<Bonus>(bonuses[b]), "bonus_" + std::to_string(b)))
            v_bonuses.push_back("bonus_" + std::to_string(b));
        else
            return false;

    return true;
}

bool World::addBonusPoint(Point2D p) {
    for(auto v : v_bonus_positions)
        if(v.first == p)
            return false;

    return v_bonus_positions.emplace_back(std::pair<Point2D, bool>(p, false)).second;
}

Point2D World::getBonusPoint() {
    return v_bonus_positions[rand() % v_bonus_positions.size()].first;
}

void World::freeBonusPoint(Point2D p) {
    for(auto& v : v_bonus_positions)
        if(v.first == p)
            v.second = false;
}

void World::rotateAllBonuses(double angle) {
    for(auto& bonus : v_bonuses)
        map_objects[bonus]->rotate(angle);
}
