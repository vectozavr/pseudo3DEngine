//
// Created by Иван Ильин on 27.07.2020.
//

#ifndef FPSONLINE_NEURALNETWORK_H
#define FPSONLINE_NEURALNETWORK_H

#include <vector>
#include "settings.h"

class NeuralNetwork {
private:
    bool b_valid = false;

    std::vector<double> v_inLayer;
    std::vector<double> v_hiddenLayer_1;
    std::vector<double> v_hiddenLayer_2;

    std::vector<std::vector<double>> v_weight_inLayer;
    std::vector<std::vector<double>> v_weight_hiddenLayer_1;
    std::vector<std::vector<double>> v_weight_hiddenLayer_2;

    std::vector<double> v_bias_inLayer;
    std::vector<double> v_bias_hiddenLayer_1;
    std::vector<double> v_bias_hiddenLayer_2;

    std::vector<double> v_outLayer; // The result of neural network work

    double d_check = 0;

public:
    explicit NeuralNetwork();
    NeuralNetwork(const NeuralNetwork& network);

    void randomlyInitNetwork();
    void mutateNetwork();

    void saveNetwork(std::string filename, int gen = 0);
    int loadNetwork(const std::string& filename);
    bool sameNet(const NeuralNetwork& network);

    void addInput(double i) { v_inLayer.push_back(i); }
    void clearInput();

    std::vector<double> getOutput();
    void crossbreeding(const NeuralNetwork& network1, const NeuralNetwork& network2, double score1, double score2);

    [[nodiscard]] std::vector<std::vector<double>> weight1() const { return v_weight_inLayer; }
    [[nodiscard]] std::vector<std::vector<double>> weight2() const { return v_weight_hiddenLayer_1; }
    //[[nodiscard]] std::vector<std::vector<double>> weight3() const { return v_weight_hiddenLayer_2; }

    [[nodiscard]] std::vector<double> bias1() const { return v_bias_inLayer; }
    [[nodiscard]] std::vector<double> bias2() const { return v_bias_hiddenLayer_1; }
    //[[nodiscard]] std::vector<double> bias3() const { return v_bias_hiddenLayer_2; }

    void setWeight1 (const std::vector<std::vector<double>> w) { v_weight_inLayer = w; }
    void setWeight2 (const std::vector<std::vector<double>> w) { v_weight_hiddenLayer_1 = w; }
    //void setWeight3 (const std::vector<std::vector<double>> w) { v_weight_hiddenLayer_2 = w; }
    void setBias1   (const std::vector<double> b) { v_bias_inLayer = b; }
    void setBias2   (const std::vector<double> b) { v_bias_hiddenLayer_1 = b; }
    //void setBias3   (const std::vector<double> b) { v_bias_hiddenLayer_2 = b; }

    static double sigmoid(double x);
    static double relu(double x);

    [[nodiscard]] bool valid() const { return b_valid; }
    [[nodiscard]] bool equalSize(const NeuralNetwork& network) const;

    [[nodiscard]] double check() const;


};


#endif //FPSONLINE_NEURALNETWORK_H
