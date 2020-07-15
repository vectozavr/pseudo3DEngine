//
// Created by Иван Ильин on 12.07.2020.
//

#ifndef FPSONLINE_POINT3D_H
#define FPSONLINE_POINT3D_H

struct Point3D
{
    double x = 0;
    double y = 0;
    double z = 0;

    Point3D operator+(const Point3D& point3D) const;
    Point3D operator-(const Point3D& point3D) const;
    Point3D operator*(double number) const;
    Point3D operator/(double number) const;

    Point3D& operator=(const Point3D& point3D);

    Point3D& operator+=(const Point3D& point3D);
    Point3D& operator-=(const Point3D& point3D);
    Point3D& operator*=(double number);
    Point3D& operator/=(double number);

    bool operator==(const Point3D& point3D);
    bool operator!=(const Point3D& point3D);

    // Returns dot product
    double operator*(const Point3D& poin3D) const;
    // Returns dot product, but static
    static double dot(const Point3D& a, const Point3D& b);
    // Returns normalized vector
    Point3D normalize();
    // Returns squared vector length
    double sqrAbs();
    // Returns vector length
    double abs();
};

#endif //FPSONLINE_POINT3D_H
