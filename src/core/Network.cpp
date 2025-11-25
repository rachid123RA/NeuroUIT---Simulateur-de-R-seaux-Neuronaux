#include "core/Network.h"
#include "core/Layer.h"
#include "core/ActivationFunction.h"
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace NeuroUIT {

Network::Network()
    : generator_(std::random_device{}())
{
}

Network::Network(const std::vector<size_t>& architecture,
                 const std::vector<ActivationType>& activations)
    : activationTypes_(activations)
    , generator_(std::random_device{}())
{
    createLayers(architecture, activations);
    initializeWeights();
}

void Network::createLayers(const std::vector<size_t>& architecture,
                           const std::vector<ActivationType>& activations) {
    if (architecture.empty()) {
        throw std::invalid_argument("L'architecture ne peut pas être vide");
    }
    
    if (architecture.size() < 2) {
        throw std::invalid_argument("Le réseau doit avoir au moins 2 couches (entrée et sortie)");
    }
    
    layers_.clear();
    
    // Couche d'entrée (pas de fonction d'activation)
    size_t numInputs = architecture[0];
    layers_.emplace_back(architecture[0], numInputs, nullptr);
    
    // Couches cachées et de sortie
    for (size_t i = 1; i < architecture.size(); ++i) {
        size_t numInputsForLayer = architecture[i - 1];
        ActivationType activationType = (i < activations.size()) 
            ? activations[i - 1] 
            : ActivationType::Sigmoid; // Par défaut
        
        auto activation = ActivationFunction::create(activationType);
        layers_.emplace_back(architecture[i], numInputsForLayer, std::move(activation));
    }
}

void Network::initializeWeights(unsigned int seed) {
    generator_.seed(seed);
    
    // Initialisation Xavier/Glorot pour chaque couche
    for (size_t i = 1; i < layers_.size(); ++i) {
        size_t numInputs = layers_[i].getNumInputs();
        size_t numNeurons = layers_[i].getNumNeurons();
        
        // Variance = 2 / (n_in + n_out)
        double variance = 2.0 / (numInputs + numNeurons);
        double stddev = std::sqrt(variance);
        
        layers_[i].initializeWeights(generator_, 0.0, stddev);
    }
}

std::vector<double> Network::forward(const std::vector<double>& inputs) {
    if (layers_.empty()) {
        throw std::runtime_error("Le réseau n'a pas de couches");
    }
    
    if (inputs.size() != layers_[0].getNumNeurons()) {
        throw std::invalid_argument("Nombre d'entrées incorrect");
    }
    
    // Propagation avant à travers toutes les couches
    std::vector<double> currentOutputs = inputs;
    
    for (size_t i = 0; i < layers_.size(); ++i) {
        if (i == 0) {
            // Couche d'entrée : juste passer les valeurs
            layers_[i].setOutputs(inputs);
            currentOutputs = inputs;
        } else {
            // Couches suivantes : propagation
            currentOutputs = layers_[i].forward(currentOutputs);
        }
    }
    
    lastOutput_ = currentOutputs;
    return currentOutputs;
}

double Network::backward(const std::vector<double>& targets) {
    if (layers_.empty()) {
        throw std::runtime_error("Le réseau n'a pas de couches");
    }
    
    if (targets.size() != lastOutput_.size()) {
        throw std::invalid_argument("Nombre de sorties cibles incorrect");
    }
    
    // Calculer l'erreur de la couche de sortie
    std::vector<double> outputErrors(lastOutput_.size());
    for (size_t i = 0; i < lastOutput_.size(); ++i) {
        outputErrors[i] = lastOutput_[i] - targets[i];
    }
    
    // Rétropropagation de la couche de sortie vers l'entrée
    std::vector<double> currentErrors = outputErrors;
    
    for (int i = static_cast<int>(layers_.size()) - 1; i >= 1; --i) {
        // Obtenir les sorties de la couche précédente
        std::vector<double> prevOutputs = (i > 1) 
            ? layers_[i - 1].getOutputs() 
            : layers_[0].getOutputs();
        
        // Calculer les erreurs pour cette couche
        layers_[i].backward(currentErrors, prevOutputs);
        
        // Propager les erreurs vers la couche précédente
        if (i > 1) {
            currentErrors = layers_[i].getErrorsForPrevLayer();
        }
    }
    
    // Retourner l'erreur quadratique moyenne
    return computeMeanSquaredError(lastOutput_, targets);
}

void Network::updateWeights(double learningRate, double momentum) {
    for (size_t i = 1; i < layers_.size(); ++i) {
        layers_[i].updateWeights(learningRate, momentum);
    }
}

std::vector<size_t> Network::getArchitecture() const {
    std::vector<size_t> architecture;
    for (const auto& layer : layers_) {
        architecture.push_back(layer.getNumNeurons());
    }
    return architecture;
}

double Network::computeMeanSquaredError(const std::vector<double>& predictions,
                                       const std::vector<double>& targets) {
    if (predictions.size() != targets.size()) {
        throw std::invalid_argument("Les tailles des vecteurs doivent être identiques");
    }
    
    if (predictions.empty()) {
        return 0.0;
    }
    
    double sumSquaredError = 0.0;
    for (size_t i = 0; i < predictions.size(); ++i) {
        double error = predictions[i] - targets[i];
        sumSquaredError += error * error;
    }
    
    return sumSquaredError / static_cast<double>(predictions.size());
}

std::vector<std::vector<std::vector<double>>> Network::getAllWeights() const {
    std::vector<std::vector<std::vector<double>>> allWeights;
    
    for (const auto& layer : layers_) {
        std::vector<std::vector<double>> layerWeights;
        for (const auto& neuron : layer.getNeurons()) {
            layerWeights.push_back(neuron.getWeights());
        }
        allWeights.push_back(layerWeights);
    }
    
    return allWeights;
}

void Network::setAllWeights(const std::vector<std::vector<std::vector<double>>>& weights) {
    if (weights.size() != layers_.size()) {
        throw std::invalid_argument("Nombre de couches incorrect");
    }
    
    for (size_t i = 0; i < layers_.size(); ++i) {
        auto& layer = layers_[i];
        if (weights[i].size() != layer.getNumNeurons()) {
            throw std::invalid_argument("Nombre de neurones incorrect pour la couche " + std::to_string(i));
        }
        
        auto& neurons = layer.getNeurons();
        for (size_t j = 0; j < neurons.size(); ++j) {
            neurons[j].setWeights(weights[i][j]);
        }
    }
}

std::vector<ActivationType> Network::getActivationTypes() const {
    return activationTypes_;
}

} // namespace NeuroUIT

