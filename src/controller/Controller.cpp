#include "controller/Controller.h"
#include <QThread>
#include <memory>
#include <algorithm>
#include <cmath>
#include <numeric>

namespace NeuroUIT {

Controller::Controller(QObject* parent)
    : QObject(parent)
{
}

Controller::~Controller() {
    if (trainer_) {
        trainer_->stop();
    }
}

bool Controller::createNetwork(const std::vector<size_t>& architecture,
                              const std::vector<ActivationType>& activations) {
    try {
        network_ = std::make_shared<Network>(architecture, activations);
        network_->initializeWeights();
        initializeTrainer();
        emit networkCreated();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Controller::loadNetwork(const std::string& filename) {
    network_ = Persistence::loadNetwork(filename);
    if (network_) {
        initializeTrainer();
        emit networkCreated();
        return true;
    }
    return false;
}

bool Controller::saveNetwork(const std::string& filename) {
    if (!network_) {
        return false;
    }
    return Persistence::saveNetwork(network_, filename);
}

bool Controller::loadDataset(const std::string& filename,
                             size_t numInputs,
                             size_t numOutputs,
                             bool hasHeader) {
    bool success = datasetManager_.loadFromCSV(filename, numInputs, numOutputs, hasHeader);
    if (success) {
        // Séparer train/test
        auto [train, test] = datasetManager_.splitTrainTest(0.7);
        trainSamples_ = train;
        testSamples_ = test;
        emit datasetLoaded();
    }
    return success;
}

void Controller::startTraining(const Trainer::TrainingParams& params) {
    if (!network_ || trainSamples_.empty()) {
        return;
    }
    
    initializeTrainer();
    
    trainingErrors_.clear();
    
    // Créer un thread pour l'entraînement (pour ne pas bloquer l'UI)
    // Note: Pour une vraie implémentation thread-safe, utiliser QThread
    // Pour simplifier ici, on fait l'entraînement de manière synchrone
    // mais on émet des signaux pour la progression
    
    Trainer::ProgressCallback callback = [this](size_t epoch, double error) {
        trainingErrors_.push_back(error);
        emit trainingProgress(epoch, error);
    };
    
    trainer_->train(trainSamples_, params, callback);
    
    emit trainingFinished();
}

void Controller::stopTraining() {
    if (trainer_) {
        trainer_->stop();
    }
}

double Controller::testNetwork() {
    if (!network_ || testSamples_.empty()) {
        return 0.0;
    }
    
    initializeTrainer();
    return trainer_->test(testSamples_);
}

bool Controller::exportResults(const std::string& filename) {
    if (trainingErrors_.empty()) {
        return false;
    }
    return Persistence::exportResultsToCSV(trainingErrors_, filename);
}

DatasetManager::Statistics Controller::getDatasetStatistics() const {
    return datasetManager_.getStatistics();
}

Controller::TestResults Controller::getDetailedTestResults() {
    TestResults results;
    
    if (!network_ || testSamples_.empty()) {
        return results;
    }
    
    // Déterminer si c'est un problème de classification ou de régression
    // On considère que c'est une classification si les sorties sont binaires (0 ou 1)
    bool isBinary = true;
    for (const auto& sample : testSamples_) {
        for (double output : sample.outputs) {
            if (output != 0.0 && output != 1.0) {
                isBinary = false;
                break;
            }
        }
        if (!isBinary) break;
    }
    results.isClassification = isBinary && testSamples_[0].outputs.size() == 1;
    
    // Faire les prédictions
    results.predictions.reserve(testSamples_.size());
    results.actuals.reserve(testSamples_.size());
    
    double sumSquaredError = 0.0;
    double sumAbsoluteError = 0.0;
    
    for (const auto& sample : testSamples_) {
        std::vector<double> prediction = network_->forward(sample.inputs);
        
        for (size_t i = 0; i < prediction.size(); ++i) {
            results.predictions.push_back(prediction[i]);
            results.actuals.push_back(sample.outputs[i]);
            
            double error = prediction[i] - sample.outputs[i];
            sumSquaredError += error * error;
            sumAbsoluteError += std::abs(error);
        }
    }
    
    size_t numValues = results.predictions.size();
    if (numValues > 0) {
        results.mse = sumSquaredError / numValues;
        results.mae = sumAbsoluteError / numValues;
        
        // Calculer R²
        double meanActual = std::accumulate(results.actuals.begin(), results.actuals.end(), 0.0) / numValues;
        double totalSumSquares = 0.0;
        for (double actual : results.actuals) {
            double diff = actual - meanActual;
            totalSumSquares += diff * diff;
        }
        if (totalSumSquares > 0.0) {
            results.r2 = 1.0 - (sumSquaredError / totalSumSquares);
        }
        
        // Calculer les métriques de classification
        if (results.isClassification) {
            // Créer la matrice de confusion
            size_t numClasses = 2; // Binaire pour l'instant
            results.confusionMatrix.resize(numClasses, std::vector<int>(numClasses, 0));
            
            int correct = 0;
            for (size_t i = 0; i < numValues; ++i) {
                int predicted = (results.predictions[i] >= 0.5) ? 1 : 0;
                int actual = (results.actuals[i] >= 0.5) ? 1 : 0;
                results.confusionMatrix[predicted][actual]++;
                if (predicted == actual) correct++;
            }
            
            results.accuracy = static_cast<double>(correct) / numValues;
            
            // Calculer précision, rappel, F1
            int tp = results.confusionMatrix[1][1]; // True Positive
            int fp = results.confusionMatrix[1][0]; // False Positive
            int fn = results.confusionMatrix[0][1]; // False Negative
            
            if (tp + fp > 0) {
                results.precision = static_cast<double>(tp) / (tp + fp);
            }
            if (tp + fn > 0) {
                results.recall = static_cast<double>(tp) / (tp + fn);
            }
            if (results.precision + results.recall > 0) {
                results.f1Score = 2.0 * (results.precision * results.recall) / 
                                 (results.precision + results.recall);
            }
        }
    }
    
    return results;
}

std::vector<std::vector<double>> Controller::getCorrelationMatrix() const {
    std::vector<std::vector<double>> correlationMatrix;
    
    if (testSamples_.empty()) {
        return correlationMatrix;
    }
    
    size_t numInputs = testSamples_[0].inputs.size();
    size_t numOutputs = testSamples_[0].outputs.size();
    size_t numVars = numInputs + numOutputs;
    
    // Créer une matrice de données combinée
    std::vector<std::vector<double>> data(numVars);
    for (size_t i = 0; i < numInputs; ++i) {
        data[i].reserve(testSamples_.size());
        for (const auto& sample : testSamples_) {
            data[i].push_back(sample.inputs[i]);
        }
    }
    for (size_t i = 0; i < numOutputs; ++i) {
        data[numInputs + i].reserve(testSamples_.size());
        for (const auto& sample : testSamples_) {
            data[numInputs + i].push_back(sample.outputs[i]);
        }
    }
    
    // Calculer la matrice de corrélation
    correlationMatrix.resize(numVars, std::vector<double>(numVars, 0.0));
    
    for (size_t i = 0; i < numVars; ++i) {
        for (size_t j = 0; j < numVars; ++j) {
            if (i == j) {
                correlationMatrix[i][j] = 1.0;
            } else {
                // Calculer la corrélation de Pearson
                double meanI = std::accumulate(data[i].begin(), data[i].end(), 0.0) / data[i].size();
                double meanJ = std::accumulate(data[j].begin(), data[j].end(), 0.0) / data[j].size();
                
                double numerator = 0.0;
                double sumSqI = 0.0;
                double sumSqJ = 0.0;
                
                for (size_t k = 0; k < data[i].size(); ++k) {
                    double diffI = data[i][k] - meanI;
                    double diffJ = data[j][k] - meanJ;
                    numerator += diffI * diffJ;
                    sumSqI += diffI * diffI;
                    sumSqJ += diffJ * diffJ;
                }
                
                double denominator = std::sqrt(sumSqI * sumSqJ);
                if (denominator > 0.0) {
                    correlationMatrix[i][j] = numerator / denominator;
                }
            }
        }
    }
    
    return correlationMatrix;
}

void Controller::initializeTrainer() {
    if (network_ && !trainer_) {
        trainer_ = std::make_shared<Trainer>(network_);
    } else if (network_ && trainer_) {
        // Recréer le trainer si le réseau a changé
        trainer_ = std::make_shared<Trainer>(network_);
    }
}

} // namespace NeuroUIT










