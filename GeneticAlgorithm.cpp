//
// Created by Иван Ильин on 25.07.2020.
//

#include "GeneticAlgorithm.h"
#include "Time.h"
#include <thread>
#include <iostream>
#include <fstream>
#include "Point3D.h"
#include "Point2D.h"
#include <sstream>
#include <thread>

GeneticAlgorithm::GeneticAlgorithm(World& world, ServerUDP& server, int number) : W_world(world), serverUdp(server) {
    for(int i = 0; i < number; i++) {
        v_generation.insert({i, std::shared_ptr<Enemy>(new Enemy(world, {0, 0}))});
        //v_generation[i]->network().randomlyInitNetwork();
    }

    //_spawns.push_back({ 1.5, 1.5 });
    //_spawns.push_back({ 15.5, 2.5 });
    //_spawns.push_back({ 27.5, -12.5 });
    //_spawns.push_back({ 21.5, -15.0 });
    //_spawns.push_back({ 23.0, 27.0 });
    //_spawns.push_back({9.5, 26.0});
    //_spawns.push_back({0.6, 8.5});
    //_spawns.push_back({15, 8});
    //_spawns.push_back({20, -5});
//////
    //_spawns.push_back({22, 19});
    //_spawns.push_back({13, 20});
    //_spawns.push_back({23, 14});
    //_spawns.push_back({28, 8});
    //_spawns.push_back({20, 20});
    //_spawns.push_back({23, 3});
    //_spawns.push_back({15, 6});
    //_spawns.push_back({20, 14});
    //_spawns.push_back({12, 19});
//
//
//
//
    //_spawns.push_back({0.5, -0.5});
    //_spawns.push_back({12.5, -12.5});
    //_spawns.push_back({30, 5});
    //_spawns.push_back({6, 26});
////
    //_spawns.push_back({9, -2});
    //_spawns.push_back({26, -9});
    //_spawns.push_back({25, 11});
    //_spawns.push_back({24, 27});
}

void GeneticAlgorithm::connect(ClientUDP& camera) {

    for(int i = 0; i < v_generation.size(); i++) {
        v_generation[i]->connect("127.0.0.1", 54000);

        while (v_generation[i]->client->isWorking() && !v_generation[i]->client->connected())
        {
            serverUdp.update();

            for(auto& p : v_generation)
                if(p.second->client != nullptr)
                    p.second->client->update();

            camera.update();

            Time::update();
        }
        // If connect fail - return to menu
        if (!v_generation[i]->client->isWorking())
        {
            throw "ENEMY CONNECTION ERROR";
        }

        if(v_generation[i]->client->localPlayer() != nullptr) {
            v_generation[i]->client->localPlayer()->client = v_generation[i]->client;
            v_generation[i]->setPosition(v_generation[i]->client->localPlayer()->position());
            v_generation[i]->client->localPlayer()->setFieldOfView(v_generation[i]->fov());
            v_generation[i]->setName(v_generation[i]->client->localPlayer()->getName());
        }
    }

    setInitStats();
}

void GeneticAlgorithm::updateFormTo(int from, int to, double dt, double elapsedTime) {
    for(int i = from; i < to; i++) {
        v_generation[i]->update(dt, elapsedTime);
    }
}

void GeneticAlgorithm::update(double dt, double elapsedTime) {

    if(m) {
        std::vector<std::shared_ptr<std::thread>> threads;
        if(v_generation.size() <= std::thread::hardware_concurrency()) {
            for(auto& player : v_generation)
                threads.emplace_back(new std::thread(&Enemy::update, player.second.get(), dt, elapsedTime));

        } else {
            int onOneThread = 1 + v_generation.size() / std::thread::hardware_concurrency();
            for(int i = 0; i < std::thread::hardware_concurrency(); i++) {
                int from = i*onOneThread;
                int to = (i+1)*onOneThread;

                if(i == std::thread::hardware_concurrency() - 1 || to >= v_generation.size()) {
                    to = v_generation.size();
                    i = std::thread::hardware_concurrency() - 1;
                }

                threads.emplace_back(new std::thread(&GeneticAlgorithm::updateFormTo, this, from, to, dt, elapsedTime));
            }
        }
        for(auto& t : threads)
            if(t->joinable())
                t->join();
    } else {
        for(auto& player : v_generation)
            player.second->update(dt, elapsedTime);
    }

    // ========================= //

    for(auto& p : v_generation) {
        if(p.second->client->localPlayer() == nullptr)
            return;

        if(p.second->client->localPlayer()->deaths() != p.second->deaths()) {
            if (b_oneDeathMode) {
                p.second->client->localPlayer()->setPosition({2000, 0});
            } else {
                p.second->setKills(p.second->client->localPlayer()->kills());
                p.second->setDeaths(p.second->client->localPlayer()->deaths());
            }
        }
    }
}

void GeneticAlgorithm::newGeneration() {

    // select AI's with the highest score = kills - deaths
    std::vector<std::pair<double, NeuralNetwork>> theBest;
    for(auto& p : v_generation)
        //theBest.emplace_back(p.second->kills(), std::move(p.second->network()));
        theBest.emplace_back(p.second->kills() * 100 - 30 * p.second->deaths() + p.second->reduced() - 0.3 * p.second->lost() + 0.1 * p.second->pathLength(), std::move(p.second->network()));
    std::sort(theBest.begin(), theBest.end(), [](const std::pair<int, NeuralNetwork>& lh, const std::pair<int, NeuralNetwork>& rh) { return lh.first > rh.first; });

    d_score = theBest[0].first;

    // crossbreed the best networks and share to all AI's
    for(auto& p : v_generation) {
        int n1 = rand() % 6;
        int n2 = rand() % 6;
        p.second->network().crossbreeding(theBest[n1].second, theBest[n2].second, theBest[n1].first, theBest[n2].first);
    }

    // make mutations of all network (10% of all weight and bias we change on +- 10%)
    mutate();
    i_generation++;
    setInitStats();
}

void GeneticAlgorithm::mutate() {
    for(auto& p : v_generation)
        p.second->network().mutateNetwork();
}

void GeneticAlgorithm::saveNetwork(std::string filename) {
    int maxKillsPlayer = 0;

    // select AI with the highest score = kills - deaths
    for(auto& p : v_generation)
        if (p.second->kills() > v_generation[maxKillsPlayer]->kills())
            maxKillsPlayer = p.first;

    // save the network of this AI
    v_generation[maxKillsPlayer]->network().saveNetwork(filename, i_generation);
}

void GeneticAlgorithm::loadNetwork(std::string filename) {
    // load the network from 'filename'

    i_generation = v_generation[0]->network().loadNetwork(filename);

    for(auto& p : v_generation)
        p.second->network().sameNet(v_generation[0]->network());
}

void GeneticAlgorithm::setInitStats() {
    for(auto& p : v_generation) {
        if(p.second->client->isWorking()) {
            p.second->setDeaths(0);
            p.second->setKills(0);
            p.second->setHeight(100);
            //p.second->setPosition(serverUdp.spawns()[p.first % serverUdp.spawns().size()]);
            //p.second->client->localPlayer()->setPosition(p.second->position());
            p.second->fullAmmunition();
            p.second->setInitStats();
            p.second->client->localPlayer()->setKills(0);
            p.second->client->localPlayer()->setDeaths(0);
        }
    }

    serverUdp.reInit();
}

void GeneticAlgorithm::logScore(std::string filename) {
    std::ofstream file;
    file.open(filename, std::ios::app);
    if(!file.is_open())
        return;

    // the number of current generation
    file << i_generation << '\t' << d_score << std::endl;

    file.close();
}

struct Point4D {
    double x = 0;
    double y = 0;
    double z = 0;
    double w = 0;
};