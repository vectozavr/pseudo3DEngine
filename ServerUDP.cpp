//
// Created by Neirokan on 30.04.2020
//

#include <fstream>
#include <sstream>
#include "ServerUDP.h"
#include "Time.h"
#include "MsgType.h"
#include "settings.h"
#include "Point2D.h"
#include "Point3D.h"

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

            updatePacket << player.first << camera.position().x << camera.position().y << camera.vPos() << camera.health() << camera.kills() << camera.deaths();
        }

        for (auto&& player : _players)
        {
            _socket.send(updatePacket, player.first);
            if(b_newGame && player.first != 1) {
                sf::Packet packet;

                packet << MsgType::ReInit << _spawns[(player.first) % _spawns.size()].x << _spawns[(player.first) % _spawns.size()].y;
                _socket.send(packet, player.first);
                player.second->setHealth(100);
                player.second->setPosition(_spawns[(player.first) % _spawns.size()]);
                player.second->setKills(0);
                player.second->setDeaths(0);
            }
        }
        b_newGame = false;
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

void ServerUDP::reInit() {
    b_newGame = true;
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
        //extraPacket << MsgType::Connect << NETWORK_VERSION << senderId;
        extraPacket << MsgType::NewPlayer << senderId;
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

        if(_players[targetId] == nullptr)
            return false;

        if (_players.at(targetId)->reduceHealth(buf[0] / buf[1]))
        {
            sendPacket << true << _spawns[targetId % _spawns.size()].x << _spawns[targetId % _spawns.size()].y;
            _players.at(targetId)->setHealth(100);
            _players.at(targetId)->setPosition(_spawns[targetId % _spawns.size()]);

            _players.at(targetId)->oneMoreDeath();
            _players.at(senderId)->oneMoreKill();
        }
        else
        {
            double dir = 2 * PI * rand() / RAND_MAX;
            sendPacket << false << 0.05 * cos(dir) << 0.05 * sin(dir);
        }
        _socket.sendRely(sendPacket, targetId);
        break;

    case MsgType::ReInit:
        for(auto& p : _players) {
            p.second->setKills(0);
            p.second->setDeaths(0);
            p.second->setHealth(100);
        }
    }

    return true;
}

struct Point4D {
    double x = 0;
    double y = 0;
    double z = 0;
    double w = 0;
};

bool ServerUDP::loadObjSpawns(const std::string& filename, double scale) {
    std::vector<Point3D> vertices;
    std::vector<Point4D> indices;

    std::ifstream file(filename);

    int numb = 0;

    while (file) {
        std::string str;
        std::getline(file, str);
        if (str == "" || str == "\r" || str == "\n")
            continue;

        std::string type = str.substr(0, 2);
        str = str.substr(2, str.size());

        if (type == "v ") {
            std::stringstream str_stream(str);
            float x; float y; float z;
            str_stream >> x >> y >> z;

            vertices.push_back({x, y, z});
        }
        /*
        # f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 v4/vt4/vn4
        # v1 - индекс из списка вершин
        # vt1 - индекс из списка текс.координат
        # vn1 - индекс из списка нормалей
        */

        if (type == "f ") {
            int i1 = 0; int i2 = 0; int i3 = 0; int i4 = 0;

            const char* buffer = str.c_str();

            sscanf(buffer, "%d %d %d %d", &i1, &i2, &i3, &i4);

            indices.push_back({static_cast<double>(i1-1), static_cast<double>(i2-1), static_cast<double>(i3-1), static_cast<double>(i4-1)});
        }
    }

    for (size_t i = 0; i < indices.size(); i++) {
        Point2D spawn = {vertices[indices[i].x].x * scale, vertices[indices[i].y].z * scale};
        addSpawn(spawn);
    }

    return !indices.empty();
}