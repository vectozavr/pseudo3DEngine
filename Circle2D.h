//
// Created by ivan- on 21.01.2020.
//

#ifndef PSEUDO3DENGINE_CIRCLE2D_H
#define PSEUDO3DENGINE_CIRCLE2D_H

#include "Object2D.h"
#include "settings.h"

class Circle2D : public Object2D{
private:
    double d_radius = 0;
public:
    explicit Circle2D(double radius = 0, Point2D position = {0, 0}, double height = 1, std::string texture = WALL_TEXTURE) : Object2D(position, {}, height, texture), d_radius(radius) {
        for(int i = 0; i < CIRCLE_CONVEX_NUMBER; i++) {
            double _x = d_radius * cos((double)i / CIRCLE_CONVEX_NUMBER * 2 * PI);
            double _y = d_radius * sin((double)i / CIRCLE_CONVEX_NUMBER * 2 * PI);

            nodes().push_back({_x, _y});
        }
    }
};


#endif //PSEUDO3DENGINE_CIRCLE2D_H
