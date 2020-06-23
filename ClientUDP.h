//
// Created by Neirokan on 30.04.2020
//

#ifndef PSEUDO3DENGINE_CLIENTUDP_H
#define PSEUDO3DENGINE_CLIENTUDP_H

#include "World.h"
#include "Camera.h"
#include "ReliableMsg.h"
#include "UDPSocket.h"
#include <memory>

class ClientUDP
{
private:
    World& _world;
    Camera* _localPlayer;
    UDPSocket _socket;
    double _lastBroadcast;
    bool _working;

    std::map<sf::Uint16, std::shared_ptr<Player>> _players;

    bool process();
    bool timeout(sf::Uint16 id);

public:
    ClientUDP(World& world);
    bool isWorking() const;
    bool connected() const;
    Camera* localPlayer();
    void connect(sf::IpAddress ip, sf::Uint16 port);
    void disconnect();
    void update();
    void shoot(const std::string& name, double damage, double distance);
};


#endif //PSEUDO3DENGINE_CLIENTUDP_H
