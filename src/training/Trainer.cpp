#include "training/Trainer.h"
#include <algorithm>
#include <random>
#include <numeric>
#include <cmath>
#include <stdexcept>

namespace NeuroUIT {

Trainer::Trainer(std::shared_ptr<Network> network)
    : network_(network)
    , isTraining_(false)
    , shouldStop_(false)
{
}

Trainer::TrainingResults Trainer::train(const std::vector<DatasetManager::Sample>& trainSamples,
                                        const TrainingParams& params,
                                        ProgressCallback callback) {
    if (!network_) {
        throw std::runtime_error("Aucun réseau assigné au Trainer");
    }
    
    if (trainSamples.empty()) {
        throw std::invalid_argument("Aucun échantillon d'entraînement fourni");
    }
    
    isTraining_ = true;
    shouldStop_ = false;
    
    TrainingResults results;
    results.epochErrors.reserve(params.numEpochs);
    
    std::vector<DatasetManager::Sample> currentSamples = trainSamples;
    std::random_device rd;
    std::mt19937 generator(rd());
    
    for (size_t epoch = 0; epoch < params.numEpochs && !shouldStop_; ++epoch) {
        // Mélanger les échantillons si demandé
        if (params.shuffle && epoch > 0) {
            std::shuffle(currentSamples.begin(), currentSamples.end(), generator);
        }
        
        // Entraîner une époque
        double epochError = trainEpoch(currentSamples, params);
        
        results.epochErrors.push_back(epochError);
        results.epochsCompleted = epoch + 1;
        
        // Appeler le callback
        if (callback) {
            callback(epoch, epochError);
        }
        
        // Vérifier si on doit arrêter
        if (shouldStop_) {
            results.stopped = true;
            break;
        }
    }
    
    results.finalError = results.epochErrors.empty() ? 0.0 : results.epochErrors.back();
    isTraining_ = false;
    
    return results;
}

double Trainer::trainEpoch(const std::vector<DatasetManager::Sample>& trainSamples,
                          const TrainingParams& params) {
    std::vector<double> batchErrors;
    batchErrors.reserve(trainSamples.size() / params.batchSize + 1);
    
    // Traiter par batches
    for (size_t i = 0; i < trainSamples.size(); i += params.batchSize) {
        size_t batchEnd = std::min(i + params.batchSize, trainSamples.size());
        std::vector<DatasetManager::Sample> batch(
            trainSamples.begin() + i,
            trainSamples.begin() + batchEnd
        );
        
        // Traiter le batch
        processBatch(batch, params);
        
        // Calculer l'erreur moyenne du batch
        double batchError = 0.0;
        for (const auto& sample : batch) {
            network_->forward(sample.inputs);
            double error = Network::computeMeanSquaredError(
                network_->getLastOutput(),
                sample.outputs
            );
            batchError += error;
        }
        batchError /= batch.size();
        batchErrors.push_back(batchError);
        
        if (shouldStop_) {
            break;
        }
    }
    
    // Retourner l'erreur moyenne de l'époque
    if (batchErrors.empty()) {
        return 0.0;
    }
    return std::accumulate(batchErrors.begin(), batchErrors.end(), 0.0) / batchErrors.size();
}

void Trainer::processBatch(const std::vector<DatasetManager::Sample>& batchSamples,
                          const TrainingParams& params) {
    // Accumuler les gradients pour le batch
    for (const auto& sample : batchSamples) {
        // Forward pass
        network_->forward(sample.inputs);
        
        // Backward pass
        network_->backward(sample.outputs);
    }
    
    // Mise à jour des poids avec la moyenne des gradients du batch
    double effectiveLearningRate = params.learningRate / batchSamples.size();
    network_->updateWeights(effectiveLearningRate, params.momentum);
}

double Trainer::test(const std::vector<DatasetManager::Sample>& testSamples) {
    if (!network_) {
        throw std::runtime_error("Aucun réseau assigné au Trainer");
    }
    
    if (testSamples.empty()) {
        return 0.0;
    }
    
    std::vector<double> errors;
    errors.reserve(testSamples.size());
    
    for (const auto& sample : testSamples) {
        network_->forward(sample.inputs);
        double error = Network::computeMeanSquaredError(
            network_->getLastOutput(),
            sample.outputs
        );
        errors.push_back(error);
    }
    
    // Retourner l'erreur moyenne
    return std::accumulate(errors.begin(), errors.end(), 0.0) / errors.size();
}

void Trainer::stop() {
    shouldStop_ = true;
}

} // namespace NeuroUIT










