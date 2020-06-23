//
// Created by ivan- on 21.01.2020.
//

#include "Circle2D.h"
#include "settings.h"

Circle2D::Circle2D(double radius, Point2D position, double height, const std::string& texture, int convexNumber)
    : Object2D(position, {}, height, texture), d_radius(radius)
{
    for (int i = 0; i < convexNumber; i++)
    {
        double _x = d_radius * cos((double)i / convexNumber * 2 * PI + PI / 4);
        double _y = d_radius * sin((double)i / convexNumber * 2 * PI + PI / 4);

        nodes().push_back({ _x, _y });
    }
}

ObjectType Circle2D::type()
{
    return ObjectType::Circle;
}

bool Circle2D::cross(const std::pair<Point2D, Point2D>& ray, std::pair<Point2D, Point2D>& wall, Point2D& point, double& uv)
{
    bool success = false;
    Point2D d = ray.second - ray.first;
    Point2D f = ray.first - p_position;
    float a = static_cast<float>(d.x * d.x + d.y * d.y);
    float b = static_cast<float>(2 * (f.x * d.x + f.y * d.y));
    float c = static_cast<float>((f.x * f.x + f.y * f.y) - d_radius * d_radius);

    float discriminant = b * b - 4 * a * c;
    if (discriminant >= 0)
    {
        discriminant = sqrt(discriminant);
        float t1 = (-b - discriminant) / (2 * a);
        float t2 = (-b + discriminant) / (2 * a);

        if (t1 >= 0 && t1 <= 1)
        {
            point.x = ray.first.x + t1 * d.x;
            point.y = ray.first.y + t1 * d.y;
            success = true;
        }
        else if (t2 >= 0 && t2 <= 1)
        {
            point.x = ray.first.x + t2 * d.x;
            point.y = ray.first.y + t2 * d.y;
            success = true;
        }
    }
    if (success)
    {
        double cx = cos(PI / 4), cy = sin(PI / 4);
        Point2D diff = point - p_position;
        double angle = atan2((cx * diff.y - cy * diff.x) / d_radius, (cx * diff.x + cy * diff.y) / d_radius);
        wall.first = { p_position.x + diff.y, p_position.y - diff.x };
        wall.second = { p_position.x - diff.y, p_position.y + diff.x };
        uv = d_radius * angle;
    }
    return success;
}
