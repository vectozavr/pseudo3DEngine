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
#include "UDPSocketConnection.h"

using namespace std;

int main()
{
    sf::Music music;
    music.openFromFile("sounds/unrealSuperHero3.ogg");
    music.setVolume(10);
    music.setLoop(true);
    music.play();

    sf::Music backSounds;
    backSounds.openFromFile("sounds/backSounds.ogg");
    backSounds.setVolume(20);
    backSounds.pause();

    // FROM HERE

    vector<Point2D> columnPositions = {{0,0}, {3,0}, {3,3}, {6,3}, {6,8}, {3,8}, {3,11}, {0,11}, {0,8}, {-3,8}, {-3,3}, {0,3}};
    vector<Circle2D> columns(12);
    vector<Poligon2D> walls (12);

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Pseudo3DEngine");

    World world(100, 100);
    Camera camera(world, {1.5, 1.5});

    for(int i = 0; i < columns.size(); i++) {
        columns[i].setPosition(columnPositions[i]);
        if(i != columns.size()-1)
            walls[i].setPoints2D({columnPositions[i], columnPositions[i+1]});
        else
            walls[i].setPoints2D({columnPositions[i], columnPositions[0]});

        world.addObject2D(walls[i], "wall" + std::to_string(i+1));
        world.addObject2D(columns[i], "col" + std::to_string(i+1));
    }

    UDPSocketConnection udpSocketConnection(world, camera);
    udpSocketConnection.bind("192.168.137.255", 54001); // 192.168.137.255

    //wall4.makeMirror();

    Poligon2D object1({{1, 1}, {2.05, 1}, {2, 2}}, {2.7, 5}, 1, INSTRUCTION_TEXTURE);
    Poligon2D object2({{0, 0}, {0.5, 0}, {0.5, 0.5}, {0, 0.5}}, {-1.9, 6.7}, 1, FROZEN_TEXTURE);
    Poligon2D object3({{1, 1}, {2, 2}, {1, 2}, {1, 2}}, {-0.79, 3.95});
    Poligon2D object4({{0, 0}, {.1, 0}, {.1, .1}, {0, .1}}, {1.92, 6.18});
    Poligon2D object5({{0, 0}, {.3, 0}, {.3, .3}, {0, .3}}, {1.84, 9});
    Poligon2D object6({{0, 0}, {.3, 0}, {.3, .3}, {0, .3}}, {1.11, 7}, 0);
    Poligon2D object7({{0, 0}, {.1, 0}, {.1, .1}, {0, .1}}, {1, 2}, 0);

    Poligon2D object8({{0, 0}, {.3, 0}, {.3, .3}, {0, .3}}, {3.6, 4.6}, 0.9);
    Poligon2D object9({{0, 0}, {.3, 0}, {.3, .3}, {0, .3}}, {1.5, 4.2}, 0.9);
    Poligon2D object10({{0, 0}, {.1, 0}, {.1, .1}, {0, .1}}, {2.5, 5.2}, 0.9);

    //object2.makeMirror();
    object3.makeMirror();

    //cameraw
    world.addObject2D(camera, camera.getName());

    world.addObject2D(object1, "object1");
    world.addObject2D(object2, "object2");
    world.addObject2D(object3, "object3");
    world.addObject2D(object4, "object4");
    world.addObject2D(object5, "object5");
    world.addObject2D(object6, "object6");
    world.addObject2D(object7, "object7");

    world.addObject2D(object8, "object8");
    world.addObject2D(object9, "object9");
    world.addObject2D(object10, "object10");

    Menu menu;

    auto tp1 = chrono::system_clock::now(); // Переменные для подсчета
    auto tp2 = chrono::system_clock::now(); // пройденного времени

    while (window.isOpen())
    {
        tp2 = chrono::system_clock::now();
        chrono::duration <double> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        double d_elapsedTime = elapsedTime.count();
        std::string title = "Pseudo3DEngine " + std::to_string((double)1/elapsedTime.count()) + "fps. x:" + std::to_string(camera.x()) + ", y:" + std::to_string(camera.y());
        window.setTitle(title);
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();     // отчистка
        if(!menu.isPaused()) {
            udpSocketConnection.update();
            window.setMouseCursorVisible(false);
            camera.updateDistances(world);
            camera.drawCameraView(window);
            //world.draw(window);
            if(!camera.keyboardControl(d_elapsedTime, window)) {
                menu.setPause();
                music.play();
                backSounds.pause();
            };
        } else {
            window.setMouseCursorVisible(true);
            if(menu.isTextures() != camera.isTextures())
                camera.switchTextures();
            if(menu.isCollision() != camera.isCollision())
                camera.switchCollision();

            menu.drawMenu(window, d_elapsedTime);
            //FOR MUSICS
            if(!menu.isPaused()) {
                backSounds.play();
                music.pause();
            }
        }
        window.display();   // отображение
    }

    return 0;
}