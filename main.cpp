#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <map>
#include <iostream>
#include <chrono>
#include "settings.h"
#include "Poligon2D.h"
#include "Circle2D.h"
#include "Object2D.h"
#include "World.h"
#include "Camera.h"
#include "Menu.h"

using namespace std;
/*
class Ismth {
    virtual void draw() = 0;
};

class Weapon : public Ismth{
public:
    Weapon() = default;

    void draw() override {};
};

class Shotgun : public Weapon {
public:
    Shotgun() = default;

    void draw() override {};
};
*/
int main()
{
    //vector<Weapon> weapons;
    //Shotgun weapon1;
    //weapons.push_back(weapon1);

    //pair<Point2D, Point2D> segment1 = {{1, 1}, {3, 1}};
    //pair<Point2D, Point2D> segment2 = {{2, 0.5}, {2, 0}};
    //Point2D crossPoint;
    //bool crss = Object2D::segments_crossing(segment1, segment2, crossPoint);
    //cout << "collision : " << crss << " in {x, y} : {" << crossPoint.x << ", " << crossPoint.y << "}" << endl;

    // FROM HERE

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Pseudo3DEngine");

    World world(100, 100);
    Camera camera(world, {2.5, 0});

    // objects
    Poligon2D wall1({{0, 0}, {0, .1}, {5, .1}, {5, 0}}, {0, 0});
    Poligon2D wall2({{0, 0}, {.1, 0}, {.1, 5}, {0, 5}}, {0, 0});
    Poligon2D wall3({{0, 0}, {0, .1}, {5, .1}, {5, 0}}, {0, 5});
    Poligon2D wall4({{0, 0}, {.1, 0}, {.1, 5}, {0, 5}}, {5, 0});

    Circle2D sphere1(.5, {0, 0}, COLUMN_TEXTURE);
    Circle2D sphere2(.5, {5, 0}, COLUMN_TEXTURE);
    Circle2D sphere3(.5, {5, 5}, COLUMN_TEXTURE);
    Circle2D sphere4(.5, {0, 5}, COLUMN_TEXTURE);

    Poligon2D object1({{1, 1}, {2.05, 1}, {2, 2}}, {1, .1}, INSTRUCTION_TEXTURE);
    Poligon2D object2({{0, 0}, {1, 0}, {1, 1}, {0, 1}}, {.3, 3}, DOG_TEXTURE);
    Poligon2D object3({{1, 1}, {2, 2}, {1, 3}, {1, 2}}, {1, 1});
    Poligon2D object4({{0, 0}, {.1, 0}, {.1, .1}, {0, .1}}, {3.5, 1});
    Poligon2D object5({{0, 0}, {.3, 0}, {.3, .3}, {0, .3}}, {4, 3});
    Poligon2D object6({{0, 0}, {.3, 0}, {.3, .3}, {0, .3}}, {3, 4});
    //camera
    world.addObject2D(camera, "camera");

    //world.addObject2D(wall1, "wall1");
    world.addObject2D(wall2, "wall2");
    world.addObject2D(wall3, "wall3");
    world.addObject2D(wall4, "wall4");

    world.addObject2D(sphere1, "zsphere1");
    world.addObject2D(sphere2, "zsphere2");
    world.addObject2D(sphere3, "zsphere3");
    world.addObject2D(sphere4, "zsphere4");

    world.addObject2D(object1, "object1");
    world.addObject2D(object2, "object2");
    world.addObject2D(object3, "object3");
    world.addObject2D(object4, "object4");
    world.addObject2D(object5, "object5");
    world.addObject2D(object6, "object6");

    Menu menu;

    auto tp1 = chrono::system_clock::now(); // Переменные для подсчета
    auto tp2 = chrono::system_clock::now(); // пройденного времени

    while (window.isOpen())
    {
        tp2 = chrono::system_clock::now();
        chrono::duration <double> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        double d_elapsedTime = elapsedTime.count();
        std::string title = "Pseudo3DEngine " + std::to_string((double)1/elapsedTime.count()) + "fps";
        window.setTitle(title);
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();     // отчистка
        if(!menu.isPaused()) {
            camera.updateDistances(world);
            camera.drawCameraView(window);
            world.draw(window);
            if(!camera.keyboardControl(d_elapsedTime, window)) {
                menu.setPause();
            };
        } else {
            if(menu.isTextures() != camera.isTextures())
                camera.switchTextures();

            menu.drawMenu(window, d_elapsedTime);
        }
        window.display();   // отображение
    }

    return 0;
}