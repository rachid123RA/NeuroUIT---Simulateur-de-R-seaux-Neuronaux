#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <string>
#include <memory>
#include "core/Network.h"

namespace NeuroUIT {

/**
 * @brief Gère la sauvegarde et le chargement des réseaux
 */
class Persistence {
public:
    /**
     * @brief Sauvegarde un réseau dans un fichier .nui
     * @param network Réseau à sauvegarder
     * @param filename Nom du fichier
     * @return True si la sauvegarde a réussi
     */
    static bool saveNetwork(std::shared_ptr<Network> network, const std::string& filename);
    
    /**
     * @brief Charge un réseau depuis un fichier .nui
     * @param filename Nom du fichier
     * @return Réseau chargé (nullptr en cas d'erreur)
     */
    static std::shared_ptr<Network> loadNetwork(const std::string& filename);
    
    /**
     * @brief Exporte les résultats dans un fichier CSV
     * @param epochErrors Erreurs par époque
     * @param filename Nom du fichier
     * @return True si l'export a réussi
     */
    static bool exportResultsToCSV(const std::vector<double>& epochErrors, 
                                   const std::string& filename);
    
    /**
     * @brief Version du format de fichier
     */
    static constexpr int FILE_VERSION = 1;

private:
    /**
     * @brief Valide le format d'un fichier .nui
     */
    static bool validateFileFormat(const std::string& filename);
};

} // namespace NeuroUIT

#endif // PERSISTENCE_H










