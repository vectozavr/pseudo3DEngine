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
public:
    UDPSocketConnection(World& world, Camera& camera) : W_world(world), C_camera(camera) {}

    void bind(std::string ip, short unsigned port) {
        s_ipAdress = std::move(ip);
        socket.bind(port);
        socket.setBlocking(false);
    }

    void update() {
        double x;
        double y;
        int health;
        std::string killedName;

        sf::Packet packet;
        sf::IpAddress sender;
        short unsigned int port;

        bool ack1 = false;
        bool ack2;

        while(socket.receive(packet, sender, port) == sf::Socket::Status::Done)
            packet >> x >> y >> killedName >> ack2;

        if(ack2)
            C_camera.cleanLastKill();

        if(killedName == C_camera.getName()) {
            C_camera.setPosition({2.5, 0});
            C_camera.fullHealth();
            ack1 = true;
        }

        if(W_world.isExist(std::to_string(port))) {
            W_world[std::to_string(port)].setPosition({x, y});
        } else {
            Camera camera(W_world, {2.5, 0});
            camera.setName(std::to_string(port));
            m_cameras.insert({port, camera});
            W_world.addObject2D(m_cameras.at(port), std::to_string(port));
        }

        sf::Packet packetSend;
        packetSend << C_camera.x() << C_camera.y() << socket.getLocalPort() << C_camera.lastKill() << ack1;
        for(auto p : m_cameras)
            socket.send(packetSend, s_ipAdress, p.first);
    }
};


#endif //PSEUDO3DENGINE_UDPSOCKETCONNECTION_H