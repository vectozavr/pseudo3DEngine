//
// Created by ivan- on 18.01.2020.
//

#include "Object2D.h"
#include "settings.h"

// Constructors

Object2D::Object2D(Point2D position, std::vector<Point2D> points, double height, std::string texture, Point2D velocity)
    : p_position(position), v_points2D(points), p_velocity(velocity), d_height(height), T_texture(*ResourceManager::loadTexture(texture))
{
}

Object2D::Object2D(const Object2D& object2D) : T_texture(object2D.T_texture)
{
    v_points2D = object2D.v_points2D;
    p_position = object2D.p_position;
    p_velocity = object2D.p_velocity;
    s_name = object2D.s_name;
    d_height = object2D.d_height;
    b_isMirror = object2D.b_isMirror;
}

// Miscellaneous

ObjectType Object2D::type()
{
    return ObjectType::Object;
}

sf::Texture& Object2D::loadTexture()
{
    return T_texture;
}

void Object2D::draw(sf::RenderTarget& window)
{
    sf::ConvexShape polygon;
    polygon.setPointCount(nodes().size());
    int i = 0;
    for (auto p : nodes())
        polygon.setPoint(i++, sf::Vector2f((float)p.x * SCALE, (float)p.y * SCALE));
    polygon.setOutlineColor(OUTLINE_COLOR);
    polygon.setFillColor(FILL_COLOR);
    polygon.setOutlineThickness(OUTLINE_THICKNESS);
    polygon.setPosition((float)x() * SCALE, (float)y() * SCALE);
    window.draw(polygon);
}

// Physics

int sign(double number)
{
    return number >= 0 ? 1 : -1;
}

bool Object2D::segments_crossing(const std::pair<Point2D, Point2D>& segment1, const std::pair<Point2D, Point2D>& segment2, Point2D& point)
{
    Point2D cut1 = segment1.second - segment1.first;
    Point2D cut2 = segment2.second - segment2.first;
    double prod1;
    double prod2;

    prod1 = Point2D::cross(cut1, (segment2.first - segment1.first));
    prod2 = Point2D::cross(cut1, (segment2.second - segment1.first));

    if (sign(prod1) == sign(prod2) || (prod1 == 0) || (prod2 == 0)) // Отсекаем также и пограничные случаи
        return false;

    prod1 = Point2D::cross(cut2, (segment1.first - segment2.first));
    prod2 = Point2D::cross(cut2, (segment1.second - segment2.first));

    if (sign(prod1) == sign(prod2) || (prod1 == 0) || (prod2 == 0)) // Отсекаем также и пограничные случаи
        return false;

    point.x = segment1.first.x + cut1.x * std::abs(prod1) / std::abs(prod2 - prod1);
    point.y = segment1.first.y + cut1.y * std::abs(prod1) / std::abs(prod2 - prod1);

    return true;
}

bool Object2D::cross(const std::pair<Point2D, Point2D>& ray, std::pair<Point2D, Point2D>& wall, Point2D& point, double& uv)
{
    Point2D crossPoint = { 0, 0 };
    std::pair<Point2D, Point2D> segment2 = { p_position + v_points2D.back(), p_position + v_points2D.front() };
    bool success = false;
    for (size_t k = 0; k < v_points2D.size(); k++)
    {
        if (segments_crossing(ray, segment2, crossPoint) && (point - ray.first).abs() > (crossPoint - ray.first).abs())
        {
            success = true;
            point = crossPoint;
            wall = std::move(segment2);
        }
        if(k + 1 != v_points2D.size())
            segment2 = { p_position + v_points2D[k], p_position + v_points2D[k + 1] };

        if(v_points2D.size() == 2)
            break;
    }

    // ------------
    /*
    if (segments_crossing(ray, segment2, crossPoint) && (point - ray.first).abs() > (crossPoint - ray.first).abs())
    {
        success = true;
        point = crossPoint;
        wall = std::move(segment2);
    }
    */
    // ------------

    if (success)
    {
        uv = (wall.second - point).abs();
    }
    return success;
}

// World coordinates

double Object2D::height() const
{
    return d_height;
}

double Object2D::x() const
{
    return p_position.x;
}

double Object2D::y() const
{
    return p_position.y;
}

Point2D Object2D::position() const
{
    return p_position;
}

void Object2D::shift(Point2D vector)
{
    p_position += vector;
}

void Object2D::setPosition(Point2D position)
{
    p_position = position;
}

// Mirror

bool Object2D::isMirror() const
{
    return b_isMirror;
}

void Object2D::setMirror(bool mirror)
{
    b_isMirror = mirror;
}

// Name

std::string Object2D::getName() const
{
    return s_name;
}

void Object2D::setName(std::string name)
{
    s_name = name;
}

// Segment nodes

std::vector<Point2D>& Object2D::nodes()
{
    return v_points2D;
}

void Object2D::setPoints2D(std::vector<Point2D> points2D)
{
    v_points2D = std::move(points2D);
}

void Object2D::rotate(double angle) {
    double cosA = cos(angle);
    double sinA = sin(angle);

    for(auto& p : v_points2D) {
        double oldX = p.x;
        double oldY = p.y;

        p.x = cosA * oldX - sinA * oldY;
        p.y = sinA * oldX + cosA * oldY;
    }
}