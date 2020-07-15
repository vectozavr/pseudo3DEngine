//
// Created by ivan- on 18.01.2020.
//

#ifndef PSEUDO3DENGINE_OBJECT2D_H
#define PSEUDO3DENGINE_OBJECT2D_H

#include <utility>
#include <vector>
#include <cmath>
#include "ResourceManager.h"
#include "Idrawable.h"
#include "ObjectType.h"
#include "Point2D.h"
#include "settings.h"

class Object2D : virtual public Idrawable
{
protected:
    std::vector<Point2D> v_points2D;

    Point2D p_position;
    Point2D p_velocity;

    std::string s_name;
    double d_height;
    bool b_isMirror = false;

    sf::Texture& T_texture;

public:
    Object2D(Point2D position = { 0, 0 }, std::vector<Point2D> points = {}, double height = 1, std::string texture = WALL_TEXTURE, Point2D velocity = { 0, 0 });
    Object2D(const Object2D& object2D);

    // Check the intersection of the ray with this object.
    // Returns true if ray intersects with this object, false otherwise.
    // wall - hitted wall.
    // point - cross point.
    // uv - distance from the beginning of wall to the cross point.
    virtual bool cross(const std::pair<Point2D, Point2D>& ray, std::pair<Point2D, Point2D>& wall, Point2D& point, double& uv);
    // Check segments intersection.
    // Returns true if segments crossing, false otherwise.
    // point - cross point.
    static bool segments_crossing(const std::pair<Point2D, Point2D>& segment1, const std::pair<Point2D, Point2D>& segment2, Point2D& point);

    void draw(sf::RenderTarget& window) override;
    sf::Texture& loadTexture();
    virtual ObjectType type();

    double height() const;
    double x() const;
    double y() const;
    Point2D position() const;
    void shift(Point2D vector);
    void setPosition(Point2D position);

    void setHeight(double h) { d_height = h; };

    bool isMirror() const;
    void setMirror(bool mirror = true);

    std::string getName() const;
    void setName(std::string name);

    std::vector<Point2D>& nodes();
    void setPoints2D(std::vector<Point2D> points2D);
};


#endif //PSEUDO3DENGINE_OBJECT2D_H
