//
// Created by Neirokan on 09.05.2020
//

#include "Point2D.h"
#include <cmath>

Point2D Point2D::operator+(const Point2D& point2D) const
{
    return Point2D{ this->x + point2D.x, this->y + point2D.y };
}

Point2D Point2D::operator-(const Point2D& point2D) const
{
    return Point2D{ this->x - point2D.x, this->y - point2D.y };
}

Point2D Point2D::operator*(double number) const
{
    return Point2D{ this->x * number, this->y * number };
}

Point2D Point2D::operator/(double number) const
{
    return Point2D{ this->x * number, this->y * number };
}

Point2D& Point2D::operator= (const Point2D& point2D)
{
    this->x = point2D.x;
    this->y = point2D.y;
    return *this;
}

Point2D& Point2D::operator+=(const Point2D& point2D)
{
    this->x += point2D.x;
    this->y += point2D.y;
    return *this;
}

Point2D& Point2D::operator-=(const Point2D& point2D)
{
    this->x -= point2D.x;
    this->y -= point2D.y;
    return *this;
}

Point2D& Point2D::operator*=(double number)
{
    this->x *= number;
    this->y *= number;
    return *this;
}

Point2D& Point2D::operator/=(double number)
{
    this->x /= number;
    this->y /= number;
    return *this;
}

bool Point2D::operator==(const Point2D& point2D)
{
    return this == &point2D || (*this - point2D).sqrAbs() < 0.0000000001;
}

bool Point2D::operator!=(const Point2D& point2D)
{
    return this != &point2D && (*this - point2D).sqrAbs() > 0.0000000001;
}

double Point2D::operator*(const Point2D& point2D) const
{
    return this->x * point2D.x + this->y * point2D.y;
}

double Point2D::cross(const Point2D& point2D) const
{
    return this->x * point2D.y - this->y * point2D.x;
}

double Point2D::dot(const Point2D& a, const Point2D& b)
{
    return a.x * b.x + a.y * b.y;
}

double Point2D::cross(const Point2D& a, const Point2D& b)
{
    return a.x * b.y - a.y * b.x;
}

Point2D Point2D::normalize()
{
    double length = this->abs();
    return Point2D{ this->x / length, this->y / length };
}

double Point2D::sqrAbs()
{
    return x * x + y * y;
}

double Point2D::abs()
{
    return sqrt(x * x + y * y);
}
