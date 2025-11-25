#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <memory>
#include <string>
#include <vector>
#include <QObject>
#include "core/Network.h"
#include "core/ActivationFunction.h"
#include "dataset/DatasetManager.h"
#include "training/Trainer.h"
#include "persistence/Persistence.h"

namespace NeuroUIT {

/**
 * @brief Contrôleur MVC - coordonne UI et logique métier
 */
class Controller : public QObject {
    Q_OBJECT

public:
    explicit Controller(QObject* parent = nullptr);
    ~Controller();
    
    /**
     * @brief Crée un nouveau réseau
     * @param architecture Architecture du réseau
     * @param activations Fonctions d'activation
     * @return True si créé avec succès
     */
    bool createNetwork(const std::vector<size_t>& architecture,
                      const std::vector<ActivationType>& activations);
    
    /**
     * @brief Charge un réseau depuis un fichier
     * @param filename Nom du fichier
     * @return True si chargé avec succès
     */
    bool loadNetwork(const std::string& filename);
    
    /**
     * @brief Sauvegarde le réseau actuel
     * @param filename Nom du fichier
     * @return True si sauvegardé avec succès
     */
    bool saveNetwork(const std::string& filename);
    
    /**
     * @brief Charge un dataset CSV
     * @param filename Nom du fichier
     * @param numInputs Nombre d'entrées
     * @param numOutputs Nombre de sorties
     * @param hasHeader True si le fichier a des en-têtes
     * @return True si chargé avec succès
     */
    bool loadDataset(const std::string& filename,
                    size_t numInputs,
                    size_t numOutputs,
                    bool hasHeader);
    
    /**
     * @brief Lance l'entraînement
     * @param params Paramètres d'entraînement
     */
    void startTraining(const Trainer::TrainingParams& params);
    
    /**
     * @brief Arrête l'entraînement
     */
    void stopTraining();
    
    /**
     * @brief Teste le réseau sur les données de test
     * @return Erreur moyenne
     */
    double testNetwork();
    
    /**
     * @brief Exporte les résultats d'entraînement
     * @param filename Nom du fichier
     * @return True si exporté avec succès
     */
    bool exportResults(const std::string& filename);
    
    /**
     * @brief Obtient le réseau actuel
     */
    std::shared_ptr<Network> getNetwork() const { return network_; }
    
    /**
     * @brief Obtient les statistiques du dataset
     */
    DatasetManager::Statistics getDatasetStatistics() const;
    
    /**
     * @brief Structure pour les résultats de test complets
     */
    struct TestResults {
        std::vector<double> predictions;           // Prédictions
        std::vector<double> actuals;                // Valeurs réelles
        std::vector<std::vector<int>> confusionMatrix; // Matrice de confusion
        double mse = 0.0;                          // Erreur quadratique moyenne
        double mae = 0.0;                          // Erreur absolue moyenne
        double r2 = 0.0;                           // Coefficient R²
        double accuracy = 0.0;                     // Précision (classification)
        double precision = 0.0;                    // Précision (classification)
        double recall = 0.0;                        // Rappel (classification)
        double f1Score = 0.0;                      // Score F1
        bool isClassification = true;              // Type de problème
    };
    
    /**
     * @brief Teste le réseau et retourne des résultats détaillés
     */
    TestResults getDetailedTestResults();
    
    /**
     * @brief Calcule la matrice de corrélation du dataset
     */
    std::vector<std::vector<double>> getCorrelationMatrix() const;
    
    /**
     * @brief Obtient les erreurs d'entraînement
     */
    const std::vector<double>& getTrainingErrors() const { return trainingErrors_; }

signals:
    /**
     * @brief Signal émis quand l'entraînement progresse
     * @param epoch Époque actuelle
     * @param error Erreur moyenne
     */
    void trainingProgress(size_t epoch, double error);
    
    /**
     * @brief Signal émis quand l'entraînement est terminé
     */
    void trainingFinished();
    
    /**
     * @brief Signal émis quand un réseau est créé
     */
    void networkCreated();
    
    /**
     * @brief Signal émis quand un dataset est chargé
     */
    void datasetLoaded();

private:
    std::shared_ptr<Network> network_;
    std::shared_ptr<Trainer> trainer_;
    DatasetManager datasetManager_;
    std::vector<DatasetManager::Sample> trainSamples_;
    std::vector<DatasetManager::Sample> testSamples_;
    std::vector<double> trainingErrors_;
    
    /**
     * @brief Initialise le trainer si nécessaire
     */
    void initializeTrainer();
};

} // namespace NeuroUIT

#endif // CONTROLLER_H










