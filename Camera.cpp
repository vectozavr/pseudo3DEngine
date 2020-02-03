//
// Created by ivan- on 18.01.2020.
//

#include <cmath>
#include <windows.h>
#include "Camera.h"


using namespace std;

void Camera::updateDistances(World& world) {
    v_distances.clear();
    std::string obj;
    for(int i = 0; i < 2*PI/d_fieldOfView*DISTANCES_SEGMENTS; i++) {
        double left = d_direction - d_fieldOfView/2;
        double right = d_direction + d_fieldOfView/2;
        // find distance to near object in that direction
        double direction = d_direction + ((double)i/DISTANCES_SEGMENTS - 0.5)*d_fieldOfView;
        double progress = 0;

        pair<Point2D, Point2D> segment1 = {{x(), y()}, {x() + d_depth*cos(direction), y() + d_depth*sin(direction)}};

        Point2D crossPoint = {0, 0};
        Point2D nearCross = {x() + d_depth*cos(direction), y() + d_depth*sin(direction)};
        double len = 0;
        for(auto object : world.objects()) {
            if(object.first == getName() || object.second.nodes().size() < 2)
                continue;
            progress = 0;
            pair<Point2D, Point2D> segment2 = { object.second.position() + object.second.nodes().front(), object.second.position() + object.second.nodes().back() };
            for(int k = 0; k < object.second.nodes().size(); k++) {
                if(segments_crossing(segment1, segment2, crossPoint)) {
                    if ((nearCross - position()).abs() > (crossPoint - position()).abs()) {
                        nearCross = crossPoint;
                        obj = object.second.getName();
                        len = 0*progress + (segment2.second - nearCross).abs();
                        // For collision detection
                        if(COLLISION_DISTANCE >= (nearCross - position()).abs()) {
                            CollisionInformation newCollision;
                            newCollision.distance = (nearCross - position()).abs();
                            newCollision.edge = segment2;
                            newCollision.collisionPoint = nearCross;
                            allCollisions.push_back(newCollision);
                        }
                    }
                } else {
                    progress += (segment2.second - segment2.first).abs();
                }
                if(k+1 < object.second.nodes().size())
                    segment2 = { object.second.position() + object.second.nodes()[k], object.second.position() + object.second.nodes()[k+1] };
            }
        }
        v_distances.push_back({(position()-nearCross).abs(), len, obj});
        //if(segments_crossing())
        /*
        for(int y = 0; y < SCREEN_HEIGHT; y++) {

        }
        */
    }
    checkCollisions();
}

void Camera::draw(sf::RenderWindow& window) {

    if(v_distances.size() == 0)
        updateDistances(W_world);

    sf::CircleShape circle;
    circle.setRadius(RADIUS_CAMERA);
    circle.setOutlineColor(OUTLINE_CAMERA_COLOR);
    circle.setFillColor(FILL_CAMERA_COLOR);
    circle.setOutlineThickness(OUTLINE_CAMERA_THICKNESS);
    circle.setPosition((float)x()*SCALE - RADIUS_CAMERA, (float)y()*SCALE - RADIUS_CAMERA);

    double left = d_direction - d_fieldOfView/2;
    double right = d_direction + d_fieldOfView/2;

    sf::ConvexShape triangle;
    triangle.setPointCount(CONVEX_NUMBER + 2);
    triangle.setPoint(0, sf::Vector2f(0, 0));
    for(int i = 0; i <= CONVEX_NUMBER; i++) {
        triangle.setPoint(i+1, sf::Vector2f(v_distances[(int)i*DISTANCES_SEGMENTS/CONVEX_NUMBER].distance*cos(left + (right - left)*i/CONVEX_NUMBER)*SCALE, v_distances[(int)i*DISTANCES_SEGMENTS/CONVEX_NUMBER].distance*sin(left + (right - left)*i/CONVEX_NUMBER)*SCALE));
    }
    triangle.setOutlineColor(OUTLINE_CAMERA_COLOR);
    triangle.setFillColor(FILED_OF_VEW_COLOR);
    triangle.setOutlineThickness(OUTLINE_CAMERA_THICKNESS);
    triangle.setPosition((float)x()*SCALE, (float)y()*SCALE);

    window.draw(triangle);
    window.draw(circle);
}

bool Camera::keyboardControl(double elapsedTime, sf::RenderWindow& window) {
    double dx = 0;
    double dy = 0;

    // left and right
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        dx += sin(d_direction) * d_walkSpeed * elapsedTime;
        dy += -cos(d_direction) * d_walkSpeed * elapsedTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        dx += -sin(d_direction) * d_walkSpeed * elapsedTime;
        dy += cos(d_direction) * d_walkSpeed * elapsedTime;
    }
    // forward and backward
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        dx += cos(d_direction) * d_walkSpeed * elapsedTime;
        dy += sin(d_direction) * d_walkSpeed * elapsedTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        dx += -cos(d_direction) * d_walkSpeed * elapsedTime;
        dy += -sin(d_direction) * d_walkSpeed * elapsedTime;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        return false;
    }
    // Mouse
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        // левая кнопка мыши нажата, стреляем!
    }
    if(sf::Mouse::getPosition(window).x != localMousePosition.x) {
        double difference = sf::Mouse::getPosition(window).x - localMousePosition.x;
        localMousePosition = sf::Mouse::getPosition(window);
        d_direction += d_viewSpeed * difference;
    }
    shiftPrecise({dx, dy});
    return true;
    //shift({dx, dy});
}

pair<double, double> height(double distance) {
    pair<double, double> h = {0, 0};
    h.first = (1 - 1/distance)*SCREEN_HEIGHT/2;
    h.second = (1 + 1/distance)*SCREEN_HEIGHT/2;
    return h;
}

void Camera::drawCameraView(sf::RenderWindow& window) {

    if(v_distances.size() == 0)
        updateDistances(W_world);
    //SKY AND FLOOR
    if(b_textures) {
        sf::Sprite sprite_sky;
        sprite_sky.setTexture(W_world.skyTexture());
        sprite_sky.setTextureRect(sf::IntRect(d_direction * SCREEN_WIDTH/2, 1080-SCREEN_HEIGHT, SCREEN_WIDTH, 1080));
        sprite_sky.setPosition(sf::Vector2f(0,0)); // абсолютная позиция
        //sprite_sky.scale(1, (double)SCREEN_HEIGHT/2/1080);
        //sprite_sky.setColor({255, 255, 255, static_cast<sf::Uint8>(alpha)});
        window.draw(sprite_sky);

        sf::Sprite sprite_floor;
        sprite_floor.setTexture(W_world.floorTexture());
        sprite_floor.setTextureRect(sf::IntRect(0, 0, SCREEN_WIDTH, 1080));
        sprite_floor.setPosition(sf::Vector2f(0,SCREEN_HEIGHT/2)); // абсолютная позиция
        sprite_floor.scale(1, (double)1/2);
        //sprite_sky.setColor({255, 255, 255, static_cast<sf::Uint8>(alpha)});
        window.draw(sprite_floor);
    }


    for(int i = 0; i < DISTANCES_SEGMENTS-1; i++) {
        sf::ConvexShape polygon;
        polygon.setPointCount(4);

        pair<double, double> height_before = height(v_distances[i].distance);
        pair<double, double> height_now = height(v_distances[i+1].distance);

        int previous_h1 = height_before.first;
        int previous_h2 = height_before.second;
        int h1 = height_now.first;
        int h2 = height_now.second;

        polygon.setPoint(0, sf::Vector2f(0, h1));
        polygon.setPoint(1, sf::Vector2f(0, h2));
        polygon.setPoint(2, sf::Vector2f(SCREEN_WIDTH/DISTANCES_SEGMENTS, h2));
        polygon.setPoint(3, sf::Vector2f(SCREEN_WIDTH/DISTANCES_SEGMENTS, h1));

        int alpha = 255*(1 - v_distances[i].distance/d_depth);
        if(alpha > 255)
            alpha = 255;
        if(alpha < 0)
            alpha = 0;

        if(!b_textures)
            polygon.setFillColor({255, 174, 174, static_cast<sf::Uint8>(alpha)});
        else
            polygon.setFillColor({255, 174, 174, 255});

        polygon.setOutlineThickness(0);
        polygon.setPosition(SCREEN_WIDTH*i/DISTANCES_SEGMENTS, 0);
        if(abs(v_distances[i].distance - d_depth) > 0.001)
            window.draw(polygon);

        double scaleFactor = (double)(h2-h1)/SCREEN_HEIGHT;
        sf::Sprite sprite;
        if(v_distances[i].object != "" && b_textures) {
            sprite.setTexture(W_world[v_distances[i].object].loadTexture());
            sprite.setTextureRect(sf::IntRect(v_distances[i].progress * SCREEN_WIDTH, 0, SCREEN_WIDTH / DISTANCES_SEGMENTS, SCREEN_HEIGHT));
            sprite.setPosition(sf::Vector2f(SCREEN_WIDTH * (i) / DISTANCES_SEGMENTS,SCREEN_HEIGHT / 2 - (h2 - h1) / 2)); // абсолютная позиция
            sprite.scale(1, scaleFactor);
            sprite.setColor({255, 255, 255, static_cast<sf::Uint8>(alpha)});
            window.draw(sprite);
        }
    }

    weapons[selectedWeapon].draw(window);
}

void Camera::checkCollisions() {
    int howMuchCollisions = allCollisions.size();
    if(b_collision) {
        for(int i = howMuchCollisions-1; i >=0; i--) {
            Point2D vector = allCollisions[i].edge.first - allCollisions[i].edge.second;
            Point2D normalVector = {-vector.y, vector.x};
            normalVector = normalVector.normalize();
            Point2D shiftVector = normalVector*COLLISION_DISTANCE;
            Point2D resultPosition = allCollisions[i].collisionPoint + shiftVector;
            //shift(shiftVector);
            setPosition(resultPosition);
            allCollisions.pop_back();
        }
    }
}

void Camera::shiftPrecise(Point2D vector) {
    shift(vector);

    //p_position += vector;
}
