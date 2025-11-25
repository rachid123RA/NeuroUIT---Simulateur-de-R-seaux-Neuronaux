#ifndef STATISTICSWIDGET_H
#define STATISTICSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QGroupBox>
#include "dataset/DatasetManager.h"

/**
 * @brief Widget pour afficher les statistiques du dataset
 */
class StatisticsWidget : public QWidget {
    Q_OBJECT

public:
    explicit StatisticsWidget(QWidget *parent = nullptr);
    
    /**
     * @brief Met à jour les statistiques affichées
     */
    void updateStatistics(const NeuroUIT::DatasetManager::Statistics& stats);
    
    /**
     * @brief Réinitialise l'affichage
     */
    void clear();

private:
    void setupUI();
    
    QLabel* lblNumSamples_;
    QLabel* lblNumInputs_;
    QLabel* lblNumOutputs_;
    QTableWidget* inputStatsTable_;
    QTableWidget* outputStatsTable_;
    
    QGroupBox* generalGroup_;
    QGroupBox* inputStatsGroup_;
    QGroupBox* outputStatsGroup_;
};

#endif // STATISTICSWIDGET_H

