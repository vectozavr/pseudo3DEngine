//
// Created by Neirokan on 30.04.2020
//

#include "ClientUDP.h"
#include "Time.h"
#include "MsgType.h"
#include "settings.h"
#include <thread>

ClientUDP::ClientUDP(World& world) : _world(world), _lastBroadcast(-INFINITY), _working(false), _localPlayer(nullptr)
{
    _socket.setTimeoutCallback(std::bind(&ClientUDP::timeout, this, std::placeholders::_1));
}

bool ClientUDP::connected() const
{
    return _socket.ownId();
}

bool ClientUDP::isWorking() const
{
    return _working;
}

Camera* ClientUDP::localPlayer()
{
    return _localPlayer;
}

void ClientUDP::shoot(const std::string& name, double damage, double distance)
{
    sf::Packet packet;
    for (auto&& player : _players)
    {
        if (player.second->getName() == name)
        {
            packet << MsgType::Shoot << player.first << damage << distance;
            _socket.sendRely(packet, _socket.serverId());
            break;
        }
    }
}

void ClientUDP::connect(sf::IpAddress ip, sf::Uint16 port)
{
    sf::Packet packet;
    packet << MsgType::Connect << NETWORK_VERSION;
    _socket.bind(0);
    _working = true;
    _socket.addConnection(_socket.serverId(), ip, port);
    _socket.sendRely(packet, _socket.serverId());
}

void ClientUDP::update()
{
    while (_working && process());

    if (!_working)
        return;

    // World state broadcast

    if (Time::time() - _lastBroadcast > 1 / WORLD_UPDATE_RATE && connected())
    {
        sf::Packet updatePacket;
        updatePacket << MsgType::PlayerUpdate << _localPlayer->x() << _localPlayer->y() << _localPlayer->vPos();
        _socket.send(updatePacket, _socket.serverId());
        _lastBroadcast = Time::time();
    }

    // Socket update

    _socket.update();
}

void ClientUDP::disconnect()
{
    for (auto it = _players.begin(); it != _players.end();)
    {
        _world.removeObject2D(it->second->getName());
        _players.erase(it++);
    }
    _localPlayer = nullptr;
    sf::Packet packet;
    packet << MsgType::Disconnect << _socket.ownId();
    _socket.send(packet, _socket.serverId());
    _socket.unbind();
    _working = false;
}

bool ClientUDP::timeout(sf::Uint16 id)
{
    if (id != _socket.serverId())
        return true;
    disconnect();
    return false;
}


// Recive and process message.
// Returns true, if some message was received.
bool ClientUDP::process()
{
    sf::Packet packet;
    sf::Uint16 senderId;
    MsgType type;

    if ((type = _socket.receive(packet, senderId)) == MsgType::None)
        return false;
    if (!connected() && type != MsgType::WorldInit)
        return true;

    sf::Packet sendPacket;
    sf::Packet extraPacket;
    sf::Uint16 targetId;
    bool revive;
    double buf[4];
    Player* player;

    switch (type)
    {

    case MsgType::Connect:
        packet >> targetId;
        player = new Player({ 2.5, 0 });
        _players.insert({ targetId, std::shared_ptr<Player>(player) });
        _world.addObject2D(*_players.at(targetId), "Player" + std::to_string(targetId));
        _localPlayer->addPlayer(_players.at(targetId)->getName(), _players.at(targetId));
        break;

    case MsgType::Disconnect:
        packet >> targetId;
        if (targetId != _socket.ownId() && _players.count(targetId))
        {
            _world.removeObject2D(_players.at(targetId)->getName());
            _localPlayer->removePlayer(_players.at(targetId)->getName());
            _players.erase(targetId);
        }
        else if (targetId == _socket.ownId())
        {
            disconnect();
        }
        break;

    case MsgType::WorldInit:
        packet >> targetId;
        _socket.setId(targetId);
        while (packet >> targetId >> buf[0] >> buf[1] >> buf[2] >> buf[3])
        {
            if (targetId == _socket.ownId())
            {
                _localPlayer = new Camera(_world, { 2.5, 0 });
                player = _localPlayer;
            }
            else
            {
                player = new Player({ 2.5, 0 });
            }
            _players.insert({ targetId, std::shared_ptr<Player>(player) });
            _world.addObject2D(*player, "Player" + std::to_string(targetId));
            player->setPosition({ buf[0], buf[1] });
            player->setVPos(buf[2]);
            player->setHealth(buf[3]);
        }
        for (auto&& player : _players)
        {
            if (player.first != _socket.ownId())
            {
                _localPlayer->addPlayer(player.second->getName(), player.second);
            }
        }
        break;

    case MsgType::WorldUpdate:
        while (packet >> targetId >> buf[0] >> buf[1] >> buf[2] >> buf[3])
        {
            if (_players.count(targetId))
            {
                player = _players.at(targetId).get();
                if (targetId != _socket.ownId())
                {
                    player->setPosition({ buf[0], buf[1] });
                    player->setVPos(buf[2]);
                }
                player->setHealth(buf[3]);
            }
        }
        break;

    case MsgType::Shoot:
        packet >> revive >> buf[0] >> buf[1];
        if (revive)
            _localPlayer->setPosition({ buf[0], buf[1] });
        else
            _localPlayer->shiftPrecise({ buf[0], buf[1] });
        break;
    }
    return true;
}
