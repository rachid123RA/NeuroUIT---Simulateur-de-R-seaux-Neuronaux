#ifndef RESULTSDASHBOARDWIDGET_H
#define RESULTSDASHBOARDWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QTableWidget>
#include <QGroupBox>
#include <QScrollArea>
#include <QProgressBar>
#include <vector>

namespace NeuroUIT {
    class Controller;
}

/**
 * @brief Widget principal pour afficher le dashboard de résultats
 */
class ResultsDashboardWidget : public QWidget {
    Q_OBJECT

public:
    explicit ResultsDashboardWidget(QWidget *parent = nullptr);
    
    /**
     * @brief Met à jour le dashboard avec les résultats du test
     */
    void updateResults(std::shared_ptr<NeuroUIT::Controller> controller);
    
    /**
     * @brief Réinitialise l'affichage
     */
    void clear();

private:
    void setupUI();
    void setupNetworkStatsSection();
    void setupPerformanceSection();
    void setupClassificationResultsSection();
    void setupRegressionResultsSection();
    void setupTrainingHistorySection();
    
    // Sections principales
    QScrollArea* scrollArea_;
    QWidget* contentWidget_;
    QVBoxLayout* mainLayout_;
    
    // Statistiques du réseau
    QGroupBox* networkStatsGroup_;
    QLabel* lblArchitecture_;
    QLabel* lblTotalNeurons_;
    QLabel* lblTotalConnections_;
    QLabel* lblActivationFunctions_;
    
    // Performance
    QGroupBox* performanceGroup_;
    QLabel* lblAccuracy_;
    QLabel* lblPrecision_;
    QLabel* lblRecall_;
    QLabel* lblF1Score_;
    QLabel* lblMSE_;
    QLabel* lblMAE_;
    QLabel* lblR2_;
    QProgressBar* accuracyBar_;
    QProgressBar* precisionBar_;
    QProgressBar* recallBar_;
    
    // Résultats de classification
    QGroupBox* classificationResultsGroup_;
    QTableWidget* classificationTable_;
    
    // Résultats de régression
    QGroupBox* regressionResultsGroup_;
    QTableWidget* regressionTable_;
    
    // Historique d'entraînement
    QGroupBox* trainingHistoryGroup_;
    QLabel* lblEpochs_;
    QLabel* lblFinalError_;
    QLabel* lblMinError_;
    QLabel* lblTrainingTime_;
    
    bool hasResults_;
};

#endif // RESULTSDASHBOARDWIDGET_H

