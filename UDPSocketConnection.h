//
// Created by ivan- on 13.02.2020.
//

#ifndef PSEUDO3DENGINE_UDPSOCKETCONNECTION_H
#define PSEUDO3DENGINE_UDPSOCKETCONNECTION_H

#include <iostream>
#include <SFML/Network.hpp>
#include <utility>
#include "World.h"
#include "Camera.h"

class UDPSocketConnection {
private:
    World& W_world;
    Camera& C_camera;
    sf::UdpSocket socket;

    std::map<short unsigned, Camera> m_cameras;
    std::string s_ipAdress;
    short unsigned i_myPort;
public:
    UDPSocketConnection(World& world, Camera& camera) : W_world(world), C_camera(camera) {}

    void bind(std::string ip, short unsigned port) {
        s_ipAdress = std::move(ip);
        socket.bind(port);
        socket.setBlocking(false);
        i_myPort = port;
    }

    void update() {
        double x;
        double y;
        int health;
        std::string killedName;

        sf::Packet packet;
        sf::IpAddress sender;
        short unsigned int senderPort = 0;
        short unsigned int port = 0;

        bool ack1 = false;
        bool ack2 = false;


        while((socket.receive(packet, sender, port) == sf::Socket::Status::Done) && (port >= 54000) && (port <= 54010)) {
            packet >> x >> y >> killedName >> ack2;
            senderPort = port;
        }

        if(ack2)
            C_camera.cleanLastKill();

        if(killedName == C_camera.getName()) {
            C_camera.setPosition({2.5, 0});
            C_camera.fullHealth();
            ack1 = true;
        }

        if(W_world.isExist(std::to_string(senderPort))) {
            m_cameras.at(senderPort).setPosition({x, y});
        } else if ((senderPort >= 54000) && (senderPort <= 54010)){
            Camera camera(W_world, {2.5, 0});
            camera.setName(std::to_string(senderPort));
            m_cameras.insert({senderPort, camera});
            W_world.addObject2D(m_cameras.at(senderPort), std::to_string(senderPort));
        }

        sf::Packet packetSend;
        packetSend << C_camera.x() << C_camera.y() << C_camera.lastKill() << ack1;

        for(int i = 54000; i < 54010; i++)
            if(i != i_myPort)
                socket.send(packetSend, s_ipAdress, i);
    }
};


#endif //PSEUDO3DENGINE_UDPSOCKETCONNECTION_H