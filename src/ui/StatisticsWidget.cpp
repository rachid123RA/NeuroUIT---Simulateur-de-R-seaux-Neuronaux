#include "ui/StatisticsWidget.h"
#include <QFormLayout>
#include <QHeaderView>
#include <QFont>
#include <QTableWidgetItem>

StatisticsWidget::StatisticsWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void StatisticsWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    
    // === Informations générales ===
    generalGroup_ = new QGroupBox("📊 Informations Générales");
    QFormLayout* generalLayout = new QFormLayout(generalGroup_);
    
    lblNumSamples_ = new QLabel("-");
    lblNumInputs_ = new QLabel("-");
    lblNumOutputs_ = new QLabel("-");
    
    generalLayout->addRow("Nombre d'échantillons:", lblNumSamples_);
    generalLayout->addRow("Nombre d'entrées:", lblNumInputs_);
    generalLayout->addRow("Nombre de sorties:", lblNumOutputs_);
    
    mainLayout->addWidget(generalGroup_);
    
    // === Statistiques des entrées ===
    inputStatsGroup_ = new QGroupBox("📥 Statistiques des Entrées");
    QVBoxLayout* inputLayout = new QVBoxLayout(inputStatsGroup_);
    
    inputStatsTable_ = new QTableWidget;
    inputStatsTable_->setColumnCount(4);
    inputStatsTable_->setHorizontalHeaderLabels({"Variable", "Min", "Max", "Moyenne"});
    inputStatsTable_->horizontalHeader()->setStretchLastSection(true);
    inputStatsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    inputStatsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    inputStatsTable_->setMaximumHeight(200);
    
    inputLayout->addWidget(inputStatsTable_);
    mainLayout->addWidget(inputStatsGroup_);
    
    // === Statistiques des sorties ===
    outputStatsGroup_ = new QGroupBox("📤 Statistiques des Sorties");
    QVBoxLayout* outputLayout = new QVBoxLayout(outputStatsGroup_);
    
    outputStatsTable_ = new QTableWidget;
    outputStatsTable_->setColumnCount(4);
    outputStatsTable_->setHorizontalHeaderLabels({"Variable", "Min", "Max", "Moyenne"});
    outputStatsTable_->horizontalHeader()->setStretchLastSection(true);
    outputStatsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    outputStatsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    outputStatsTable_->setMaximumHeight(200);
    
    outputLayout->addWidget(outputStatsTable_);
    mainLayout->addWidget(outputStatsGroup_);
    
    mainLayout->addStretch();
    
    // Style des labels
    QFont valueFont("Consolas", 10);
    lblNumSamples_->setFont(valueFont);
    lblNumInputs_->setFont(valueFont);
    lblNumOutputs_->setFont(valueFont);
}

void StatisticsWidget::updateStatistics(const NeuroUIT::DatasetManager::Statistics& stats) {
    lblNumSamples_->setText(QString::number(stats.numSamples));
    lblNumInputs_->setText(QString::number(stats.numInputs));
    lblNumOutputs_->setText(QString::number(stats.numOutputs));
    
    // Remplir le tableau des entrées
    inputStatsTable_->setRowCount(static_cast<int>(stats.numInputs));
    for (size_t i = 0; i < stats.numInputs; ++i) {
        inputStatsTable_->setItem(static_cast<int>(i), 0, 
                                  new QTableWidgetItem(QString("Input %1").arg(i + 1)));
        inputStatsTable_->setItem(static_cast<int>(i), 1, 
                                  new QTableWidgetItem(QString::number(stats.inputMin[i], 'f', 4)));
        inputStatsTable_->setItem(static_cast<int>(i), 2, 
                                  new QTableWidgetItem(QString::number(stats.inputMax[i], 'f', 4)));
        inputStatsTable_->setItem(static_cast<int>(i), 3, 
                                  new QTableWidgetItem(QString::number(stats.inputMean[i], 'f', 4)));
    }
    inputStatsTable_->resizeColumnsToContents();
    
    // Remplir le tableau des sorties
    outputStatsTable_->setRowCount(static_cast<int>(stats.numOutputs));
    for (size_t i = 0; i < stats.numOutputs; ++i) {
        outputStatsTable_->setItem(static_cast<int>(i), 0, 
                                   new QTableWidgetItem(QString("Output %1").arg(i + 1)));
        outputStatsTable_->setItem(static_cast<int>(i), 1, 
                                   new QTableWidgetItem(QString::number(stats.outputMin[i], 'f', 4)));
        outputStatsTable_->setItem(static_cast<int>(i), 2, 
                                   new QTableWidgetItem(QString::number(stats.outputMax[i], 'f', 4)));
        outputStatsTable_->setItem(static_cast<int>(i), 3, 
                                   new QTableWidgetItem(QString::number(stats.outputMean[i], 'f', 4)));
    }
    outputStatsTable_->resizeColumnsToContents();
}

void StatisticsWidget::clear() {
    lblNumSamples_->setText("-");
    lblNumInputs_->setText("-");
    lblNumOutputs_->setText("-");
    inputStatsTable_->setRowCount(0);
    outputStatsTable_->setRowCount(0);
}

