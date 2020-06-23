#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>
#include "settings.h"
#include "Poligon2D.h"
#include "World.h"
#include "Camera.h"
#include "Menu.h"
#include "Time.h"
#include "ServerUDP.h"
#include "ClientUDP.h"
#include "Object2D.h"

using namespace std;

// Read server/client settings and start both.
// If client doesn't connect to the localhost - server doesn't start.
void InitNetwork(ServerUDP& server, ClientUDP& client)
{
    std::string clientIp;
    sf::Uint16 clientPort;
    sf::Uint16 serverPort;
    std::ifstream connectfile("connect.txt", std::ifstream::in);

    // If failed to read client settings
    if (!connectfile.is_open() || !(connectfile >> clientIp >> clientPort) || sf::IpAddress(clientIp) == sf::IpAddress::None)
    {
        connectfile.close();
        // Create file and write default settings
        clientIp = "127.0.0.1";
        clientPort = 54000;
        std::ofstream temp("connect.txt", std::ofstream::out);
        temp << clientIp << std::endl << clientPort;
        temp.close();
    }
    connectfile.close();

    // If failed to read server settings
    connectfile.open("server.txt", std::ifstream::in);
    if (!connectfile.is_open() || !(connectfile >> serverPort))
    {
        connectfile.close();
        // Create file and write default settings
        serverPort = 54000;
        std::ofstream temp("server.txt", std::ofstream::out);
        temp << serverPort;
        temp.close();
    }
    connectfile.close();
    
    if (clientIp == sf::IpAddress::LocalHost)
        server.start(serverPort);
    client.connect(clientIp, clientPort);
}

int main()
{
    // Window should be created first because of drawing context.
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Pseudo3DEngine");
    window.setFramerateLimit(60);

    // Sounds

    sf::Music music;
    music.openFromFile("sounds/unrealSuperHero3.ogg");
    music.setVolume(10);
    music.setLoop(true);
    music.play();

    sf::Music backSounds;
    backSounds.openFromFile("sounds/backSounds.ogg");
    backSounds.setVolume(20);
    backSounds.setLoop(true);
    backSounds.pause();

    // World Init

    vector<Point2D> columnPositions = { {0,0}, {3,0}, {3,3}, {6,3}, {6,8}, {3,8}, {3,11}, {0,11}, {0,8}, {-3,8}, {-3,3}, {0,3} };
    vector<Circle2D> columns(12);
    vector<Poligon2D> walls(12);

    World world(100, 100);
    Camera* camera = nullptr;

    for (size_t i = 0; i < columns.size(); i++) {
        columns[i].setPosition(columnPositions[i]);
        if (i != columns.size() - 1)
            walls[i].setPoints2D({ columnPositions[i], columnPositions[i + 1] });
        else
            walls[i].setPoints2D({ columnPositions[i], columnPositions[0] });

        world.addObject2D(walls[i], "wall" + std::to_string(i + 1));
        world.addObject2D(columns[i], "col" + std::to_string(i + 1));
    }
    
    //walls[9].setMirror();

    Poligon2D object1({ {1, 1}, {2.05, 1}, {2, 2} }, { 2.7, 5 }, 1, INSTRUCTION_TEXTURE);
    Poligon2D object2({ {0, 0}, {1, 0}, {1, 1}, {0, 1} }, { -1.9, 6.7 }, 1, FROZEN_TEXTURE);
    Poligon2D object3({ {1, 1}, {2, 2}, {1, 2}, {1, 2} }, { -0.79, 3.95 });
    Poligon2D object4({ {0, 0}, {.1, 0}, {.1, .1}, {0, .1} }, { 1.92, 6.18 });
    Poligon2D object5({ {0, 0}, {.3, 0}, {.3, .3}, {0, .3} }, { 1.84, 9 });
    Poligon2D object6({ {0, 0}, {.3, 0}, {.3, .3}, {0, .3} }, { 1.11, 7 }, 0.5);
    Poligon2D object7({ {0, 0}, {.1, 0}, {.1, .1}, {0, .1} }, { 1, 2 }, 0.5);

    Poligon2D object8({ {0, 0}, {.3, 0}, {.3, .3}, {0, .3} }, { 3.6, 4.6 }, 0.9);
    Poligon2D object9({ {0, 0}, {.3, 0}, {.3, .3}, {0, .3} }, { 1.5, 4.2 }, 0.9);
    Poligon2D object10({ {0, 0}, {.1, 0}, {.1, .1}, {0, .1} }, { 2.5, 5.2 }, 0.9);

    //object2.setMirror();
    object3.setMirror();

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

    ServerUDP server(world);
    ClientUDP client(world);
    server.addSpawn({ 1.5, 1.5 });
    server.addSpawn({ 1.5, 9 });

    // Game loop

    while (window.isOpen())
    {
        // Time update
        Time::update();
        double d_elapsedTime = Time::deltaTime();

        // Title update
        std::string title = "Pseudo3DEngine " + std::to_string((double)1 / d_elapsedTime) + "fps.";
        if (camera != nullptr)
            title += " x:" + std::to_string(camera->x()) + ", y:" + std::to_string(camera->y());
        window.setTitle(title);

        // Close event search
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Actually game
        window.clear();
        if (!menu.isPaused())
        {
            window.setMouseCursorVisible(false);
            camera->updateDistances(world);
            camera->drawCameraView(window);
            // world.draw(window); // top-down debug map. To fix exception - look into "Camera::updateDistances"

            // if escape was pressed
            if (!camera->keyboardControl(d_elapsedTime, window))
            {
                client.disconnect();
                server.stop();
                menu.setPause();
                music.play();
                backSounds.pause();
            };

            // Network update (must be after camera use)
            server.update();
            client.update();

            // if client timeout or disconnected
            if (!client.isWorking())
            {
                menu.setPause();
                music.play();
                backSounds.pause();
                camera = nullptr;
            };
        }
        // Menu
        else
        {
            window.setMouseCursorVisible(true);
            menu.drawMenu(window, d_elapsedTime);
            // if "play game" pressed
            if (!menu.isPaused()) {
                window.clear({ 255,255,255 });
                window.display();
                InitNetwork(server, client);
                // Waiting for connect and updating server if it's same window
                while (client.isWorking() && !client.connected())
                {
                    client.update();
                    server.update();
                    Time::update();
                }
                // If connect fail - return to menu
                if (!client.isWorking())
                {
                    menu.setPause();
                    server.stop();
                }
                // If connect success - init camera and start game
                else
                {
                    camera = client.localPlayer();
                    camera->client = &client;
                    camera->setTextures(menu.isTextures());
                    camera->setSmooth(menu.isSmooth());
                    camera->setCollision(menu.isCollision());
                    backSounds.play();
                    music.pause();
                }
            }
        }
        window.display();
    }

    ResourceManager::unloadAllResources();
    return 0;
}
