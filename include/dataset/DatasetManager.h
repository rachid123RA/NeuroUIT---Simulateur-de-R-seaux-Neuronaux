#ifndef DATASETMANAGER_H
#define DATASETMANAGER_H

#include <vector>
#include <string>
#include <utility>

namespace NeuroUIT {

/**
 * @brief Gère le chargement et la manipulation de datasets
 */
class DatasetManager {
public:
    /**
     * @brief Structure représentant un échantillon (entrées + sorties)
     */
    struct Sample {
        std::vector<double> inputs;   // Valeurs d'entrée
        std::vector<double> outputs;   // Valeurs de sortie (cibles)
    };
    
    /**
     * @brief Statistiques sur le dataset
     */
    struct Statistics {
        size_t numSamples = 0;        // Nombre d'échantillons
        size_t numInputs = 0;          // Nombre d'entrées
        size_t numOutputs = 0;         // Nombre de sorties
        
        // Statistiques sur les entrées
        std::vector<double> inputMin;  // Valeurs minimales par entrée
        std::vector<double> inputMax;  // Valeurs maximales par entrée
        std::vector<double> inputMean;  // Valeurs moyennes par entrée
        
        // Statistiques sur les sorties
        std::vector<double> outputMin; // Valeurs minimales par sortie
        std::vector<double> outputMax; // Valeurs maximales par sortie
        std::vector<double> outputMean;// Valeurs moyennes par sortie
    };
    
    /**
     * @brief Constructeur par défaut
     */
    DatasetManager();
    
    /**
     * @brief Charge un dataset depuis un fichier CSV
     * @param filename Nom du fichier CSV
     * @param numInputs Nombre de colonnes d'entrée
     * @param numOutputs Nombre de colonnes de sortie
     * @param hasHeader True si la première ligne contient des en-têtes
     * @return True si chargé avec succès
     */
    bool loadFromCSV(const std::string& filename, 
                    size_t numInputs, 
                    size_t numOutputs,
                    bool hasHeader = false);
    
    /**
     * @brief Divise le dataset en ensembles d'entraînement et de test
     * @param trainRatio Proportion pour l'entraînement (0.0 à 1.0)
     * @return Paire (trainSamples, testSamples)
     */
    std::pair<std::vector<Sample>, std::vector<Sample>> 
    splitTrainTest(double trainRatio = 0.8);
    
    /**
     * @brief Obtient toutes les statistiques du dataset
     * @return Structure Statistics
     */
    Statistics getStatistics() const;
    
    /**
     * @brief Vide le dataset
     */
    void clear();
    
    /**
     * @brief Obtient tous les échantillons
     */
    const std::vector<Sample>& getSamples() const { return samples_; }
    
    /**
     * @brief Obtient le nombre d'entrées
     */
    size_t getNumInputs() const { return numInputs_; }
    
    /**
     * @brief Obtient le nombre de sorties
     */
    size_t getNumOutputs() const { return numOutputs_; }
    
    /**
     * @brief Vérifie si le dataset est vide
     */
    bool isEmpty() const { return samples_.empty(); }
    
    /**
     * @brief Obtient le nombre d'échantillons
     */
    size_t getNumSamples() const { return samples_.size(); }

private:
    std::vector<Sample> samples_;  // Tous les échantillons
    size_t numInputs_;             // Nombre d'entrées par échantillon
    size_t numOutputs_;             // Nombre de sorties par échantillon
    
    /**
     * @brief Parse une ligne CSV en vecteur de doubles
     * @param line Ligne CSV
     * @return Vecteur de valeurs
     */
    std::vector<double> parseCSVLine(const std::string& line);
    
    /**
     * @brief Valide un échantillon
     * @param sample Échantillon à valider
     * @return True si valide
     */
    bool validateSample(const Sample& sample) const;
};

} // namespace NeuroUIT

#endif // DATASETMANAGER_H


