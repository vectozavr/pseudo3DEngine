//
// Created by Neirokan on 09.05.2020
//

#ifndef PSEUDO3DENGINE_POINT2D_H
#define PSEUDO3DENGINE_POINT2D_H

struct Point2D
{
    double x = 0;
    double y = 0;

    Point2D operator+(const Point2D& point2D) const;
    Point2D operator-(const Point2D& point2D) const;
    Point2D operator*(double number) const;
    Point2D operator/(double number) const;

    Point2D& operator=(const Point2D& point2D);

    Point2D& operator+=(const Point2D& point2D);
    Point2D& operator-=(const Point2D& point2D);
    Point2D& operator*=(double number);
    Point2D& operator/=(double number);

    bool operator==(const Point2D& point2D);
    bool operator!=(const Point2D& point2D);
    bool operator<(const Point2D& point2D) const;

    // Returns dot product
    double operator*(const Point2D& point2D) const;
    // Returns cross product
    double cross(const Point2D& point2D) const;
    // Returns dot product, but static
    static double dot(const Point2D& a, const Point2D& b);
    // Returns cross product, but static
    static double cross(const Point2D& a, const Point2D& b);
    // Returns normalized vector
    Point2D normalize();
    // Returns squared vector length
    double sqrAbs();
    // Returns vector length
    double abs();
};

#endif //PSEUDO3DENGINE_POINT2D_H
