//
// Created by ivan- on 18.01.2020.
//

#include <cmath>
//#include <windows.h>
#include "Camera.h"


using namespace std;

void Camera::recursiveIncreaseDistance(std::vector<RayCastStructure>& v_RayCastStructure, double distance) {
    for(auto& rcs : v_RayCastStructure) {
        rcs.distance += distance;
        recursiveIncreaseDistance(rcs.v_mirrorRayCast, distance);
    }
}

void Camera::objectsRayCrossed(pair<Point2D, Point2D> ray, std::vector<RayCastStructure> &v_rayCastStruct, const std::string& name, int reflections) {
    std::string obj;
    double len = 0;
    double progress = 0;
    for(auto object : W_world.objects()) {
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

                        if(reflections < 40) {
                            objectsRayCrossed(newRay, v_reflectedRayCastStructure, object.first,reflections + 1);
                            recursiveIncreaseDistance(v_reflectedRayCastStructure, (ray.first - nearCross).abs());
                        }
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

void Camera::updateDistances(const World& world) {
    v_distances.clear();
    allCollisions.clear();

    for(int i = 0; i < 2*PI/d_fieldOfView*DISTANCES_SEGMENTS; i++) {
        double left = d_direction - d_fieldOfView/2;
        double right = d_direction + d_fieldOfView/2;
        // find distance to near object in that direction
        double direction = d_direction + ((double)i/DISTANCES_SEGMENTS - 0.5)*d_fieldOfView;

        pair<Point2D, Point2D> segment1 = {{x(), y()}, {x() + d_depth*cos(direction), y() + d_depth*sin(direction)}};

        std::vector<RayCastStructure> v_rayCastStructure;

        objectsRayCrossed(segment1, v_rayCastStructure, getName());

        for(const auto& rcs : v_rayCastStructure)
            if((m_playersOnTheScreen.count(rcs.object) == 0) && (W_world[rcs.object].type() == 1))
                m_playersOnTheScreen.insert({rcs.object, dynamic_cast<Camera&>(W_world[rcs.object])});

        if(!v_rayCastStructure.empty())
            v_distances.push_back(v_rayCastStructure);
        else
            v_distances.push_back({{d_depth, 0, "", 1}});
    }
}

void Camera::draw(sf::RenderWindow& window) {

    if(v_distances.empty() || i_health <= 0)
        return;

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

std::pair<std::string, double> Camera::cameraRayCheck(RayCastStructure &structure) {
    std::pair<std::string, double> result = {"", 1};
    if(W_world[structure.object].type() == 1) {
        result.first = structure.object;
        result.second = structure.distance;
    }
    else if(!structure.v_mirrorRayCast.empty()) {
        return cameraRayCheck(structure.v_mirrorRayCast[structure.v_mirrorRayCast.size()-1]);
    }
    return result;
}

void Camera::fire() {
    pair<Point2D, Point2D> segment1 = {{x(), y()}, {x() + d_depth*cos(d_direction), y() + d_depth*sin(d_direction)}};
    std::vector<RayCastStructure> v_rayCastStructure;
    objectsRayCrossed(segment1, v_rayCastStructure, getName());

    if(!v_rayCastStructure.empty()) {
        std::pair<std::string, double> hitted = cameraRayCheck(v_rayCastStructure[v_rayCastStructure.size()-1]);
        //W_world.removeObject2D(hitted.first);
        if(hitted.first == "") return;

        dynamic_cast<Camera&>(W_world[hitted.first]).updateDistances(W_world);
        double dir = 2*PI * rand() / RAND_MAX;
        dynamic_cast<Camera&>(W_world[hitted.first]).shiftPrecise({0.05*cos(dir), 0.05*sin(dir)});

        if(dynamic_cast<Camera&>(W_world[hitted.first]).reduceHealth(static_cast<int>(v_weapons[i_selectedWeapon].damage() / hitted.second))) {
            //W_world.removeObject2D(hitted.first);
            W_world[hitted.first].setPosition({2.5, 0});
            s_lastKill = hitted.first;
            dynamic_cast<Camera&>(W_world[hitted.first]).setHealth(100);
        }
    }
}

bool Camera::keyboardControl(double elapsedTime, sf::RenderWindow& window) {
    double dx = 0;
    double dy = 0;

    // left and right
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        dx += sin(d_direction);
        dy += -cos(d_direction);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        dx += -sin(d_direction);
        dy += cos(d_direction);
    }
    // forward and backward
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        dx += cos(d_direction);
        dy += sin(d_direction);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        dx += -cos(d_direction);
        dy += -sin(d_direction);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        return false;
    }
    // Mouse
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        // левая кнопка мыши нажата, стреляем!
        if(v_weapons[i_selectedWeapon].fire())
            fire();
    }
    if(sf::Mouse::getPosition(window).x != localMousePosition.x) {
        double difference = sf::Mouse::getPosition(window).x - localMousePosition.x;
        sf::Mouse::setPosition({SCREEN_WIDTH/2, SCREEN_HEIGHT/2});
        localMousePosition = sf::Mouse::getPosition(window);
        d_direction += d_viewSpeed * difference;
    }

    if((dx*dx + dy*dy) > d_walkSpeed * elapsedTime * d_walkSpeed * elapsedTime / 10) {
       if(walkSound.getStatus() != sf::Sound::Status::Playing)
           walkSound.play();
    } else {
        walkSound.pause();
    }

    shiftPrecise({dx * d_walkSpeed * elapsedTime * ((double)i_health/100), dy * d_walkSpeed * elapsedTime * ((double)i_health/100)});
    return true;
}

pair<double, double> heightInPixels(double distance, double height) {
    pair<double, double> h = {0, 0};
    h.first = (1 - height/distance)*SCREEN_HEIGHT/2;
    h.second = (1 + 1/distance)*SCREEN_HEIGHT/2;
    return h;
}


void Camera::drawVerticalStrip(sf::RenderWindow &window, const RayCastStructure& obj, int shift, int f) {
    sf::ConvexShape polygon;
    polygon.setPointCount(4);

    double d_angle = -d_fieldOfView/2 + shift * d_fieldOfView / DISTANCES_SEGMENTS;
    pair<double, double> height_now = heightInPixels(cos(0)*obj.distance, obj.height);

    int h1 = height_now.first;
    int h2 = height_now.second;

    polygon.setPoint(0, sf::Vector2f(0, h1));
    polygon.setPoint(1, sf::Vector2f(0, h2));
    polygon.setPoint(2, sf::Vector2f(SCREEN_WIDTH / DISTANCES_SEGMENTS, h2));
    polygon.setPoint(3, sf::Vector2f(SCREEN_WIDTH / DISTANCES_SEGMENTS, h1));

    int alpha = 255 * (1 - obj.distance / d_depth);
    if (alpha > 255)
        alpha = 255;
    if (alpha < 0)
        alpha = 0;

    alpha = 255 - alpha;

    if (!b_textures)
        polygon.setFillColor({255, 174, 174, static_cast<sf::Uint8>(alpha)});
    else
        polygon.setFillColor({255, 255, 255, static_cast<sf::Uint8>(alpha)});

    polygon.setOutlineThickness(0); // we can make non zero thickness for debug
    polygon.setPosition(shift * SCREEN_WIDTH / DISTANCES_SEGMENTS, 0);

    double scaleFactor = (double) (h2 - h1) / SCREEN_HEIGHT;
    sf::Sprite sprite;
    if (obj.object != "" && b_textures) {
        int left = obj.progress * SCREEN_WIDTH;
        int top = 0;
        if(W_world[obj.object].isMirror()) { // In case of mirror
            sprite.setTexture(W_world.skyTexture());
            left = (d_direction/10) * SCREEN_WIDTH;
            top = sprite.getTextureRect().height/2-SCREEN_HEIGHT/2;
        } else {
            sprite.setTexture(W_world[obj.object].loadTexture());
        }
        sprite.setTextureRect(sf::IntRect(left, top, SCREEN_WIDTH / DISTANCES_SEGMENTS, SCREEN_HEIGHT));
        sprite.setPosition(sf::Vector2f(shift * SCREEN_WIDTH / DISTANCES_SEGMENTS, h1)); // абсолютная позиция
        sprite.scale(1, scaleFactor);
        //sprite.setColor({static_cast<sf::Uint8>(alpha), static_cast<sf::Uint8>(alpha), static_cast<sf::Uint8>(alpha)});
        window.draw(sprite);
    }

    if (abs(obj.distance - d_depth) > 0.001)
        window.draw(polygon);

    int x = shift*SCREEN_WIDTH/DISTANCES_SEGMENTS;

    if((f == 0) || (x % FLOOR_SEGMENT_SIZE != 0)) return;
    //sf::Sprite floor;
    //floor.setTexture(W_world.floorTexture());
    for(int z = h2; z < SCREEN_HEIGHT; z += FLOOR_SEGMENT_SIZE) {

        double l = (double)1/(z - SCREEN_HEIGHT/2);

        int alpha2 = 255* 2*(z-SCREEN_HEIGHT/2) / SCREEN_HEIGHT;

        // SPRITE
        if (b_textures) {
            int left = (280*position().x + 100000*l*cos(d_angle + d_direction));
            int top  = (-280*position().y - 100000*l*sin(d_angle + d_direction));

            sf::Sprite& floor = W_world.floor();

            floor.setTextureRect(sf::IntRect(left, top, FLOOR_SEGMENT_SIZE, FLOOR_SEGMENT_SIZE));
            floor.setPosition(sf::Vector2f(x, z)); // абсолютная позиция
            //sprite.scale(1, scaleFactor);
            floor.setColor({255, 255, 255, static_cast<sf::Uint8>(alpha2)});
            window.draw(floor);
        }

        //window.draw(floor);
    }
}

void Camera::recursiveDrawing(sf::RenderWindow& window, const std::vector<RayCastStructure>& v_RayCastStructure, int shift, int rec) {
    int i = 0;
    for(const auto & k : v_RayCastStructure) {
        if(i + 1 != v_RayCastStructure.size() || (rec != 1))
            drawVerticalStrip(window, k, shift, 0);
        else
            drawVerticalStrip(window, k, shift, 1);
        i++;
        if(!k.v_mirrorRayCast.empty())
            recursiveDrawing(window, k.v_mirrorRayCast, shift, 2);
    }
}

void Camera::drawHealth(sf::RenderWindow& window, int xPos, int yPos, int width, int healthProgress) {
    sf::ConvexShape polygon1;
    polygon1.setPointCount(4);
    sf::ConvexShape polygon2;
    polygon2.setPointCount(4);

    polygon1.setPoint(0, sf::Vector2f(xPos, yPos));
    polygon1.setPoint(1, sf::Vector2f(xPos + width, yPos));
    polygon1.setPoint(2, sf::Vector2f(xPos + width, yPos + 20));
    polygon1.setPoint(3, sf::Vector2f(xPos, yPos + 20));

    polygon2.setPoint(0, sf::Vector2f(xPos, yPos));
    polygon2.setPoint(1, sf::Vector2f(xPos + width*healthProgress/100, yPos));
    polygon2.setPoint(2, sf::Vector2f(xPos + width*healthProgress/100, yPos + 20));
    polygon2.setPoint(3, sf::Vector2f(xPos, yPos + 20));

    polygon1.setFillColor({255, 174, 174, 100});
    polygon2.setFillColor({static_cast<sf::Uint8>((100 - healthProgress)*255), static_cast<sf::Uint8>(healthProgress*255/100), 0, 100});

    polygon1.setOutlineThickness(3); // we can make non zero thickness for debug
    window.draw(polygon1);
    window.draw(polygon2);
}

void Camera::drawCameraView(sf::RenderWindow& window) {

    if(v_distances.empty())
        return;
    //SKY AND FLOOR
    if(b_textures) {
        sf::Sprite sprite_sky;
        sprite_sky.setTexture(W_world.skyTexture());
        sprite_sky.setTextureRect(sf::IntRect(d_direction * SCREEN_WIDTH/2, sprite_sky.getTextureRect().height/2-SCREEN_HEIGHT/2, SCREEN_WIDTH, 1080));
        sprite_sky.setPosition(sf::Vector2f(0,0)); // абсолютная позиция
        window.draw(sprite_sky);
    }

    for(int i = 0; i < DISTANCES_SEGMENTS; i++)
        recursiveDrawing(window, v_distances[i], i);

    //m_playersOnTheScreen
    for(const auto& player : m_playersOnTheScreen) {
        Point2D enemyDirection = (player.second.position() - position()).normalize();
        Point2D cameraLeftDirection = {cos(d_direction - d_fieldOfView/2), sin(d_direction - d_fieldOfView/2)};
        double angle = acos(enemyDirection*cameraLeftDirection);
        if((cameraLeftDirection.x*enemyDirection.y - enemyDirection.x*cameraLeftDirection.y) > 0) {
            int xPos = SCREEN_WIDTH * angle/d_fieldOfView;
            int yPos = heightInPixels((player.second.position() - position()).abs(), 1).first;
            auto healthProgress = (double)player.second.health();
            drawHealth(window, xPos-50, yPos, 100, healthProgress);
        }
    }
    drawHealth(window, 50, SCREEN_HEIGHT - 50, 400, health());
    v_weapons[i_selectedWeapon].draw(window);
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
    if(i_selectedWeapon > 0)
        i_selectedWeapon--;
    else
        i_selectedWeapon = v_weapons.size() - 1;
}

void Camera::nextWeapon() {
    if(i_selectedWeapon < v_weapons.size() - 1)
        i_selectedWeapon++;
    else
        i_selectedWeapon = 0;
}

bool Camera::reduceHealth(double damage) {
    if(i_health > 100)
        i_health = 100;
    i_health -= damage;
    return i_health < 0;
}
