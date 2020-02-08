//
// Created by ivan- on 18.01.2020.
//

#include <cmath>
#include <windows.h>
#include "Camera.h"


using namespace std;

void Camera::objectsRayCrossed(pair<Point2D, Point2D> ray, std::vector<RayCastStructure> &v_rayCastStruct, World &world, std::string name) {
    std::string obj;
    double len = 0;
    double progress = 0;
    for(auto object : world.objects()) {
        Point2D crossPoint = {0, 0};
        obj = "";
        Point2D nearCross = ray.second;
        std::vector<RayCastStructure> v_reflectedRayCastStructure;
        if(object.first == name || object.second.nodes().size() < 2)
            continue;
        progress = 0;
        pair<Point2D, Point2D> segment2 = {object.second.position() + object.second.nodes().back(), object.second.position() + object.second.nodes().front() };
        for(int k = 0; k < object.second.nodes().size(); k++) {
            if(segments_crossing(ray, segment2, crossPoint)) {
                if ((nearCross - ray.first).abs() > (crossPoint - ray.first).abs()) {
                    nearCross = crossPoint;
                    obj = object.second.getName();
                    len = 0*progress + (segment2.second - nearCross).abs();
                    // For collision detection
                    if(name == getName() && COLLISION_AREA >= (nearCross - position()).abs()) {
                        CollisionInformation newCollision;
                        newCollision.distance = (nearCross - position()).abs();
                        newCollision.edge = segment2;
                        newCollision.collisionPoint = nearCross;
                        allCollisions.push_back(newCollision);
                    }
                    if(object.second.isMirror() && scalarWithNormal(segment2.second - segment2.first, ray.second - ray.first) < 0) {
                        Point2D edgeVector = segment2.second - segment2.first;
                        Point2D rayVector = ray.second - ray.first;
                        double twistAngle = 2*acos((edgeVector.x*rayVector.x + edgeVector.y*rayVector.y)/(edgeVector.abs()*rayVector.abs()));
                        Point2D twistedRayVector = {rayVector.x*cos(twistAngle) + rayVector.y*sin(twistAngle), -rayVector.x*sin(twistAngle) + rayVector.y*cos(twistAngle)};
                        pair<Point2D, Point2D> newRay = {crossPoint, crossPoint + twistedRayVector};

                        objectsRayCrossed(newRay, v_reflectedRayCastStructure, world, object.first);
                    }
                }
            } else {
                progress += (segment2.second - segment2.first).abs();
            }
            if(k+1 < object.second.nodes().size())
                segment2 = { object.second.position() + object.second.nodes()[k], object.second.position() + object.second.nodes()[k+1] };
        }
        if(!obj.empty())
            v_rayCastStruct.push_back({(ray.first-nearCross).abs(), len, obj, object.second.height(), v_reflectedRayCastStructure});
    }
    sort(v_rayCastStruct.begin(), v_rayCastStruct.end(), [](const RayCastStructure& lh, const RayCastStructure& rh) { return lh.distance > rh.distance; });
}

void Camera::updateDistances(World& world) {
    v_distances.clear();
    allCollisions.clear();

    for(int i = 0; i < 2*PI/d_fieldOfView*DISTANCES_SEGMENTS; i++) {
        double left = d_direction - d_fieldOfView/2;
        double right = d_direction + d_fieldOfView/2;
        // find distance to near object in that direction
        double direction = d_direction + ((double)i/DISTANCES_SEGMENTS - 0.5)*d_fieldOfView;

        pair<Point2D, Point2D> segment1 = {{x(), y()}, {x() + d_depth*cos(direction), y() + d_depth*sin(direction)}};

        std::vector<RayCastStructure> v_rayCastStructure;

        objectsRayCrossed(segment1, v_rayCastStructure, world, getName());

        if(!v_rayCastStructure.empty())
            v_distances.push_back(v_rayCastStructure);
        else
            v_distances.push_back({{d_depth, 0, "", 1}});
    }
}

void Camera::draw(sf::RenderWindow& window) {

    if(v_distances.size() == 0)
        updateDistances(W_world);

    sf::CircleShape circle;
    circle.setRadius(COLLISION_DISTANCE*SCALE);
    circle.setOutlineColor(OUTLINE_CAMERA_COLOR);
    circle.setFillColor(FILL_CAMERA_COLOR);
    circle.setOutlineThickness(OUTLINE_CAMERA_THICKNESS);
    circle.setPosition((float)x()*SCALE - COLLISION_DISTANCE*SCALE, (float)y()*SCALE - COLLISION_DISTANCE*SCALE);

    double left = d_direction - d_fieldOfView/2;
    double right = d_direction + d_fieldOfView/2;

    sf::ConvexShape triangle;
    triangle.setPointCount(CONVEX_NUMBER + 2);
    triangle.setPoint(0, sf::Vector2f(0, 0));
    for(int i = 0; i <= CONVEX_NUMBER; i++) {
        int index = v_distances[(int)i*DISTANCES_SEGMENTS/CONVEX_NUMBER].size() - 1;
        triangle.setPoint(i+1, sf::Vector2f(v_distances[(int)i*DISTANCES_SEGMENTS/CONVEX_NUMBER][index].distance*cos(left + (right - left)*i/CONVEX_NUMBER)*SCALE, v_distances[(int)i*DISTANCES_SEGMENTS/CONVEX_NUMBER][index].distance*sin(left + (right - left)*i/CONVEX_NUMBER)*SCALE));
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
        weapons[selectedWeapon].fire();
    }
    if(sf::Mouse::getPosition(window).x != localMousePosition.x) {
        double difference = sf::Mouse::getPosition(window).x - localMousePosition.x;
        localMousePosition = sf::Mouse::getPosition(window);
        d_direction += d_viewSpeed * difference;
    }

    shiftPrecise({dx, dy});
    return true;
}

pair<double, double> heightInPixels(double distance, double height) {
    pair<double, double> h = {0, 0};
    h.first = (1 - height/distance)*SCREEN_HEIGHT/2;
    h.second = (1 + 1/distance)*SCREEN_HEIGHT/2;
    return h;
}


void Camera::drawVerticalStrip(sf::RenderWindow &window, const RayCastStructure& obj, double distance, int shift, double s) {
    sf::ConvexShape polygon;
    polygon.setPointCount(4);

    pair<double, double> height_now = heightInPixels(distance, obj.height);

    int h1 = height_now.first + s;
    int h2 = height_now.second - s;

    polygon.setPoint(0, sf::Vector2f(0, h1));
    polygon.setPoint(1, sf::Vector2f(0, h2));
    polygon.setPoint(2, sf::Vector2f(SCREEN_WIDTH / DISTANCES_SEGMENTS, h2));
    polygon.setPoint(3, sf::Vector2f(SCREEN_WIDTH / DISTANCES_SEGMENTS, h1));

    int alpha = 255 * (1 - distance / d_depth);
    if (alpha > 255)
        alpha = 255;
    if (alpha < 0)
        alpha = 0;

    if (!b_textures)
        polygon.setFillColor({255, 174, 174, static_cast<sf::Uint8>(alpha)});
    else
        polygon.setFillColor({255, 174, 174, 255});

    polygon.setOutlineThickness(0); // we can make non zero thickness for debug
    polygon.setPosition(shift * SCREEN_WIDTH / DISTANCES_SEGMENTS, 0);
    if (abs(distance - d_depth) > 0.001)
        window.draw(polygon);

    double scaleFactor = (double) (h2 - h1) / SCREEN_HEIGHT;
    sf::Sprite sprite;
    if (obj.object != "" && b_textures) {
        sprite.setTexture(W_world[obj.object].loadTexture());
        sprite.setTextureRect(sf::IntRect(obj.progress * SCREEN_WIDTH, 0, SCREEN_WIDTH / DISTANCES_SEGMENTS, SCREEN_HEIGHT));
        sprite.setPosition(sf::Vector2f(shift * SCREEN_WIDTH / DISTANCES_SEGMENTS, h1)); // абсолютная позиция
        sprite.scale(1, scaleFactor);
        sprite.setColor({255, 255, 255, static_cast<sf::Uint8>(alpha)});
        window.draw(sprite);
    }
}

void Camera::recursiveDrawing(sf::RenderWindow& window, const std::vector<RayCastStructure>& v_RayCastStructure, double distance, int shift) {
    for(int k = 0; k < v_RayCastStructure.size(); k++) {
        drawVerticalStrip(window, v_RayCastStructure[k], v_RayCastStructure[k].distance + distance, shift, 0);
        if(!v_RayCastStructure[k].v_mirrorRayCast.empty()) {
            //drawVerticalStrip(window, v_distances[shift][k].v_mirrorRayCast[v_distances[shift][k].v_mirrorRayCast.size()-1], v_distances[shift][k].v_mirrorRayCast[v_distances[shift][k].v_mirrorRayCast.size()-1].distance + v_distances[shift][k].distance, shift, 0);
            recursiveDrawing(window, v_RayCastStructure[k].v_mirrorRayCast, v_RayCastStructure[k].distance, shift);
        }
    }
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
        recursiveDrawing(window, v_distances[i], 0, i);
        //for(int k = 0; k < v_distances[i].size(); k++) {
        //    drawVerticalStrip(window, v_distances[i][k], v_distances[i][k].distance, i, 0);
        //    if(!v_distances[i][k].v_mirrorRayCast.empty()) {
        //        drawVerticalStrip(window, v_distances[i][k].v_mirrorRayCast[v_distances[i][k].v_mirrorRayCast.size()-1], v_distances[i][k].v_mirrorRayCast[v_distances[i][k].v_mirrorRayCast.size()-1].distance + v_distances[i][k].distance, i, 0);
        //    }
        //}
    }

    weapons[selectedWeapon].draw(window);
}

double Camera::scalarWithNormal(Point2D edge, Point2D vector) {
    Point2D normal = {edge.y, -edge.x};
    normal = normal.normalize();
    double scalar = vector.x*normal.x + vector.y*normal.y;
    return scalar;
}

void Camera::shiftPrecise(Point2D vector) {

    if(!b_collision) {
        shift(vector);
        return;
    }

    for(auto c : allCollisions) {
        Point2D edgeVector = c.edge.second - c.edge.first;
        Point2D normal = {edgeVector.y, -edgeVector.x};
        normal = normal.normalize();
        double scalar = vector.x*normal.x + vector.y*normal.y;
        if(scalarWithNormal(edgeVector, vector) < 0) {
            if(c.distance - abs(scalar) < COLLISION_DISTANCE) {
                vector.x -= normal.x*scalar;
                vector.y -= normal.y*scalar;
            }
        }
    }

    shift(vector);
}

void Camera::previousWeapon() {
    if(selectedWeapon > 0)
        selectedWeapon--;
    else
        selectedWeapon = weapons.size()-1;
}

void Camera::nextWeapon() {
    if(selectedWeapon < weapons.size()-1)
        selectedWeapon++;
    else
        selectedWeapon = 0;
}