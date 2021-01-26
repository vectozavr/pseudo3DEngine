//
// Created by Иван Ильин on 27.07.2020.
//

#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include "NeuralNetwork.h"

using namespace std;

NeuralNetwork::NeuralNetwork() { b_valid = false; }

NeuralNetwork::NeuralNetwork(const NeuralNetwork& network) {
    b_valid = false;
    if(!network.b_valid)
        return;

    v_inLayer = network.v_inLayer;
    v_hiddenLayer_1 = network.v_hiddenLayer_1;
    v_hiddenLayer_2 = network.v_hiddenLayer_2;
    v_outLayer = network.v_outLayer;

    v_weight_inLayer = network.v_weight_inLayer;
    v_weight_hiddenLayer_1 = network.v_weight_hiddenLayer_1;
    v_weight_hiddenLayer_2 = network.v_weight_hiddenLayer_2;

    v_bias_inLayer = network.v_bias_inLayer;
    v_bias_hiddenLayer_1 = network.v_bias_hiddenLayer_1;
    v_bias_hiddenLayer_2 = network.v_bias_hiddenLayer_2;

    b_valid = true;

    d_check = check();
}

void NeuralNetwork::randomlyInitNetwork() {
    for(int i = 0; i < DIS_SEG_FOR_BOTS + 8; i++) {
        v_bias_inLayer.emplace_back((double)rand()/RAND_MAX);
        v_bias_hiddenLayer_1.emplace_back((double)rand()/RAND_MAX);

        v_weight_inLayer.push_back({});
        v_weight_hiddenLayer_1.push_back({});
        for(int j = 0; j < DIS_SEG_FOR_BOTS + 8; j++) {
            v_weight_inLayer[i].emplace_back((double)rand()/RAND_MAX);
            v_weight_hiddenLayer_1[i].emplace_back((double)rand()/RAND_MAX);
        }
    }

    //for(int i = 0; i < 4; i++) {
    //    v_bias_hiddenLayer_2.emplace_back((double)rand()/RAND_MAX);

    //    v_weight_hiddenLayer_2.push_back({});
    //    for(int j = 0; j < DIS_SEG_FOR_BOTS + 8; j++) {
    //        v_weight_hiddenLayer_2[i].emplace_back((double)rand()/RAND_MAX);
    //    }
    //}
    b_valid = true;

    d_check = check();
}

void NeuralNetwork::mutateNetwork() {
    if(!b_valid)
        return;

    double amp = 1;
    double prop = 0.2;

    double range_random = 100;
    double prob_random_range = 0.1;

    // ----- 3 WEIGHT -----
    for(auto& w1 : v_weight_inLayer) {
        for(auto& w11 : w1) {
            double p = 1.0 * rand() / RAND_MAX;
            if(p <= prop) { // with probability of prop * 100% we change the weight
                double ch = 2 * amp * (0.5 - 1.0 * rand() / RAND_MAX);
                w11 += ch;
            }
            if(p <= prob_random_range) {
                double newValue = 2 * range_random * (0.5 - 1.0 * rand() / RAND_MAX);
                w11 = newValue;
            }
        }
    }
    for(auto& w2 : v_weight_hiddenLayer_1) {
        for(auto& w22 : w2) {
            double p = 1.0 * rand() / RAND_MAX;
            if(p <= prop) { // with probability of prop * 100% we change the weight
                double ch = 2 * amp * (0.5 - 1.0 * rand() / RAND_MAX);
                w22 += ch;
            }
            if(p <= prob_random_range) {
                double newValue = 2 * range_random * (0.5 - 1.0 * rand() / RAND_MAX);
                w22 = newValue;
            }
        }
    }

    for(auto& w3 : v_weight_hiddenLayer_2) {
        for(auto& w33 : w3) {
            double p = 1.0 * rand() / RAND_MAX;
            if(p <= prop) { // with probability of prop * 100% we change the weight
                double ch = 2 * amp * (0.5 - 1.0 * rand() / RAND_MAX);
                w33 += ch;
            }
            if(p <= prob_random_range) {
                double newValue = 2 * range_random * (0.5 - 1.0 * rand() / RAND_MAX);
                w33 = newValue;
            }
        }
    }

    // ----- 3 BIAS -----
    for(auto& b1 : v_bias_inLayer) {
        double p = 1.0 * rand() / RAND_MAX;
        if(p <= prop) { // with probability of prop * 100% we change the bias
            double ch = 2 * amp * (0.5 - 1.0 * rand() / RAND_MAX);
            b1 += ch;
        }
        if(p <= prob_random_range) {
            double newValue = 2 * range_random * (0.5 - 1.0 * rand() / RAND_MAX);
            b1 = newValue;
        }
    }
    for(auto& b2 : v_bias_hiddenLayer_1) {
        double p = 1.0 * rand() / RAND_MAX;
        if(p <= prop) { // with probability of prop * 100% we change the bias
            double ch = 2 * amp * (0.5 - 1.0 * rand() / RAND_MAX);
            b2 += ch;
        }
        if(p <= prob_random_range) {
            double newValue = 2 * range_random * (0.5 - 1.0 * rand() / RAND_MAX);
            b2 = newValue;
        }
    }

    for(auto& b3 : v_bias_hiddenLayer_2) {
        double p = 1.0 * rand() / RAND_MAX;
        if(p <= prop) { // with probability of prop * 100% we change the bias
            double ch = amp * (0.5 - 1.0 * rand() / RAND_MAX);
            b3 += ch;
        }
        if(p <= prob_random_range) {
            double newValue = 2 * range_random * (0.5 - 1.0 * rand() / RAND_MAX);
            b3 = newValue;
        }
    }

    d_check = check();
}

void NeuralNetwork::saveNetwork(std::string filename, int gen) {
    if(!b_valid)
        return;

    ofstream file;
    file.open(filename);
    if(!file.is_open())
        return;

    // the number of current generation
    file << gen << endl;

    // ----- 3 WEIGHT -----
    for(auto& w1 : v_weight_inLayer) {
        for(auto& w11 : w1) {
            file << w11 << " ";
        }
        file << endl;
    }
    file << "-" << endl;

    for(auto& w2 : v_weight_hiddenLayer_1) {
        for(auto& w22 : w2) {
            file << w22 << " ";
        }
        file << endl;
    }
    file << "-" << endl;

    for(auto& w3 : v_weight_hiddenLayer_2) {
        for(auto& w33 : w3) {
            file << w33 << " ";
        }
        file << endl;
    }
    file << "-" << endl;

    // ----- 3 BIAS -----
    for(auto& b1 : v_bias_inLayer) {
        file << b1 << " ";
    }
    file << endl << "-" << endl;

    for(auto& b2 : v_bias_hiddenLayer_1) {
        file << b2 << " ";
    }
    file << endl << "-" << endl;

    for(auto& b3 : v_bias_hiddenLayer_2) {
        file << b3 << " ";
    }

    file.close();
}

int NeuralNetwork::loadNetwork(const std::string& filename) {
    b_valid = false;

    v_weight_inLayer.clear();
    v_weight_hiddenLayer_1.clear();
    v_weight_hiddenLayer_2.clear();

    v_bias_inLayer.clear();
    v_bias_hiddenLayer_1.clear();
    v_bias_hiddenLayer_2.clear();

    ifstream file;
    file.open(filename);
    if(!file.is_open())
        return -1;

    // the number of current generation
    int gen;
    file >> gen;

    string line = "s";
    double var;

    // ----- 3 WEIGHT -----
    getline(file, line);
    line = "s";
    while(true) {
        getline(file, line);
        if(line == "-" || line.empty())
            break;
        v_weight_inLayer.emplace_back();
        stringstream strStream(line);
        while(!strStream.eof()) {
            strStream >> var;
            if(!strStream.eof())
                v_weight_inLayer.back().push_back(var);
        }
    }
    line = "s";

    while(true) {
        getline(file, line);
        if(line == "-" || line.empty())
            break;
        v_weight_hiddenLayer_1.emplace_back();
        stringstream strStream(line);
        while(!strStream.eof()) {
            strStream >> var;
            if(!strStream.eof())
                v_weight_hiddenLayer_1.back().push_back(var);
        }
    }
    line = "s";

    while(1) {
        getline(file, line);
        if(line == "-" || line.empty())
            break;
        v_weight_hiddenLayer_2.emplace_back();
        stringstream strStream(line);
        while(!strStream.eof()) {
            strStream >> var;
            if(!strStream.eof())
                v_weight_hiddenLayer_2.back().push_back(var);
        }
    }
    line = "s";

    // ----- 3 BIAS -----
    getline(file, line);
    if(line == "-" || line.empty())
        return -1;
    stringstream strStream1(line);
    while(!strStream1.eof()) {
        strStream1 >> var;
        if(!strStream1.eof())
            v_bias_inLayer.push_back(var);
    }

    line = "s";
    getline(file, line);
    getline(file, line);
    if(line == "-" || line.empty())
        return -1;
    stringstream strStream2(line);
    while(!strStream2.eof()) {
        strStream2 >> var;
        if(!strStream2.eof())
            v_bias_hiddenLayer_1.push_back(var);
    }

    line = "s";
    getline(file, line);
    getline(file, line);
    if(line == "-" || line.empty())
        return -1;
    stringstream strStream3(line);
    while(!strStream3.eof()) {
        strStream3 >> var;
        if(!strStream3.eof())
            v_bias_hiddenLayer_2.push_back(var);
    }

    file.close();
    b_valid = true;

    d_check = check();

    return gen;
}

void NeuralNetwork::clearInput() {
    v_inLayer.clear();
}

std::vector<double> NeuralNetwork::getOutput() {
    if(!b_valid)
        return std::vector<double>(0);

    // Old system
    // ------ 1'TH HIDDEN LAYER ------
    for(int i = 0; i < v_bias_inLayer.size(); i++) {
        v_hiddenLayer_1.emplace_back(v_bias_inLayer[i]);
        for (int j = 0; j < v_inLayer.size(); j++)
            v_hiddenLayer_1[i] += v_weight_inLayer[i][j] * v_inLayer[j];
        v_hiddenLayer_1[i] = sigmoid(v_hiddenLayer_1[i]);
    }

    // ------ 2'TH HIDDEN LAYER ------
    for(int i = 0; i < v_bias_hiddenLayer_1.size(); i++) {
        v_hiddenLayer_2.emplace_back(v_bias_hiddenLayer_1[i]);
        for (int j = 0; j < v_hiddenLayer_1.size(); j++)
            v_hiddenLayer_2[i] += v_weight_hiddenLayer_1[i][j] * v_hiddenLayer_1[j];
        v_hiddenLayer_2[i] = sigmoid(v_hiddenLayer_2[i]);
    }


    // ---------- OUT LAYER ----------
    for(int i = 0; i < v_bias_hiddenLayer_2.size(); i++) {
        v_outLayer.emplace_back(v_bias_hiddenLayer_2[i]);
        for (int j = 0; j < v_hiddenLayer_2.size(); j++)
            v_outLayer[i] += v_weight_hiddenLayer_2[i][j] * v_hiddenLayer_2[j];
        v_outLayer[i] = sigmoid(v_outLayer[i]);
    }

    // Multi Neural System

    // ------ 1'TH HIDDEN LAYER ------

    // motion neural network
    //for(int i = 0; i < 5; i++) {
    //    v_hiddenLayer_1.emplace_back(v_bias_inLayer[i]);
    //    for (int j = 0; j < v_weight_inLayer[i].size(); j++)
    //        v_hiddenLayer_1[i] += v_weight_inLayer[i][j] * v_inLayer[j];
    //    v_hiddenLayer_1[i] = sigmoid(v_hiddenLayer_1[i]);
    //}
    //// fire neural network
    //for(int i = 5; i < 7; i++) {
    //    v_hiddenLayer_1.emplace_back(v_bias_inLayer[i]);
    //    for (int j = 0; j < v_weight_inLayer[i].size(); j++)
    //        v_hiddenLayer_1[i] += v_weight_inLayer[i][j] * v_inLayer[j + 8];
    //    v_hiddenLayer_1[i] = sigmoid(v_hiddenLayer_1[i]);
    //}

    // ------ 2'TH HIDDEN LAYER ------
//
    //// motion neural network
    //for(int i = 0; i < 5; i++) {
    //    v_hiddenLayer_2.emplace_back(v_bias_hiddenLayer_1[i]);
    //    for (int j = 0; j < 5; j++)
    //        v_hiddenLayer_2[i] += v_weight_hiddenLayer_1[i][j] * v_hiddenLayer_1[j];
    //    v_hiddenLayer_2[i] = sigmoid(v_hiddenLayer_2[i]);
    //}
    //// fire neural network
    //for(int i = 5; i < 7; i++) {
    //    v_hiddenLayer_2.emplace_back(v_bias_hiddenLayer_1[i]);
    //    for (int j = 5; j < 7; j++)
    //        v_hiddenLayer_2[i] += v_weight_hiddenLayer_1[i][j] * v_hiddenLayer_1[j];
    //    v_hiddenLayer_2[i] = sigmoid(v_hiddenLayer_2[i]);
    //}
//
    // ---------- OUT LAYER ----------

    // motion neural network
    //for(int i = 0; i < 2; i++) {
    //    v_outLayer.emplace_back(v_bias_hiddenLayer_1[i]);
    //    for (int j = 0; j < v_weight_hiddenLayer_1[i].size(); j++)
    //        v_outLayer[i] += v_weight_hiddenLayer_1[i][j] * v_hiddenLayer_1[j];
    //    v_outLayer[i] = sigmoid(v_outLayer[i]);
    //}
    //// fire neural network
    //for(int i = 2; i < 3; i++) {
    //    v_outLayer.emplace_back(v_bias_hiddenLayer_1[i]);
    //    for (int j = 0; j < v_weight_hiddenLayer_1[i].size(); j++)
    //        v_outLayer[i] += v_weight_hiddenLayer_1[i][j] * v_hiddenLayer_1[j + 5];
    //    v_outLayer[i] = sigmoid(v_outLayer[i]);
    //}

    // cleaning the network
    v_hiddenLayer_1.clear();
    v_hiddenLayer_2.clear();
    return std::move(v_outLayer);
}

double NeuralNetwork::sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

double NeuralNetwork::relu(double x) {
    return x > 0 ? x : 0;
}

[[nodiscard]] bool NeuralNetwork::equalSize(const NeuralNetwork& network) const {
    if(v_weight_inLayer.size() != network.v_weight_inLayer.size())
        return false;
    if(v_weight_hiddenLayer_1.size() != network.v_weight_hiddenLayer_1.size())
        return false;
    if(v_weight_hiddenLayer_2.size() != network.v_weight_hiddenLayer_2.size())
        return false;
    for(int i = 0; i < v_weight_inLayer.size(); i++)
        if(v_weight_inLayer[i].size() != network.v_weight_inLayer[i].size())
            return false;
    for(int i = 0; i < v_weight_hiddenLayer_1.size(); i++)
        if(v_weight_hiddenLayer_1[i].size() != network.v_weight_hiddenLayer_1[i].size())
            return false;
    for(int i = 0; i < v_weight_hiddenLayer_2.size(); i++)
        if(v_weight_hiddenLayer_2[i].size() != network.v_weight_hiddenLayer_2[i].size())
            return false;
    if(v_bias_inLayer.size() != network.v_bias_inLayer.size())
        return false;
    if(v_bias_hiddenLayer_1.size() != network.v_bias_hiddenLayer_1.size())
        return false;
    if(v_bias_hiddenLayer_2.size() != network.v_bias_hiddenLayer_2.size())
        return false;

    return true;
}

void NeuralNetwork::crossbreeding(const NeuralNetwork& network1, const NeuralNetwork& network2, double score1, double score2) {
    if(!network1.valid() || !network2.valid() || !network1.equalSize(network2))
        return;

    v_weight_inLayer.clear();
    v_weight_hiddenLayer_1.clear();
    v_weight_hiddenLayer_2.clear();

    v_bias_inLayer.clear();
    v_bias_hiddenLayer_1.clear();
    v_bias_hiddenLayer_2.clear();

    double p1 = score1/(score1 + score2);

    for(int i = 0; i < network1.v_weight_inLayer.size(); i++) {
        v_weight_inLayer.emplace_back();
        for (int j = 0; j < network1.v_weight_inLayer[i].size(); j++) {
            int s = (double)rand() / RAND_MAX;
            double w = (s <= p1) ? network1.v_weight_inLayer[i][j] : network2.v_weight_inLayer[i][j];
            v_weight_inLayer[i].emplace_back(w);
        }
    }

    for(int i = 0; i < network1.v_weight_hiddenLayer_1.size(); i++) {
        v_weight_hiddenLayer_1.emplace_back();
        for (int j = 0; j < network1.v_weight_hiddenLayer_1[i].size(); j++) {
            int s = (double)rand() / RAND_MAX;
            double w = (s <= p1) ? network1.v_weight_hiddenLayer_1[i][j] : network2.v_weight_hiddenLayer_1[i][j];
            v_weight_hiddenLayer_1[i].emplace_back(w);
        }
    }

    for(int i = 0; i < network1.v_weight_hiddenLayer_2.size(); i++) {
        v_weight_hiddenLayer_2.emplace_back();
        for (int j = 0; j < network1.v_weight_hiddenLayer_2[i].size(); j++) {
            int s = (double)rand() / RAND_MAX;
            double w = (s <= p1) ? network1.v_weight_hiddenLayer_2[i][j] : network2.v_weight_hiddenLayer_2[i][j];
            v_weight_hiddenLayer_2[i].emplace_back(w);
        }
    }

    for (int i = 0; i < network1.v_bias_inLayer.size(); i++) {
        int s = (double)rand() / RAND_MAX;
        double w = (s <= p1) ? network1.v_bias_inLayer[i] : network2.v_bias_inLayer[i];
        v_bias_inLayer.emplace_back(w);
    }
    for (int i = 0; i < network1.v_bias_hiddenLayer_1.size(); i++) {
        int s = (double)rand() / RAND_MAX;
        double w = (s <= p1) ? network1.v_bias_hiddenLayer_1[i] : network2.v_bias_hiddenLayer_1[i];
        v_bias_hiddenLayer_1.emplace_back(w);
    }
    for (int i = 0; i < network1.v_bias_hiddenLayer_2.size(); i++) {
        int s = (double)rand() / RAND_MAX;
        double w = (s <= p1) ? network1.v_bias_hiddenLayer_2[i] : network2.v_bias_hiddenLayer_2[i];
        v_bias_hiddenLayer_2.emplace_back(w);
    }

    d_check = check();

    b_valid = true;
}

double NeuralNetwork::check() const {
    double res = 0;

    for(auto& w1 : v_weight_inLayer)
        for(auto& w11 : w1)
            res += w11;
    for(auto& w1 : v_weight_hiddenLayer_1)
        for(auto& w11 : w1)
            res += w11;
    for(auto& w1 : v_weight_hiddenLayer_2)
        for(auto& w11 : w1)
            res += w11;

    for(auto& b1 : v_bias_inLayer)
        res += b1;
    for(auto& b1 : v_bias_hiddenLayer_1)
        res += b1;
    for(auto& b1 : v_bias_hiddenLayer_2)
        res += b1;

    return res;
}

bool NeuralNetwork::sameNet(const NeuralNetwork& network) {

    if(!network.b_valid) {
        b_valid = false;
        return false;
    }

    v_inLayer = network.v_inLayer;
    v_hiddenLayer_1 = network.v_hiddenLayer_1;
    v_hiddenLayer_2 = network.v_hiddenLayer_2;
    v_outLayer = network.v_outLayer;

    v_weight_inLayer = network.v_weight_inLayer;
    v_weight_hiddenLayer_1 = network.v_weight_hiddenLayer_1;
    v_weight_hiddenLayer_2 = network.v_weight_hiddenLayer_2;

    v_bias_inLayer = network.v_bias_inLayer;
    v_bias_hiddenLayer_1 = network.v_bias_hiddenLayer_1;
    v_bias_hiddenLayer_2 = network.v_bias_hiddenLayer_2;

    b_valid = true;

    d_check = check();

    return b_valid;
}