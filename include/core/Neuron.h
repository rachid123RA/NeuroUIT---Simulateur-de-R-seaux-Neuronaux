#ifndef NEURON_H
#define NEURON_H

#include <vector>
#include <memory>
#include <random>
#include "core/ActivationFunction.h"

namespace NeuroUIT {

/**
 * @brief Représente un neurone dans le réseau
 */
class Neuron {
public:
    /**
     * @brief Constructeur
     * @param numInputs Nombre de connexions entrantes
     * @param activation Fonction d'activation (peut être nullptr pour la couche d'entrée)
     */
    Neuron(size_t numInputs, std::shared_ptr<ActivationFunction> activation = nullptr);
    
    /**
     * @brief Initialise les poids aléatoirement
     * @param generator Générateur de nombres aléatoires
     * @param mean Moyenne pour l'initialisation
     * @param stddev Écart-type pour l'initialisation
     */
    void initializeWeights(std::mt19937& generator, double mean = 0.0, double stddev = 1.0);
    
    /**
     * @brief Définit les poids manuellement
     * @param weights Nouveaux poids (doit avoir la bonne taille)
     */
    void setWeights(const std::vector<double>& weights);
    
    /**
     * @brief Obtient les poids
     */
    const std::vector<double>& getWeights() const { return weights_; }
    
    /**
     * @brief Obtient le biais
     */
    double getBias() const { return bias_; }
    
    /**
     * @brief Définit le biais
     */
    void setBias(double bias) { bias_ = bias; }
    
    /**
     * @brief Calcule la sortie du neurone (forward pass)
     * @param inputs Valeurs d'entrée
     * @return Valeur activée
     */
    double forward(const std::vector<double>& inputs);
    
    /**
     * @brief Obtient la dernière sortie calculée
     */
    double getOutput() const { return output_; }
    
    /**
     * @brief Obtient la dernière valeur avant activation
     */
    double getNetInput() const { return netInput_; }
    
    /**
     * @brief Obtient la dérivée de l'erreur par rapport à la sortie
     */
    double getDelta() const { return delta_; }
    
    /**
     * @brief Définit la dérivée de l'erreur (pour backpropagation)
     */
    void setDelta(double delta) { delta_ = delta; }
    
    /**
     * @brief Met à jour les poids et le biais (après calcul du delta)
     * @param inputs Valeurs d'entrée utilisées dans forward
     * @param learningRate Taux d'apprentissage
     * @param momentum Facteur de momentum (0 pour désactiver)
     * @param prevWeightUpdates Mises à jour précédentes pour momentum
     */
    void updateWeights(const std::vector<double>& inputs, double learningRate, 
                      double momentum, std::vector<double>& prevWeightUpdates);
    
    /**
     * @brief Obtient le nombre d'entrées
     */
    size_t getNumInputs() const { return weights_.size(); }
    
    /**
     * @brief Obtient la fonction d'activation
     */
    std::shared_ptr<ActivationFunction> getActivation() const { return activation_; }

private:
    std::vector<double> weights_;  // Poids des connexions entrantes
    double bias_;                   // Biais du neurone
    double output_;                 // Dernière sortie calculée
    double netInput_;              // Dernière somme avant activation
    double delta_;                 // Dérivée de l'erreur (pour backpropagation)
    std::shared_ptr<ActivationFunction> activation_; // Fonction d'activation
};

} // namespace NeuroUIT

#endif // NEURON_H


