#include "ui/MetricsWidget.h"
#include <QFormLayout>
#include <QFont>

MetricsWidget::MetricsWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void MetricsWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    
    // === Métriques de Classification ===
    classificationGroup_ = new QGroupBox("📊 Métriques de Classification");
    QFormLayout* classLayout = new QFormLayout(classificationGroup_);
    
    lblAccuracy_ = new QLabel("-");
    lblPrecision_ = new QLabel("-");
    lblRecall_ = new QLabel("-");
    lblF1Score_ = new QLabel("-");
    
    classLayout->addRow("Précision (Accuracy):", lblAccuracy_);
    classLayout->addRow("Précision (Precision):", lblPrecision_);
    classLayout->addRow("Rappel (Recall):", lblRecall_);
    classLayout->addRow("Score F1:", lblF1Score_);
    
    mainLayout->addWidget(classificationGroup_);
    
    // === Métriques de Régression ===
    regressionGroup_ = new QGroupBox("📈 Métriques de Régression");
    QFormLayout* regLayout = new QFormLayout(regressionGroup_);
    
    lblMSE_ = new QLabel("-");
    lblMAE_ = new QLabel("-");
    lblR2_ = new QLabel("-");
    
    regLayout->addRow("MSE (Erreur Quadratique Moyenne):", lblMSE_);
    regLayout->addRow("MAE (Erreur Absolue Moyenne):", lblMAE_);
    regLayout->addRow("R² (Coefficient de Détermination):", lblR2_);
    
    mainLayout->addWidget(regressionGroup_);
    
    // === Informations d'Entraînement ===
    trainingGroup_ = new QGroupBox("⏱️ Informations d'Entraînement");
    QFormLayout* trainLayout = new QFormLayout(trainingGroup_);
    
    lblMinError_ = new QLabel("-");
    lblFinalError_ = new QLabel("-");
    lblEpochs_ = new QLabel("-");
    lblTrainingTime_ = new QLabel("-");
    
    trainLayout->addRow("Erreur minimale:", lblMinError_);
    trainLayout->addRow("Erreur finale:", lblFinalError_);
    trainLayout->addRow("Époques complétées:", lblEpochs_);
    trainLayout->addRow("Temps d'entraînement:", lblTrainingTime_);
    
    mainLayout->addWidget(trainingGroup_);
    
    mainLayout->addStretch();
    
    // Style des labels de valeurs
    QFont valueFont("Consolas", 10);
    lblAccuracy_->setFont(valueFont);
    lblPrecision_->setFont(valueFont);
    lblRecall_->setFont(valueFont);
    lblF1Score_->setFont(valueFont);
    lblMSE_->setFont(valueFont);
    lblMAE_->setFont(valueFont);
    lblR2_->setFont(valueFont);
    lblMinError_->setFont(valueFont);
    lblFinalError_->setFont(valueFont);
    lblEpochs_->setFont(valueFont);
    lblTrainingTime_->setFont(valueFont);
}

void MetricsWidget::updateMetrics(const Metrics& metrics) {
    if (metrics.isClassification) {
        classificationGroup_->setVisible(true);
        regressionGroup_->setVisible(false);
        
        lblAccuracy_->setText(QString("%1%").arg(metrics.accuracy * 100, 0, 'f', 2));
        lblPrecision_->setText(QString("%1").arg(metrics.precision, 0, 'f', 4));
        lblRecall_->setText(QString("%1").arg(metrics.recall, 0, 'f', 4));
        lblF1Score_->setText(QString("%1").arg(metrics.f1Score, 0, 'f', 4));
    } else {
        classificationGroup_->setVisible(false);
        regressionGroup_->setVisible(true);
        
        lblMSE_->setText(QString("%1").arg(metrics.mse, 0, 'f', 6));
        lblMAE_->setText(QString("%1").arg(metrics.mae, 0, 'f', 6));
        lblR2_->setText(QString("%1").arg(metrics.r2, 0, 'f', 4));
    }
    
    lblMinError_->setText(QString("%1").arg(metrics.minError, 0, 'f', 6));
    lblFinalError_->setText(QString("%1").arg(metrics.finalError, 0, 'f', 6));
    lblEpochs_->setText(QString::number(metrics.epochsCompleted));
    
    // Format du temps
    if (metrics.trainingTime < 60) {
        lblTrainingTime_->setText(QString("%1 s").arg(metrics.trainingTime, 0, 'f', 2));
    } else if (metrics.trainingTime < 3600) {
        lblTrainingTime_->setText(QString("%1 min %2 s")
            .arg(static_cast<int>(metrics.trainingTime / 60))
            .arg(static_cast<int>(metrics.trainingTime) % 60));
    } else {
        int hours = static_cast<int>(metrics.trainingTime / 3600);
        int minutes = static_cast<int>((metrics.trainingTime - hours * 3600) / 60);
        int seconds = static_cast<int>(metrics.trainingTime) % 60;
        lblTrainingTime_->setText(QString("%1 h %2 min %3 s")
            .arg(hours).arg(minutes).arg(seconds));
    }
}

void MetricsWidget::clear() {
    lblAccuracy_->setText("-");
    lblPrecision_->setText("-");
    lblRecall_->setText("-");
    lblF1Score_->setText("-");
    lblMSE_->setText("-");
    lblMAE_->setText("-");
    lblR2_->setText("-");
    lblMinError_->setText("-");
    lblFinalError_->setText("-");
    lblEpochs_->setText("-");
    lblTrainingTime_->setText("-");
}

