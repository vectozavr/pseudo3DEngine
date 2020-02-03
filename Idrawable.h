//
// Created by ivan- on 21.01.2020.
//

#ifndef PSEUDO3DENGINE_IDRAWABLE_H
#define PSEUDO3DENGINE_IDRAWABLE_H

#include <SFML/Graphics.hpp>

class Idrawable {
public:
    virtual void draw(sf::RenderWindow& window) = 0;
};


#endif //PSEUDO3DENGINE_IDRAWABLE_H
