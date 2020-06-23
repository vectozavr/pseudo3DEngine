//
// Created by Neirokan on 30.04.2020
//

#ifndef PSEUDO3DENGINE_RELIABLEMSG_H
#define PSEUDO3DENGINE_RELIABLEMSG_H

#include <SFML/Network.hpp>

class ReliableMsg
{
private:
    sf::Packet packet;
    sf::IpAddress address;
    sf::Uint16 port;
    double lastTry;
    double firstTry;

public:
    ReliableMsg(sf::Packet& packet, sf::IpAddress address, sf::Uint16 port);
    ReliableMsg(const ReliableMsg& msg);

    bool trySend(sf::UdpSocket& socket);
};


#endif //PSEUDO3DENGINE_RELIABLEMSG_H
