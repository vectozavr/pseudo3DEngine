//
// Created by Neirokan on 30.04.2020
//

#include "UDPConnection.h"
#include "Time.h"
#include "settings.h"

UDPConnection::UDPConnection(sf::Uint16 id, sf::IpAddress ip, sf::Uint16 port) : _id(id), _ip(ip), _port(port), lastMsg(Time::time()) {}

sf::Uint16 UDPConnection::id() const
{
    return _id;
}

const sf::IpAddress& UDPConnection::ip() const
{
    return _ip;
}

sf::Uint16 UDPConnection::port() const
{
    return _port;
}

bool UDPConnection::timeout() const
{
    return Time::time() - lastMsg > TIMEOUT_SECONDS;
}

bool UDPConnection::same(sf::IpAddress& ip, sf::Uint16 port) const
{
    return _ip == ip && _port == port;
}

void UDPConnection::update()
{
    lastMsg = Time::time();
}

void UDPConnection::send(sf::UdpSocket& socket, sf::Packet& packet)
{
    socket.send(packet, _ip, _port);
}
