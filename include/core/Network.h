#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <memory>
#include <random>
#include <string>
#include "core/Layer.h"
#include "core/ActivationFunction.h"

namespace NeuroUIT {

/**
 * @brief Représente un réseau de neurones multicouche
 */
class Network {
public:
    /**
     * @brief Constructeur
     * @param architecture Description de l'architecture : nombre de neurones par couche
     * @param activations Fonctions d'activation pour chaque couche (sauf entrée)
     */
    Network(const std::vector<size_t>& architecture,
            const std::vector<ActivationType>& activations);
    
    /**
     * @brief Constructeur par défaut (crée un réseau vide)
     */
    Network();
    
    /**
     * @brief Initialise les poids du réseau
     * @param seed Graine pour le générateur aléatoire
     */
    void initializeWeights(unsigned int seed = std::random_device{}());
    
    /**
     * @brief Propagation avant (forward pass)
     * @param inputs Valeurs d'entrée
     * @return Sorties du réseau
     */
    std::vector<double> forward(const std::vector<double>& inputs);
    
    /**
     * @brief Rétropropagation (backward pass)
     * @param targets Valeurs cibles (sorties attendues)
     * @return Erreur quadratique moyenne
     */
    double backward(const std::vector<double>& targets);
    
    /**
     * @brief Met à jour les poids après backpropagation
     * @param learningRate Taux d'apprentissage
     * @param momentum Facteur de momentum
     */
    void updateWeights(double learningRate, double momentum = 0.0);
    
    /**
     * @brief Obtient l'architecture du réseau
     */
    std::vector<size_t> getArchitecture() const;
    
    /**
     * @brief Obtient les couches
     */
    const std::vector<Layer>& getLayers() const { return layers_; }
    std::vector<Layer>& getLayers() { return layers_; }
    
    /**
     * @brief Obtient le nombre de couches
     */
    size_t getNumLayers() const { return layers_.size(); }
    
    /**
     * @brief Obtient la dernière sortie calculée
     */
    const std::vector<double>& getLastOutput() const { return lastOutput_; }
    
    /**
     * @brief Calcule l'erreur quadratique moyenne entre prédictions et cibles
     * @param predictions Prédictions du réseau
     * @param targets Valeurs cibles
     * @return Erreur quadratique moyenne
     */
    static double computeMeanSquaredError(const std::vector<double>& predictions,
                                         const std::vector<double>& targets);
    
    /**
     * @brief Obtient les poids de toutes les couches (pour sauvegarde)
     */
    std::vector<std::vector<std::vector<double>>> getAllWeights() const;
    
    /**
     * @brief Définit les poids de toutes les couches (pour chargement)
     */
    void setAllWeights(const std::vector<std::vector<std::vector<double>>>& weights);
    
    /**
     * @brief Obtient les fonctions d'activation
     */
    std::vector<ActivationType> getActivationTypes() const;

private:
    std::vector<Layer> layers_;                    // Couches du réseau
    std::vector<ActivationType> activationTypes_;  // Types d'activation par couche
    std::vector<double> lastOutput_;               // Dernière sortie calculée
    std::mt19937 generator_;                      // Générateur aléatoire
    
    /**
     * @brief Crée le réseau à partir de l'architecture
     */
    void createLayers(const std::vector<size_t>& architecture,
                      const std::vector<ActivationType>& activations);
};

} // namespace NeuroUIT

#endif // NETWORK_H


