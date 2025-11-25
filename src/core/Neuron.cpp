#include "core/Neuron.h"
#include <numeric>
#include <algorithm>
#include <stdexcept>

namespace NeuroUIT {

Neuron::Neuron(size_t numInputs, std::shared_ptr<ActivationFunction> activation)
    : weights_(numInputs, 0.0)
    , bias_(0.0)
    , output_(0.0)
    , netInput_(0.0)
    , delta_(0.0)
    , activation_(activation)
{
}

void Neuron::initializeWeights(std::mt19937& generator, double mean, double stddev) {
    std::normal_distribution<double> dist(mean, stddev);
    for (auto& weight : weights_) {
        weight = dist(generator);
    }
    bias_ = dist(generator);
}

void Neuron::setWeights(const std::vector<double>& weights) {
    if (weights.size() != weights_.size()) {
        throw std::invalid_argument("Taille des poids incorrecte");
    }
    weights_ = weights;
}

double Neuron::forward(const std::vector<double>& inputs) {
    if (inputs.size() != weights_.size()) {
        throw std::invalid_argument("Nombre d'entrées incorrect");
    }
    
    // Calcul de la somme pondérée + biais
    netInput_ = std::inner_product(inputs.begin(), inputs.end(), weights_.begin(), 0.0);
    netInput_ += bias_;
    
    // Application de la fonction d'activation (si présente)
    if (activation_) {
        output_ = activation_->activate(netInput_);
    } else {
        output_ = netInput_; // Pas d'activation pour la couche d'entrée
    }
    
    return output_;
}

void Neuron::updateWeights(const std::vector<double>& inputs, double learningRate,
                          double momentum, std::vector<double>& prevWeightUpdates) {
    if (inputs.size() != weights_.size()) {
        throw std::invalid_argument("Nombre d'entrées incorrect pour mise à jour");
    }
    
    // Calcul de la dérivée de la fonction d'activation
    double activationDerivative = 1.0;
    if (activation_) {
        activationDerivative = activation_->derivative(netInput_);
    }
    
    // Multiplier delta par la dérivée de l'activation
    double finalDelta = delta_ * activationDerivative;
    
    // Mise à jour des poids avec momentum
    for (size_t i = 0; i < weights_.size(); ++i) {
        double weightUpdate = learningRate * finalDelta * inputs[i];
        if (momentum > 0.0 && i < prevWeightUpdates.size()) {
            weightUpdate += momentum * prevWeightUpdates[i];
        }
        weights_[i] += weightUpdate;
        if (i < prevWeightUpdates.size()) {
            prevWeightUpdates[i] = weightUpdate;
        } else {
            prevWeightUpdates.push_back(weightUpdate);
        }
    }
    
    // Mise à jour du biais
    double biasUpdate = learningRate * finalDelta;
    bias_ += biasUpdate;
}

} // namespace NeuroUIT

