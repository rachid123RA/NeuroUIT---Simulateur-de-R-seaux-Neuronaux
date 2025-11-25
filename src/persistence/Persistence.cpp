#include "persistence/Persistence.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>

namespace NeuroUIT {

bool Persistence::saveNetwork(std::shared_ptr<Network> network, const std::string& filename) {
    if (!network) {
        return false;
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // En-tête du fichier
    file << "# NeuroUIT Network File v" << FILE_VERSION << "\n";
    file << "# Format: Architecture | Activations | Weights\n\n";
    
    // Architecture
    auto architecture = network->getArchitecture();
    file << "ARCHITECTURE:";
    for (size_t size : architecture) {
        file << " " << size;
    }
    file << "\n";
    
    // Types d'activation
    auto activationTypes = network->getActivationTypes();
    file << "ACTIVATIONS:";
    for (ActivationType type : activationTypes) {
        int typeInt = static_cast<int>(type);
        file << " " << typeInt;
    }
    file << "\n";
    
    // Poids
    file << "WEIGHTS:\n";
    auto allWeights = network->getAllWeights();
    
    for (size_t layerIdx = 0; layerIdx < allWeights.size(); ++layerIdx) {
        file << "LAYER " << layerIdx << ":\n";
        const auto& layerWeights = allWeights[layerIdx];
        
        for (size_t neuronIdx = 0; neuronIdx < layerWeights.size(); ++neuronIdx) {
            file << "NEURON " << neuronIdx << ":";
            const auto& neuronWeights = layerWeights[neuronIdx];
            
            for (double weight : neuronWeights) {
                file << " " << std::fixed << std::setprecision(10) << weight;
            }
            
            // Biais (pour l'instant, on ne stocke pas le biais séparément)
            // On le stockera dans les poids pour simplifier
            file << "\n";
        }
    }
    
    file.close();
    return true;
}

std::shared_ptr<Network> Persistence::loadNetwork(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return nullptr;
    }
    
    std::string line;
    
    // Lire la version
    std::getline(file, line);
    
    // Lire l'architecture
    std::getline(file, line); // Ligne vide
    std::getline(file, line);
    
    if (line.find("ARCHITECTURE:") == std::string::npos) {
        file.close();
        return nullptr;
    }
    
    std::vector<size_t> architecture;
    std::istringstream archStream(line.substr(13)); // Après "ARCHITECTURE:"
    size_t size;
    while (archStream >> size) {
        architecture.push_back(size);
    }
    
    // Lire les types d'activation
    std::getline(file, line);
    if (line.find("ACTIVATIONS:") == std::string::npos) {
        file.close();
        return nullptr;
    }
    
    std::vector<ActivationType> activationTypes;
    std::istringstream actStream(line.substr(12)); // Après "ACTIVATIONS:"
    int typeInt;
    while (actStream >> typeInt) {
        activationTypes.push_back(static_cast<ActivationType>(typeInt));
    }
    
    // Créer le réseau
    auto network = std::make_shared<Network>(architecture, activationTypes);
    
    // Lire "WEIGHTS:"
    std::getline(file, line);
    if (line.find("WEIGHTS:") == std::string::npos) {
        file.close();
        return network; // Retourner le réseau même sans poids
    }
    
    // Lire les poids couche par couche
    std::vector<std::vector<std::vector<double>>> allWeights;
    allWeights.resize(architecture.size());
    
    size_t currentLayer = 0;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        if (line.find("LAYER ") != std::string::npos) {
            // Nouvelle couche
            size_t layerIdx;
            std::istringstream layerStream(line.substr(6)); // Après "LAYER "
            layerStream >> layerIdx;
            currentLayer = layerIdx;
            
            if (currentLayer >= architecture.size()) {
                file.close();
                return nullptr;
            }
            
            allWeights[currentLayer].resize(architecture[currentLayer]);
        } else if (line.find("NEURON ") != std::string::npos) {
            // Poids d'un neurone
            size_t neuronIdx;
            std::istringstream neuronStream(line.substr(7)); // Après "NEURON "
            neuronStream >> neuronIdx;
            
            size_t colonPos = line.find(':');
            if (colonPos == std::string::npos) {
                file.close();
                return nullptr;
            }
            
            std::istringstream weightStream(line.substr(colonPos + 1));
            std::vector<double> weights;
            double weight;
            
            while (weightStream >> weight) {
                weights.push_back(weight);
            }
            
            if (currentLayer < allWeights.size() && 
                neuronIdx < allWeights[currentLayer].size()) {
                allWeights[currentLayer][neuronIdx] = weights;
            }
        }
    }
    
    // Définir les poids
    try {
        network->setAllWeights(allWeights);
    } catch (const std::exception&) {
        file.close();
        return nullptr;
    }
    
    file.close();
    return network;
}

bool Persistence::exportResultsToCSV(const std::vector<double>& epochErrors,
                                     const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // En-têtes
    file << "Epoch,Error\n";
    
    // Données
    for (size_t i = 0; i < epochErrors.size(); ++i) {
        file << i << "," << std::fixed << std::setprecision(10) << epochErrors[i] << "\n";
    }
    
    file.close();
    return true;
}

bool Persistence::validateFileFormat(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::getline(file, line);
    
    bool isValid = line.find("NeuroUIT Network File") != std::string::npos;
    file.close();
    
    return isValid;
}

} // namespace NeuroUIT










