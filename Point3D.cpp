//
// Created by Иван Ильин on 12.07.2020.
//

#include "Point3D.h"
#include <cmath>

Point3D Point3D::operator+(const Point3D& point3D) const
{
    return Point3D{ this->x + point3D.x, this->y + point3D.y, this->z + point3D.z };
}

Point3D Point3D::operator-(const Point3D& point3D) const
{
    return Point3D{ this->x - point3D.x, this->y - point3D.y, this->z - point3D.z };
}

Point3D Point3D::operator*(double number) const
{
    return Point3D{ this->x * number, this->y * number, this->z*number };
}

Point3D Point3D::operator/(double number) const
{
    return Point3D{ this->x * number, this->y * number };
}

Point3D& Point3D::operator= (const Point3D& point3D)
{
    this->x = point3D.x;
    this->y = point3D.y;
    this->z = point3D.z;
    return *this;
}

Point3D& Point3D::operator+=(const Point3D& point3D)
{
    this->x += point3D.x;
    this->y += point3D.y;
    this->z += point3D.z;
    return *this;
}

Point3D& Point3D::operator-=(const Point3D& point3D)
{
    this->x -= point3D.x;
    this->y -= point3D.y;
    this->z -= point3D.z;
    return *this;
}

Point3D& Point3D::operator*=(double number)
{
    this->x *= number;
    this->y *= number;
    this->z *= number;
    return *this;
}

Point3D& Point3D::operator/=(double number)
{
    this->x /= number;
    this->y /= number;
    this->z /= number;
    return *this;
}

bool Point3D::operator==(const Point3D& point3D)
{
    return this == &point3D || (*this - point3D).sqrAbs() < 0.0000000001;
}

bool Point3D::operator!=(const Point3D& point3D)
{
    return this != &point3D && (*this - point3D).sqrAbs() > 0.0000000001;
}

double Point3D::operator*(const Point3D& point3D) const
{
    return this->x * point3D.x + this->y * point3D.y + this->z * point3D.z;
}

double Point3D::dot(const Point3D& a, const Point3D& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}


Point3D Point3D::normalize()
{
    double length = this->abs();
    return Point3D{ this->x / length, this->y / length,  this->z / length};
}

double Point3D::sqrAbs()
{
    return x * x + y * y + z * z;
}

double Point3D::abs()
{
    return sqrt(x * x + y * y + z * z);
}