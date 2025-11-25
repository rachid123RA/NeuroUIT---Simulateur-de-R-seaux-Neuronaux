#ifndef METRICSWIDGET_H
#define METRICSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <vector>

/**
 * @brief Widget pour afficher les métriques de performance du réseau
 */
class MetricsWidget : public QWidget {
    Q_OBJECT

public:
    explicit MetricsWidget(QWidget *parent = nullptr);
    
    /**
     * @brief Structure pour les métriques
     */
    struct Metrics {
        double accuracy = 0.0;      // Précision (classification)
        double precision = 0.0;      // Précision (classification)
        double recall = 0.0;         // Rappel (classification)
        double f1Score = 0.0;        // Score F1 (classification)
        double mse = 0.0;            // Erreur quadratique moyenne (régression)
        double mae = 0.0;            // Erreur absolue moyenne (régression)
        double r2 = 0.0;             // Coefficient de détermination R² (régression)
        double minError = 0.0;       // Erreur minimale atteinte
        double finalError = 0.0;     // Erreur finale
        size_t epochsCompleted = 0;  // Nombre d'époques complétées
        double trainingTime = 0.0;   // Temps d'entraînement en secondes
        bool isClassification = true; // Type de problème
    };
    
    /**
     * @brief Met à jour les métriques affichées
     */
    void updateMetrics(const Metrics& metrics);
    
    /**
     * @brief Réinitialise l'affichage
     */
    void clear();

private:
    void setupUI();
    
    // Labels pour les métriques
    QLabel* lblAccuracy_;
    QLabel* lblPrecision_;
    QLabel* lblRecall_;
    QLabel* lblF1Score_;
    QLabel* lblMSE_;
    QLabel* lblMAE_;
    QLabel* lblR2_;
    QLabel* lblMinError_;
    QLabel* lblFinalError_;
    QLabel* lblEpochs_;
    QLabel* lblTrainingTime_;
    
    QGroupBox* classificationGroup_;
    QGroupBox* regressionGroup_;
    QGroupBox* trainingGroup_;
};

#endif // METRICSWIDGET_H

