#include "dataset/DatasetManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <stdexcept>
#include <limits>
#include <random>

namespace NeuroUIT {

DatasetManager::DatasetManager()
    : numInputs_(0)
    , numOutputs_(0)
{
}

bool DatasetManager::loadFromCSV(const std::string& filename, 
                                size_t numInputs, 
                                size_t numOutputs,
                                bool hasHeader) {
    clear();
    
    numInputs_ = numInputs;
    numOutputs_ = numOutputs;
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    
    // Ignorer l'en-tête si présent
    if (hasHeader && std::getline(file, line)) {
        // Ignorer la première ligne
    }
    
    // Lire les données
    while (std::getline(file, line)) {
        if (line.empty() || line.find_first_not_of(" \t\n\r") == std::string::npos) {
            continue; // Ignorer les lignes vides
        }
        
        std::vector<double> values = parseCSVLine(line);
        
        if (values.size() != numInputs + numOutputs) {
            // Erreur de format
            clear();
            return false;
        }
        
        Sample sample;
        sample.inputs.assign(values.begin(), values.begin() + numInputs);
        sample.outputs.assign(values.begin() + numInputs, values.end());
        
        if (validateSample(sample)) {
            samples_.push_back(sample);
        } else {
            clear();
            return false;
        }
    }
    
    file.close();
    return !samples_.empty();
}

std::vector<double> DatasetManager::parseCSVLine(const std::string& line) {
    std::vector<double> values;
    std::stringstream ss(line);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        // Supprimer les espaces
        item.erase(0, item.find_first_not_of(" \t"));
        item.erase(item.find_last_not_of(" \t") + 1);
        
        if (!item.empty()) {
            try {
                double value = std::stod(item);
                values.push_back(value);
            } catch (const std::exception&) {
                // Erreur de conversion
                return {}; // Retourner un vecteur vide en cas d'erreur
            }
        }
    }
    
    return values;
}

bool DatasetManager::validateSample(const Sample& sample) const {
    if (sample.inputs.size() != numInputs_ || sample.outputs.size() != numOutputs_) {
        return false;
    }
    
    // Vérifier qu'il n'y a pas de NaN ou Inf
    for (double val : sample.inputs) {
        if (std::isnan(val) || std::isinf(val)) {
            return false;
        }
    }
    
    for (double val : sample.outputs) {
        if (std::isnan(val) || std::isinf(val)) {
            return false;
        }
    }
    
    return true;
}

std::pair<std::vector<DatasetManager::Sample>, std::vector<DatasetManager::Sample>> 
DatasetManager::splitTrainTest(double trainRatio) {
    if (samples_.empty()) {
        return {{}, {}};
    }
    
    // Mélanger les échantillons (optionnel mais recommandé)
    std::vector<Sample> shuffled = samples_;
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(shuffled.begin(), shuffled.end(), g);
    
    size_t trainSize = static_cast<size_t>(samples_.size() * trainRatio);
    
    std::vector<Sample> trainSamples(shuffled.begin(), shuffled.begin() + trainSize);
    std::vector<Sample> testSamples(shuffled.begin() + trainSize, shuffled.end());
    
    return {trainSamples, testSamples};
}

void DatasetManager::clear() {
    samples_.clear();
    numInputs_ = 0;
    numOutputs_ = 0;
}

DatasetManager::Statistics DatasetManager::getStatistics() const {
    Statistics stats;
    stats.numSamples = samples_.size();
    stats.numInputs = numInputs_;
    stats.numOutputs = numOutputs_;
    
    if (samples_.empty()) {
        return stats;
    }
    
    // Statistiques sur les entrées
    stats.inputMin.resize(numInputs_, std::numeric_limits<double>::max());
    stats.inputMax.resize(numInputs_, std::numeric_limits<double>::lowest());
    stats.inputMean.resize(numInputs_, 0.0);
    
    // Statistiques sur les sorties
    stats.outputMin.resize(numOutputs_, std::numeric_limits<double>::max());
    stats.outputMax.resize(numOutputs_, std::numeric_limits<double>::lowest());
    stats.outputMean.resize(numOutputs_, 0.0);
    
    // Calculer min, max et somme pour les moyennes
    for (const auto& sample : samples_) {
        for (size_t i = 0; i < numInputs_; ++i) {
            stats.inputMin[i] = std::min(stats.inputMin[i], sample.inputs[i]);
            stats.inputMax[i] = std::max(stats.inputMax[i], sample.inputs[i]);
            stats.inputMean[i] += sample.inputs[i];
        }
        
        for (size_t i = 0; i < numOutputs_; ++i) {
            stats.outputMin[i] = std::min(stats.outputMin[i], sample.outputs[i]);
            stats.outputMax[i] = std::max(stats.outputMax[i], sample.outputs[i]);
            stats.outputMean[i] += sample.outputs[i];
        }
    }
    
    // Calculer les moyennes
    double numSamples = static_cast<double>(samples_.size());
    for (size_t i = 0; i < numInputs_; ++i) {
        stats.inputMean[i] /= numSamples;
    }
    for (size_t i = 0; i < numOutputs_; ++i) {
        stats.outputMean[i] /= numSamples;
    }
    
    return stats;
}

} // namespace NeuroUIT

