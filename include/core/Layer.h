#ifndef LAYER_H
#define LAYER_H

#include <vector>
#include <memory>
#include <random>
#include "core/Neuron.h"
#include "core/ActivationFunction.h"

namespace NeuroUIT {

/**
 * @brief Représente une couche de neurones dans un réseau
 */
class Layer {
public:
    /**
     * @brief Constructeur
     * @param numNeurons Nombre de neurones dans la couche
     * @param numInputs Nombre d'entrées pour chaque neurone
     * @param activation Fonction d'activation partagée par tous les neurones
     */
    Layer(size_t numNeurons, size_t numInputs, 
          std::shared_ptr<ActivationFunction> activation);
    
    /**
     * @brief Initialise les poids de tous les neurones
     * @param generator Générateur aléatoire
     * @param mean Moyenne pour l'initialisation
     * @param stddev Écart-type pour l'initialisation
     */
    void initializeWeights(std::mt19937& generator, double mean = 0.0, double stddev = 0.1);
    
    /**
     * @brief Propagation avant (forward pass)
     * @param inputs Valeurs d'entrée
     * @return Sorties de la couche
     */
    std::vector<double> forward(const std::vector<double>& inputs);
    
    /**
     * @brief Rétropropagation (backward pass)
     * @param errors Erreurs de la couche suivante
     * @param prevLayerOutputs Sorties de la couche précédente
     */
    void backward(const std::vector<double>& errors, const std::vector<double>& prevLayerOutputs);
    
    /**
     * @brief Calcule les erreurs à propager vers la couche précédente
     * @return Erreurs pour la couche précédente
     */
    std::vector<double> getErrorsForPrevLayer() const;
    
    /**
     * @brief Met à jour les poids de tous les neurones
     * @param learningRate Taux d'apprentissage
     * @param momentum Facteur de momentum
     */
    void updateWeights(double learningRate, double momentum = 0.0);
    
    /**
     * @brief Définit les sorties de la couche (pour la couche d'entrée)
     * @param outputs Sorties à définir
     */
    void setOutputs(const std::vector<double>& outputs);
    
    /**
     * @brief Obtient les neurones de la couche
     */
    const std::vector<Neuron>& getNeurons() const { return neurons_; }
    std::vector<Neuron>& getNeurons() { return neurons_; }
    
    /**
     * @brief Obtient les sorties de la couche
     */
    const std::vector<double>& getOutputs() const { return outputs_; }
    
    /**
     * @brief Obtient le nombre de neurones
     */
    size_t getNumNeurons() const { return neurons_.size(); }
    
    /**
     * @brief Obtient le nombre d'entrées
     */
    size_t getNumInputs() const { return numInputs_; }

private:
    std::vector<Neuron> neurons_;              // Neurones de la couche
    size_t numInputs_;                         // Nombre d'entrées
    std::vector<double> outputs_;              // Sorties de la couche
    std::vector<double> prevInputs_;            // Entrées précédentes (pour mise à jour des poids)
    std::vector<std::vector<double>> prevWeightUpdates_; // Mises à jour précédentes (pour momentum)
};

} // namespace NeuroUIT

#endif // LAYER_H



