#include <QApplication>
#include "ui/MainWindow.h"
#include "controller/Controller.h"
#include <memory>
#include <iostream>
#include <exception>

int main(int argc, char *argv[]) {
    try {
        QApplication app(argc, argv);
        
        std::cout << "Initialisation de l'application..." << std::endl;
        
        // Créer le contrôleur
        auto controller = std::make_shared<NeuroUIT::Controller>();
        std::cout << "Contrôleur créé" << std::endl;
        
        // Créer la fenêtre principale
        MainWindow window;
        std::cout << "Fenêtre créée" << std::endl;
        
        window.setController(controller);
        std::cout << "Contrôleur assigné" << std::endl;
        
        // Définir une taille et position par défaut
        window.resize(1200, 800);
        window.move(100, 100);
        
        std::cout << "Affichage de la fenêtre..." << std::endl;
        window.show();
        
        std::cout << "Application lancée avec succès!" << std::endl;
        
        return app.exec();
    } catch (const std::exception& e) {
        std::cerr << "ERREUR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERREUR INCONNUE" << std::endl;
        return 1;
    }
}










