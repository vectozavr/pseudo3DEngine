//
// Created by Neirokan on 30.04.2020
//

#include "ServerUDP.h"
#include "Time.h"
#include "MsgType.h"
#include "settings.h"

ServerUDP::ServerUDP(World& world) : _world(world), _lastBroadcast(-INFINITY), _working(false)
{
    _socket.setTimeoutCallback(std::bind(&ServerUDP::timeout, this, std::placeholders::_1));
}

bool ServerUDP::isWorking() const
{
    return _working;
}

bool ServerUDP::start(sf::Uint16 port)
{
    return _working = _socket.bind(port);
}

void ServerUDP::update()
{
    if (!_working)
        return;

    while (process());

    // World state broadcast

    if (Time::time() - _lastBroadcast > 1 / WORLD_UPDATE_RATE)
    {
        sf::Packet updatePacket;
        updatePacket << MsgType::WorldUpdate;

        for (auto&& player : _players)
        {
            Player& camera = *player.second;
            camera.reduceHealth(-1 * (Time::time() - _lastBroadcast));
            updatePacket << player.first << camera.position().x << camera.position().y << camera.vPos() << camera.health();
        }

        for (auto&& player : _players)
        {
            _socket.send(updatePacket, player.first);
        }

        _lastBroadcast = Time::time();
    }

    // Socket update

    _socket.update();
}

void ServerUDP::stop()
{
    for (auto it = _players.begin(); it != _players.end();)
    {
        sf::Packet packet;
        packet << MsgType::Disconnect << it->first;
        _socket.send(packet, it->first);
        _players.erase(it++);
    }
    _socket.unbind();
    _working = false;
}

void ServerUDP::addSpawn(Point2D point)
{
    _spawns.push_back(point);
}

void ServerUDP::clearSpawns()
{
    _spawns.clear();
}

bool ServerUDP::timeout(sf::Uint16 playerId)
{
    sf::Packet packet;
    packet << MsgType::Disconnect << playerId;

    _players.erase(playerId);
    for (auto&& player : _players)
    {
        _socket.sendRely(packet, player.first);
    }

    return true;
}

// Recive and process message.
// Returns true, if some message was received.
bool ServerUDP::process()
{
    sf::Packet packet;
    sf::Uint16 senderId;
    MsgType type;

    if ((type = _socket.receive(packet, senderId)) == MsgType::None)
        return false;

    sf::Packet sendPacket;
    sf::Packet extraPacket;
    sf::Uint16 targetId;
    double buf[3];

    switch (type)
    {

    case MsgType::Connect:
        extraPacket << MsgType::Connect << NETWORK_VERSION << senderId;
        sendPacket << MsgType::WorldInit << senderId;
        _players.insert({ senderId, std::shared_ptr<Player>(new Player(_spawns[senderId % _spawns.size()])) });
        for (auto&& player : _players)
        {
            Player& camera = *player.second;
            sendPacket << player.first << camera.x() << camera.y() << camera.vPos() << camera.health();
            if (player.first != senderId)
                _socket.sendRely(extraPacket, player.first);
        }
        _socket.sendRely(sendPacket, senderId);

        break;

    case MsgType::Disconnect:
        sendPacket << MsgType::Disconnect << senderId;
        _players.erase(senderId);
        _socket.removeConnection(senderId);
        for (auto&& player : _players)
        {
            _socket.sendRely(sendPacket, player.first);
        }
        break;

    case MsgType::PlayerUpdate:
        packet >> buf[0] >> buf[1] >> buf[2];
        _players.at(senderId)->setPosition({ buf[0], buf[1] });
        _players.at(senderId)->setVPos(buf[2]);
        break;

    case MsgType::Shoot:
        packet >> targetId >> buf[0] >> buf[1];
        sendPacket << MsgType::Shoot;
        
        if (_players.at(targetId)->reduceHealth(buf[0] / buf[1]))
        {
            sendPacket << true << _spawns[targetId % _spawns.size()].x << _spawns[targetId % _spawns.size()].y;
            _players.at(targetId)->setHealth(100);
            _players.at(targetId)->setPosition(_spawns[targetId % _spawns.size()]);
        }
        else
        {
            double dir = 2 * PI * rand() / RAND_MAX;
            sendPacket << false << 0.05 * cos(dir) << 0.05 * sin(dir);
        }
        _socket.sendRely(sendPacket, targetId);
        break;
    }
    return true;
}
