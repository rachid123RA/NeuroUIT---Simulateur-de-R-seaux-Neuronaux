#include "core/Layer.h"
#include <stdexcept>

namespace NeuroUIT {

Layer::Layer(size_t numNeurons, size_t numInputs, 
             std::shared_ptr<ActivationFunction> activation)
    : numInputs_(numInputs)
    , outputs_(numNeurons)
    , prevWeightUpdates_(numNeurons)
{
    neurons_.reserve(numNeurons);
    for (size_t i = 0; i < numNeurons; ++i) {
        neurons_.emplace_back(numInputs, activation);
        prevWeightUpdates_[i].resize(numInputs, 0.0);
    }
}

void Layer::initializeWeights(std::mt19937& generator, double mean, double stddev) {
    for (auto& neuron : neurons_) {
        neuron.initializeWeights(generator, mean, stddev);
    }
}

std::vector<double> Layer::forward(const std::vector<double>& inputs) {
    if (inputs.size() != numInputs_) {
        throw std::invalid_argument("Nombre d'entrées incorrect pour la couche");
    }
    
    prevInputs_ = inputs;
    outputs_.resize(neurons_.size());
    
    for (size_t i = 0; i < neurons_.size(); ++i) {
        outputs_[i] = neurons_[i].forward(inputs);
    }
    
    return outputs_;
}

void Layer::backward(const std::vector<double>& errors, const std::vector<double>& prevLayerOutputs) {
    (void)prevLayerOutputs; // Utilisé pour la cohérence de l'interface, peut être utilisé dans des implémentations futures
    if (errors.size() != neurons_.size()) {
        throw std::invalid_argument("Nombre d'erreurs incorrect");
    }
    
    for (size_t i = 0; i < neurons_.size(); ++i) {
        double delta = errors[i];
        
        // Multiplier par la dérivée de l'activation
        if (neurons_[i].getActivation()) {
            double activationDerivative = neurons_[i].getActivation()->derivative(
                neurons_[i].getNetInput());
            delta *= activationDerivative;
        }
        
        neurons_[i].setDelta(delta);
    }
}

std::vector<double> Layer::getErrorsForPrevLayer() const {
    if (neurons_.empty()) {
        return {};
    }
    
    size_t numPrevNeurons = numInputs_;
    std::vector<double> errors(numPrevNeurons, 0.0);
    
    // Pour chaque neurone de cette couche
    for (const auto& neuron : neurons_) {
        const auto& weights = neuron.getWeights();
        double delta = neuron.getDelta();
        
        // Propager l'erreur vers chaque neurone de la couche précédente
        for (size_t j = 0; j < weights.size() && j < numPrevNeurons; ++j) {
            errors[j] += weights[j] * delta;
        }
    }
    
    return errors;
}

void Layer::updateWeights(double learningRate, double momentum) {
    for (size_t i = 0; i < neurons_.size(); ++i) {
        neurons_[i].updateWeights(prevInputs_, learningRate, momentum, prevWeightUpdates_[i]);
    }
}

void Layer::setOutputs(const std::vector<double>& outputs) {
    outputs_ = outputs;
}

} // namespace NeuroUIT


