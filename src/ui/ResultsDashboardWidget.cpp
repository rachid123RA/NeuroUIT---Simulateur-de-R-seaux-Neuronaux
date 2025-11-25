#include "ui/ResultsDashboardWidget.h"
#include "controller/Controller.h"
#include "core/Network.h"
#include "core/ActivationFunction.h"
#include <QFrame>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QBrush>
#include <QColor>
#include <QHeaderView>
#include <QFont>
#include <QFontMetrics>
#include <algorithm>
#include <iomanip>
#include <sstream>

ResultsDashboardWidget::ResultsDashboardWidget(QWidget *parent)
    : QWidget(parent)
    , hasResults_(false)
{
    setupUI();
}

void ResultsDashboardWidget::setupUI() {
    QVBoxLayout* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(10, 10, 10, 10);
    outerLayout->setSpacing(0);
    
    // ScrollArea pour permettre le défilement
    scrollArea_ = new QScrollArea(this);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setFrameShape(QFrame::NoFrame);
    scrollArea_->setStyleSheet(
        "QScrollArea { border: none; background-color: transparent; }"
        "QScrollBar:vertical { background: #f0f0f0; width: 12px; border-radius: 6px; }"
        "QScrollBar::handle:vertical { background: #c0c0c0; min-height: 20px; border-radius: 6px; }"
        "QScrollBar::handle:vertical:hover { background: #a0a0a0; }"
    );
    
    contentWidget_ = new QWidget;
    mainLayout_ = new QVBoxLayout(contentWidget_);
    mainLayout_->setSpacing(20);
    mainLayout_->setContentsMargins(20, 20, 20, 20);
    
    setupNetworkStatsSection();
    setupPerformanceSection();
    setupClassificationResultsSection();
    setupRegressionResultsSection();
    setupTrainingHistorySection();
    
    mainLayout_->addStretch();
    
    scrollArea_->setWidget(contentWidget_);
    outerLayout->addWidget(scrollArea_);
    
    // Appliquer les styles modernes
    QString groupBoxStyle = 
        "QGroupBox { "
        "font-weight: bold; "
        "font-size: 11pt; "
        "border: 2px solid #2196F3; "
        "border-radius: 8px; "
        "margin-top: 15px; "
        "padding-top: 15px; "
        "background-color: #ffffff; "
        "}"
        "QGroupBox::title { "
        "subcontrol-origin: margin; "
        "left: 15px; "
        "padding: 0 8px; "
        "color: #2196F3; "
        "}";
    
    networkStatsGroup_->setStyleSheet(groupBoxStyle);
    performanceGroup_->setStyleSheet(groupBoxStyle);
    classificationResultsGroup_->setStyleSheet(groupBoxStyle);
    regressionResultsGroup_->setStyleSheet(groupBoxStyle);
    trainingHistoryGroup_->setStyleSheet(groupBoxStyle);
    
    // Style pour les tableaux
    QString tableStyle =
        "QTableWidget { "
        "border: 1px solid #e0e0e0; "
        "border-radius: 5px; "
        "background-color: #ffffff; "
        "gridline-color: #f0f0f0; "
        "}"
        "QTableWidget::item { "
        "padding: 5px; "
        "}"
        "QTableWidget::item:selected { "
        "background-color: #E3F2FD; "
        "color: #1976D2; "
        "}"
        "QHeaderView::section { "
        "background-color: #2196F3; "
        "color: white; "
        "padding: 8px; "
        "border: none; "
        "font-weight: bold; "
        "}";
    
    classificationTable_->setStyleSheet(tableStyle);
    regressionTable_->setStyleSheet(tableStyle);
    
    // Style pour les ProgressBar
    QString progressBarStyle =
        "QProgressBar { "
        "border: 1px solid #e0e0e0; "
        "border-radius: 5px; "
        "background-color: #f5f5f5; "
        "text-align: center; "
        "height: 20px; "
        "}"
        "QProgressBar::chunk { "
        "background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #2196F3, stop:1 #1976D2); "
        "border-radius: 4px; "
        "}";
    
    accuracyBar_->setStyleSheet(progressBarStyle);
    precisionBar_->setStyleSheet(progressBarStyle);
    recallBar_->setStyleSheet(progressBarStyle);
}

void ResultsDashboardWidget::setupNetworkStatsSection() {
    networkStatsGroup_ = new QGroupBox("Statistiques du Reseau");
    QGridLayout* layout = new QGridLayout(networkStatsGroup_);
    layout->setSpacing(10);
    
    QLabel* lblArchTitle = new QLabel("Architecture:");
    lblArchTitle->setStyleSheet("font-weight: bold;");
    lblArchitecture_ = new QLabel("-");
    lblArchitecture_->setWordWrap(true);
    
    QLabel* lblNeuronsTitle = new QLabel("Nombre total de neurones:");
    lblNeuronsTitle->setStyleSheet("font-weight: bold;");
    lblTotalNeurons_ = new QLabel("-");
    
    QLabel* lblConnectionsTitle = new QLabel("Nombre total de connexions:");
    lblConnectionsTitle->setStyleSheet("font-weight: bold;");
    lblTotalConnections_ = new QLabel("-");
    
    QLabel* lblActivationsTitle = new QLabel("Fonctions d'activation:");
    lblActivationsTitle->setStyleSheet("font-weight: bold;");
    lblActivationFunctions_ = new QLabel("-");
    lblActivationFunctions_->setWordWrap(true);
    
    layout->addWidget(lblArchTitle, 0, 0);
    layout->addWidget(lblArchitecture_, 0, 1);
    layout->addWidget(lblNeuronsTitle, 1, 0);
    layout->addWidget(lblTotalNeurons_, 1, 1);
    layout->addWidget(lblConnectionsTitle, 2, 0);
    layout->addWidget(lblTotalConnections_, 2, 1);
    layout->addWidget(lblActivationsTitle, 3, 0);
    layout->addWidget(lblActivationFunctions_, 3, 1);
    
    mainLayout_->addWidget(networkStatsGroup_);
}

void ResultsDashboardWidget::setupPerformanceSection() {
    performanceGroup_ = new QGroupBox("Performance du Modele");
    QGridLayout* layout = new QGridLayout(performanceGroup_);
    layout->setSpacing(10);
    
    // Métriques de classification
    QLabel* lblAccTitle = new QLabel("Precision (Accuracy):");
    lblAccTitle->setStyleSheet("font-weight: bold;");
    lblAccuracy_ = new QLabel("-");
    accuracyBar_ = new QProgressBar;
    accuracyBar_->setMaximum(100);
    accuracyBar_->setTextVisible(true);
    accuracyBar_->setFormat("%p%");
    
    QLabel* lblPrecTitle = new QLabel("Precision:");
    lblPrecTitle->setStyleSheet("font-weight: bold;");
    lblPrecision_ = new QLabel("-");
    precisionBar_ = new QProgressBar;
    precisionBar_->setMaximum(100);
    precisionBar_->setTextVisible(true);
    precisionBar_->setFormat("%p%");
    
    QLabel* lblRecTitle = new QLabel("Rappel (Recall):");
    lblRecTitle->setStyleSheet("font-weight: bold;");
    lblRecall_ = new QLabel("-");
    recallBar_ = new QProgressBar;
    recallBar_->setMaximum(100);
    recallBar_->setTextVisible(true);
    recallBar_->setFormat("%p%");
    
    QLabel* lblF1Title = new QLabel("Score F1:");
    lblF1Title->setStyleSheet("font-weight: bold;");
    lblF1Score_ = new QLabel("-");
    
    // Métriques de régression
    QLabel* lblMSETitle = new QLabel("MSE (Erreur Quadratique Moyenne):");
    lblMSETitle->setStyleSheet("font-weight: bold;");
    lblMSE_ = new QLabel("-");
    
    QLabel* lblMAETitle = new QLabel("MAE (Erreur Absolue Moyenne):");
    lblMAETitle->setStyleSheet("font-weight: bold;");
    lblMAE_ = new QLabel("-");
    
    QLabel* lblR2Title = new QLabel("R2 (Coefficient de Determination):");
    lblR2Title->setStyleSheet("font-weight: bold;");
    lblR2_ = new QLabel("-");
    
    int row = 0;
    layout->addWidget(lblAccTitle, row, 0);
    layout->addWidget(lblAccuracy_, row, 1);
    layout->addWidget(accuracyBar_, row, 2);
    row++;
    
    layout->addWidget(lblPrecTitle, row, 0);
    layout->addWidget(lblPrecision_, row, 1);
    layout->addWidget(precisionBar_, row, 2);
    row++;
    
    layout->addWidget(lblRecTitle, row, 0);
    layout->addWidget(lblRecall_, row, 1);
    layout->addWidget(recallBar_, row, 2);
    row++;
    
    layout->addWidget(lblF1Title, row, 0);
    layout->addWidget(lblF1Score_, row, 1);
    row++;
    
    layout->addWidget(lblMSETitle, row, 0);
    layout->addWidget(lblMSE_, row, 1);
    row++;
    
    layout->addWidget(lblMAETitle, row, 0);
    layout->addWidget(lblMAE_, row, 1);
    row++;
    
    layout->addWidget(lblR2Title, row, 0);
    layout->addWidget(lblR2_, row, 1);
    
    mainLayout_->addWidget(performanceGroup_);
}

void ResultsDashboardWidget::setupClassificationResultsSection() {
    classificationResultsGroup_ = new QGroupBox("Resultats de Classification");
    QVBoxLayout* layout = new QVBoxLayout(classificationResultsGroup_);
    
    classificationTable_ = new QTableWidget;
    classificationTable_->setColumnCount(4);
    classificationTable_->setHorizontalHeaderLabels({"Echantillon", "Valeur Reelle", "Prediction", "Statut"});
    classificationTable_->horizontalHeader()->setStretchLastSection(true);
    classificationTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    classificationTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    classificationTable_->setAlternatingRowColors(true);
    classificationTable_->setMaximumHeight(300);
    
    layout->addWidget(classificationTable_);
    mainLayout_->addWidget(classificationResultsGroup_);
}

void ResultsDashboardWidget::setupRegressionResultsSection() {
    regressionResultsGroup_ = new QGroupBox("Resultats de Regression");
    QVBoxLayout* layout = new QVBoxLayout(regressionResultsGroup_);
    
    regressionTable_ = new QTableWidget;
    regressionTable_->setColumnCount(5);
    regressionTable_->setHorizontalHeaderLabels({"Echantillon", "Valeur Reelle", "Prediction", "Erreur", "Erreur %"});
    regressionTable_->horizontalHeader()->setStretchLastSection(true);
    regressionTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    regressionTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    regressionTable_->setAlternatingRowColors(true);
    regressionTable_->setMaximumHeight(300);
    
    layout->addWidget(regressionTable_);
    mainLayout_->addWidget(regressionResultsGroup_);
}

void ResultsDashboardWidget::setupTrainingHistorySection() {
    trainingHistoryGroup_ = new QGroupBox("Historique d'Entrainement");
    QGridLayout* layout = new QGridLayout(trainingHistoryGroup_);
    layout->setSpacing(10);
    
    QLabel* lblEpochsTitle = new QLabel("Epoques completees:");
    lblEpochsTitle->setStyleSheet("font-weight: bold;");
    lblEpochs_ = new QLabel("-");
    
    QLabel* lblFinalErrorTitle = new QLabel("Erreur finale:");
    lblFinalErrorTitle->setStyleSheet("font-weight: bold;");
    lblFinalError_ = new QLabel("-");
    
    QLabel* lblMinErrorTitle = new QLabel("Erreur minimale:");
    lblMinErrorTitle->setStyleSheet("font-weight: bold;");
    lblMinError_ = new QLabel("-");
    
    QLabel* lblTimeTitle = new QLabel("Temps d'entrainement:");
    lblTimeTitle->setStyleSheet("font-weight: bold;");
    lblTrainingTime_ = new QLabel("-");
    
    layout->addWidget(lblEpochsTitle, 0, 0);
    layout->addWidget(lblEpochs_, 0, 1);
    layout->addWidget(lblFinalErrorTitle, 1, 0);
    layout->addWidget(lblFinalError_, 1, 1);
    layout->addWidget(lblMinErrorTitle, 2, 0);
    layout->addWidget(lblMinError_, 2, 1);
    layout->addWidget(lblTimeTitle, 3, 0);
    layout->addWidget(lblTrainingTime_, 3, 1);
    
    mainLayout_->addWidget(trainingHistoryGroup_);
}

void ResultsDashboardWidget::updateResults(std::shared_ptr<NeuroUIT::Controller> controller) {
    if (!controller) {
        clear();
        return;
    }
    
    auto network = controller->getNetwork();
    if (!network) {
        clear();
        return;
    }
    
    // Statistiques du réseau
    auto architecture = network->getArchitecture();
    QString archText;
    for (size_t i = 0; i < architecture.size(); ++i) {
        if (i > 0) archText += " -> ";
        archText += QString::number(architecture[i]);
    }
    lblArchitecture_->setText(archText);
    
    size_t totalNeurons = 0;
    size_t totalConnections = 0;
    for (size_t i = 0; i < architecture.size(); ++i) {
        totalNeurons += architecture[i];
        if (i > 0) {
            totalConnections += architecture[i-1] * architecture[i];
        }
    }
    lblTotalNeurons_->setText(QString::number(totalNeurons));
    lblTotalConnections_->setText(QString::number(totalConnections));
    
    auto activations = network->getActivationTypes();
    QString activationsText;
    for (size_t i = 0; i < activations.size(); ++i) {
        if (i > 0) activationsText += ", ";
        switch (activations[i]) {
            case NeuroUIT::ActivationType::Sigmoid:
                activationsText += "Sigmoid";
                break;
            case NeuroUIT::ActivationType::Tanh:
                activationsText += "Tanh";
                break;
            case NeuroUIT::ActivationType::ReLU:
                activationsText += "ReLU";
                break;
            case NeuroUIT::ActivationType::Linear:
                activationsText += "Linear";
                break;
        }
    }
    lblActivationFunctions_->setText(activationsText);
    
    // Résultats de test
    auto testResults = controller->getDetailedTestResults();
    
    if (testResults.isClassification) {
        classificationResultsGroup_->setVisible(true);
        regressionResultsGroup_->setVisible(false);
        
        // Métriques de classification
        lblAccuracy_->setText(QString("%1%").arg(testResults.accuracy * 100, 0, 'f', 2));
        accuracyBar_->setValue(static_cast<int>(testResults.accuracy * 100));
        
        lblPrecision_->setText(QString("%1").arg(testResults.precision, 0, 'f', 4));
        precisionBar_->setValue(static_cast<int>(testResults.precision * 100));
        
        lblRecall_->setText(QString("%1").arg(testResults.recall, 0, 'f', 4));
        recallBar_->setValue(static_cast<int>(testResults.recall * 100));
        
        lblF1Score_->setText(QString("%1").arg(testResults.f1Score, 0, 'f', 4));
        
        // Remplir le tableau de classification
        size_t numSamples = std::min(testResults.predictions.size(), testResults.actuals.size());
        classificationTable_->setRowCount(static_cast<int>(numSamples));
        
        int correctCount = 0;
        for (size_t i = 0; i < numSamples; ++i) {
            int predicted = (testResults.predictions[i] >= 0.5) ? 1 : 0;
            int actual = (testResults.actuals[i] >= 0.5) ? 1 : 0;
            bool isCorrect = (predicted == actual);
            if (isCorrect) correctCount++;
            
            classificationTable_->setItem(static_cast<int>(i), 0, 
                new QTableWidgetItem(QString("Echantillon %1").arg(i + 1)));
            classificationTable_->setItem(static_cast<int>(i), 1, 
                new QTableWidgetItem(QString::number(actual)));
            classificationTable_->setItem(static_cast<int>(i), 2, 
                new QTableWidgetItem(QString::number(predicted)));
            
            QTableWidgetItem* statusItem = new QTableWidgetItem(isCorrect ? "Correct" : "Incorrect");
            statusItem->setForeground(isCorrect ? QBrush(QColor(0, 150, 0)) : QBrush(QColor(200, 0, 0)));
            classificationTable_->setItem(static_cast<int>(i), 3, statusItem);
        }
        classificationTable_->resizeColumnsToContents();
        
    } else {
        classificationResultsGroup_->setVisible(false);
        regressionResultsGroup_->setVisible(true);
        
        // Métriques de régression
        lblMSE_->setText(QString("%1").arg(testResults.mse, 0, 'f', 6));
        lblMAE_->setText(QString("%1").arg(testResults.mae, 0, 'f', 6));
        lblR2_->setText(QString("%1").arg(testResults.r2, 0, 'f', 4));
        
        // Remplir le tableau de régression
        size_t numSamples = std::min(testResults.predictions.size(), testResults.actuals.size());
        regressionTable_->setRowCount(static_cast<int>(numSamples));
        
        for (size_t i = 0; i < numSamples; ++i) {
            double error = testResults.predictions[i] - testResults.actuals[i];
            double errorPercent = testResults.actuals[i] != 0.0 ? 
                (error / testResults.actuals[i]) * 100.0 : 0.0;
            
            regressionTable_->setItem(static_cast<int>(i), 0, 
                new QTableWidgetItem(QString("Echantillon %1").arg(i + 1)));
            regressionTable_->setItem(static_cast<int>(i), 1, 
                new QTableWidgetItem(QString::number(testResults.actuals[i], 'f', 4)));
            regressionTable_->setItem(static_cast<int>(i), 2, 
                new QTableWidgetItem(QString::number(testResults.predictions[i], 'f', 4)));
            regressionTable_->setItem(static_cast<int>(i), 3, 
                new QTableWidgetItem(QString::number(error, 'f', 4)));
            regressionTable_->setItem(static_cast<int>(i), 4, 
                new QTableWidgetItem(QString("%1%").arg(errorPercent, 0, 'f', 2)));
        }
        regressionTable_->resizeColumnsToContents();
    }
    
    // Historique d'entraînement
    auto trainingErrors = controller->getTrainingErrors();
    if (!trainingErrors.empty()) {
        lblEpochs_->setText(QString::number(trainingErrors.size()));
        lblFinalError_->setText(QString::number(trainingErrors.back(), 'f', 6));
        lblMinError_->setText(QString::number(*std::min_element(trainingErrors.begin(), trainingErrors.end()), 'f', 6));
        lblTrainingTime_->setText("N/A"); // À implémenter si nécessaire
    }
    
    hasResults_ = true;
}

void ResultsDashboardWidget::clear() {
    lblArchitecture_->setText("-");
    lblTotalNeurons_->setText("-");
    lblTotalConnections_->setText("-");
    lblActivationFunctions_->setText("-");
    
    lblAccuracy_->setText("-");
    lblPrecision_->setText("-");
    lblRecall_->setText("-");
    lblF1Score_->setText("-");
    lblMSE_->setText("-");
    lblMAE_->setText("-");
    lblR2_->setText("-");
    
    accuracyBar_->setValue(0);
    precisionBar_->setValue(0);
    recallBar_->setValue(0);
    
    classificationTable_->setRowCount(0);
    regressionTable_->setRowCount(0);
    
    lblEpochs_->setText("-");
    lblFinalError_->setText("-");
    lblMinError_->setText("-");
    lblTrainingTime_->setText("-");
    
    hasResults_ = false;
}

