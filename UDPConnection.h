//
// Created by Neirokan on 30.04.2020
//

#ifndef PSEUDO3DENGINE_UDPCONNECTION_H
#define PSEUDO3DENGINE_UDPCONNECTION_H

#include <SFML/Network.hpp>

class UDPConnection
{
private:
    sf::Uint16 _id;
    sf::IpAddress _ip;
    sf::Uint16 _port;
    double lastMsg;

public:
    explicit UDPConnection(sf::Uint16 id, sf::IpAddress ip, sf::Uint16 port);
    sf::Uint16 id() const;
    const sf::IpAddress& ip() const;
    sf::Uint16 port() const;
    bool timeout() const;
    bool same(sf::IpAddress& ip, sf::Uint16 port) const;
    void update();
    void send(sf::UdpSocket& socket, sf::Packet& packet);
};


#endif //PSEUDO3DENGINE_UDPCONNECTION_H
