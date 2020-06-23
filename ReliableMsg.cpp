//
// Created by Neirokan on 30.04.2020
//

#include <cmath>
#include "ReliableMsg.h"
#include "Time.h"
#include "settings.h"
ReliableMsg::ReliableMsg(sf::Packet& packet, sf::IpAddress address, sf::Uint16 port) : packet(packet), address(address), port(port), lastTry(-INFINITY), firstTry(Time::time()) {}
ReliableMsg::ReliableMsg(const ReliableMsg& msg) : packet(msg.packet), address(msg.address), port(msg.port), lastTry(msg.lastTry), firstTry(msg.firstTry) {}

bool ReliableMsg::trySend(sf::UdpSocket& socket)
{
    if (Time::time() - firstTry > TIMEOUT_SECONDS)
        return false;
    if (Time::time() - lastTry > RELIABLE_RETRY_TIME)
    {
        lastTry = Time::time();
        socket.send(packet, address, port);
    }
    return true;
}
