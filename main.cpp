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
#include <iostream>
#include <sstream>
#include "Enemy.h"
#include "GeneticAlgorithm.h"

using namespace std;

// Read server/client settings and start both.
// If client doesn't connect to the localhost - server doesn't start.
void InitNetwork(ServerUDP& server, ClientUDP& client)
{
    std::string clientIp;
    sf::Uint16 clientPort;
    sf::Uint16 serverPort;
    std::ifstream connectfile("../connect.txt", std::ifstream::in);

    // If failed to read client settings
    if (!connectfile.is_open() || !(connectfile >> clientIp >> clientPort) || sf::IpAddress(clientIp) == sf::IpAddress::None)
    {
        connectfile.close();
        // Create file and write default settings
        clientIp = "127.0.0.1";
        clientPort = 54000;
        std::ofstream temp("../connect.txt", std::ofstream::out);
        temp << clientIp << std::endl << clientPort;
        temp.close();
    }
    connectfile.close();

    // If failed to read server settings
    connectfile.open("../server.txt", std::ifstream::in);
    if (!connectfile.is_open() || !(connectfile >> serverPort))
    {
        connectfile.close();
        // Create file and write default settings
        serverPort = 54000;
        std::ofstream temp("../server.txt", std::ofstream::out);
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
    /*
    sf::Music music;
    music.openFromFile("sounds/unrealSuperHero3.ogg");
    music.setVolume(10);
    music.setLoop(true);
    music.play();
    */

    sf::Music backSounds;
    backSounds.openFromFile("../sounds/backSounds.ogg");
    backSounds.setVolume(20);
    backSounds.setLoop(true);
    backSounds.pause();

    // World Init
    World world;
    Camera* camera = nullptr;

    world.load3DObj("../maps/city.obj", WALL_TEXTURE, 0.03, {-1, 1});
    world.addBonusPoint({0.5, -0.5});
    world.addBonusPoint({9.5, -4.0});
    world.addBonusPoint({13.0, -15.5});
    world.addBonusPoint({30.5, 5.5});
    world.addBonusPoint({6.5, 18.0});
    world.addBonusPoint({9.5, 26.0});

    world.addBonusPoint({42, 37});
    world.addBonusPoint({42, 34});
    world.addBonusPoint({42, -6});
    world.addBonusPoint({40, 1});
    world.addBonusPoint({45, 12});
    world.addBonusPoint({52, -10});

    Menu menu;

    bool learn = false;
    bool botView = false;

    ServerUDP server(world);
    ClientUDP client(world);

    server.loadObjSpawns("../maps/spawns_city_big.obj", 0.03);

    // generation of AI's
    GeneticAlgorithm generation(world, server, 11);
    generation.loadNetwork("../neuralNetwork.txt");
    //generation.saveNetwork("../bigChange1.txt");

    double dt = 0.02;
    int iterations = 0;

    // Game loop
    while (window.isOpen())
    {
        // Time update
        Time::update();
        double d_elapsedTime = Time::deltaTime();

        // Title update
        std::string title = "Pseudo3DEngine " + std::to_string((double)1 / d_elapsedTime) + "fps.";
        if (camera != nullptr)
            title += " x:" + std::to_string(camera->x()) + ", y:" + std::to_string(camera->y()) + ", health: " + std::to_string(camera->health());
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
            if(!learn) {
                window.setMouseCursorVisible(false);
                camera->updateDistances();
                camera->drawCameraView(window);
                world.rotateAllBonuses(d_elapsedTime);
            }
            // world.draw(window); // top-down debug map. To fix exception - look into "Camera::updateDistances"

            // if escape was pressed
            if (!camera->keyboardControl(d_elapsedTime, window))
            {
                client.disconnect();
                server.stop();
                menu.setPause();
                //music.play();
                //backSounds.pause();
            };

            // Network update (must be after camera use)
            server.update();
            client.update();
            if(learn)
                generation.update(dt, d_elapsedTime);
            else
                generation.update(d_elapsedTime, d_elapsedTime);

            if(iterations != 0 && ((iterations % 250) == 0) && learn) { // 500 * dt = 10 sec
                generation.newGeneration();
                cout << "gen: " << generation.generation() << " : score = \t" << generation.maxScore() <<  endl;
            }
            if(iterations != 0 && ((iterations % 2000) == 0) && learn) {
                cout << "saved gen: " << generation.generation() << " to file " << endl;
                generation.saveNetwork("../neuralNetwork.txt");
                generation.logScore("../log.txt");
            }
            iterations++;

            // if client timeout or disconnected
            if (!client.isWorking())
            {
                menu.setPause();
                //music.play();
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
                    if(!botView) {
                        camera = client.localPlayer();
                        camera->client = &client;

                        camera->setTextures(menu.isTextures());
                        camera->setSmooth(menu.isSmooth());
                        camera->setCollision(menu.isCollision());

                        camera->setPosition({0, 0});
                    }

                    //backSounds.play();
                    //music.pause();
                }

                generation.connect(client);

                if(botView) {
                    camera = generation.connectToEnemyCamera();
                    camera->setTextures(menu.isTextures());
                    camera->setSmooth(menu.isSmooth());
                    camera->setCollision(menu.isCollision());
                    //client.localPlayer()->setPosition({250, 0});
                }

                //camera = generation.connectToEnemyCamera();
                //camera->setTextures(menu.isTextures());
                //camera->setSmooth(menu.isSmooth());
                //camera->setCollision(menu.isCollision());
                //client.localPlayer()->setPosition({250, 0});

                if(learn)
                    camera->setPosition({252, 0});
            }
            if(learn)
                window.display();
        }
        if(!learn)
            window.display();
    }

    if(learn)
        generation.saveNetwork("../neuralNetwork.txt");

    ResourceManager::unloadAllResources();

    return 0;
}
