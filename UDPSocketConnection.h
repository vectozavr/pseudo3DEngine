//
// Created by ivan- on 13.02.2020.
//

#ifndef PSEUDO3DENGINE_UDPSOCKETCONNECTION_H
#define PSEUDO3DENGINE_UDPSOCKETCONNECTION_H

#include <iostream>
#include <SFML/Network.hpp>
#include "World.h"
#include "Camera.h"

class UDPSocketConnection {
private:
    World& W_world;
    Camera& C_camera;
    sf::UdpSocket socket;

    std::map<std::string, Camera> m_cameras;
public:
    UDPSocketConnection(World& world, Camera& camera) : W_world(world), C_camera(camera) {}

    void bind(short unsigned port) {
        socket.bind(port);
        socket.setBlocking(false);
    }

    void update() {
        double x;
        double y;
        std::string senderName;
        int health;
        std::string killedName;

        sf::Packet packet;
        sf::IpAddress sender;

        bool ack1 = false;
        bool ack2 = false;

        short unsigned int port;
        while(socket.receive(packet, sender, port) == sf::Socket::Status::Done)
            packet >> x >> y >> senderName >> ack2;

        if(ack2)
            C_camera.cleanLastKill();

        if(killedName == C_camera.getName()) {
            C_camera.setPosition({2.5, 0});
            C_camera.fullHealth();
            ack1 = true;
        }
        std::cout << senderName << " " << port << std::endl;
        if(W_world.isExist(senderName)) {
            W_world[senderName].setPosition({x, y});
        } else {
            Camera camera(W_world, {0, 0});
            camera.setName(senderName);
            m_cameras.insert({senderName, camera});
            W_world.addObject2D(m_cameras.at(senderName), senderName);
        }

        sf::Packet packetSend;
        packetSend << C_camera.x() << C_camera.y() << C_camera.getName() << C_camera.lastKill() << ack1;
        socket.send(packetSend, "192.168.1.255", 54334);
    }
};


#endif //PSEUDO3DENGINE_UDPSOCKETCONNECTION_H