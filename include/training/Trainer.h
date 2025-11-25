#ifndef TRAINER_H
#define TRAINER_H

#include <memory>
#include <vector>
#include <functional>
#include "core/Network.h"
#include "dataset/DatasetManager.h"

namespace NeuroUIT {

/**
 * @brief Gère l'entraînement du réseau de neurones
 */
class Trainer {
public:
    /**
     * @brief Callback pour la progression de l'entraînement
     * @param epoch Époque actuelle
     * @param error Erreur moyenne sur cette époque
     */
    using ProgressCallback = std::function<void(size_t epoch, double error)>;
    
    /**
     * @brief Paramètres d'entraînement
     */
    struct TrainingParams {
        double learningRate = 0.01;      // Taux d'apprentissage
        size_t numEpochs = 100;          // Nombre d'époques
        size_t batchSize = 1;             // Taille du batch (1 = descente stochastique)
        double momentum = 0.0;           // Facteur de momentum
        bool shuffle = true;             // Mélanger les échantillons à chaque époque
    };
    
    /**
     * @brief Résultats d'entraînement
     */
    struct TrainingResults {
        std::vector<double> epochErrors;  // Erreur par époque
        size_t epochsCompleted = 0;      // Nombre d'époques complétées
        double finalError = 0.0;          // Erreur finale
        bool stopped = false;             // True si arrêté par l'utilisateur
    };
    
    /**
     * @brief Constructeur
     * @param network Réseau à entraîner
     */
    Trainer(std::shared_ptr<Network> network);
    
    /**
     * @brief Entraîne le réseau sur les données fournies
     * @param trainSamples Échantillons d'entraînement
     * @param params Paramètres d'entraînement
     * @param callback Callback appelé à chaque époque
     * @return Résultats d'entraînement
     */
    TrainingResults train(const std::vector<DatasetManager::Sample>& trainSamples,
                         const TrainingParams& params,
                         ProgressCallback callback = nullptr);
    
    /**
     * @brief Teste le réseau sur des échantillons de test
     * @param testSamples Échantillons de test
     * @return Erreur moyenne
     */
    double test(const std::vector<DatasetManager::Sample>& testSamples);
    
    /**
     * @brief Arrête l'entraînement en cours (thread-safe)
     */
    void stop();
    
    /**
     * @brief Vérifie si l'entraînement est en cours
     */
    bool isTraining() const { return isTraining_; }

private:
    std::shared_ptr<Network> network_;
    bool isTraining_;
    bool shouldStop_;
    
    /**
     * @brief Entraîne une époque
     * @param trainSamples Échantillons d'entraînement
     * @param params Paramètres d'entraînement
     * @return Erreur moyenne sur l'époque
     */
    double trainEpoch(const std::vector<DatasetManager::Sample>& trainSamples,
                     const TrainingParams& params);
    
    /**
     * @brief Traite un batch
     * @param batchSamples Échantillons du batch
     * @param params Paramètres d'entraînement
     */
    void processBatch(const std::vector<DatasetManager::Sample>& batchSamples,
                     const TrainingParams& params);
};

} // namespace NeuroUIT

#endif // TRAINER_H










