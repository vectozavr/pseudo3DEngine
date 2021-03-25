//
// Created by Иван Ильин on 25.07.2020.
//

#ifndef FPSONLINE_GENETICALGORITHM_H
#define FPSONLINE_GENETICALGORITHM_H

#include "World.h"
#include "Enemy.h"


class GeneticAlgorithm {
private:
    World& W_world;
    int i_generation = 0;
    std::map<const int, std::shared_ptr<Enemy>> v_generation;
    //std::vector<Point2D> _spawns;
    ServerUDP& serverUdp;

    bool m = true;
    bool b_oneDeathMode = false;

    double d_score = 0;

public:
    GeneticAlgorithm(World& world, ServerUDP& server, int number);

    void update         (double dt, double elapsedTime);
    void updateFormTo   (int from, int to, double dt, double elapsedTime);

    void connect(ClientUDP& camera);

    void newGeneration();
    void mutate();

    void saveNetwork(std::string filename);
    void loadNetwork(std::string filename);

    [[nodiscard]] int generation() const { return i_generation; }

    void setInitStats();

    Camera* connectToEnemyCamera() {
        if(!v_generation.empty() && v_generation[0] != nullptr) {
            return v_generation[0]->client->localPlayer();
            //camera->client = v_generation[0]->client;
        } else
            return nullptr;
    }

    void logScore(std::string filename);

    [[nodiscard]] double maxScore() const { return d_score; }
};


#endif //FPSONLINE_GENETICALGORITHM_H