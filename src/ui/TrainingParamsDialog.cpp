#include "ui/TrainingParamsDialog.h"
#include "training/Trainer.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>

namespace NeuroUIT {

TrainingParamsDialog::TrainingParamsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Paramètres d'entraînement");
    setModal(true);
    resize(350, 250);
    
    setupUI();
}

void TrainingParamsDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QLabel* titleLabel = new QLabel("Configuration de l'entraînement");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14pt;");
    mainLayout->addWidget(titleLabel);
    
    QFormLayout* formLayout = new QFormLayout;
    
    // Taux d'apprentissage
    learningRateSpinBox_ = new QDoubleSpinBox;
    learningRateSpinBox_->setMinimum(0.0001);
    learningRateSpinBox_->setMaximum(10.0);
    learningRateSpinBox_->setSingleStep(0.001);
    learningRateSpinBox_->setDecimals(5);
    learningRateSpinBox_->setValue(0.01);
    formLayout->addRow("Taux d'apprentissage:", learningRateSpinBox_);
    
    // Nombre d'époques
    numEpochsSpinBox_ = new QSpinBox;
    numEpochsSpinBox_->setMinimum(1);
    numEpochsSpinBox_->setMaximum(100000);
    numEpochsSpinBox_->setValue(100);
    formLayout->addRow("Nombre d'époques:", numEpochsSpinBox_);
    
    // Taille du batch
    batchSizeSpinBox_ = new QSpinBox;
    batchSizeSpinBox_->setMinimum(1);
    batchSizeSpinBox_->setMaximum(10000);
    batchSizeSpinBox_->setValue(1);
    formLayout->addRow("Taille du batch:", batchSizeSpinBox_);
    
    // Momentum
    momentumSpinBox_ = new QDoubleSpinBox;
    momentumSpinBox_->setMinimum(0.0);
    momentumSpinBox_->setMaximum(1.0);
    momentumSpinBox_->setSingleStep(0.01);
    momentumSpinBox_->setDecimals(2);
    momentumSpinBox_->setValue(0.0);
    formLayout->addRow("Momentum:", momentumSpinBox_);
    
    // Mélanger
    shuffleCheckBox_ = new QCheckBox("Mélanger les échantillons à chaque époque");
    shuffleCheckBox_->setChecked(true);
    formLayout->addRow("", shuffleCheckBox_);
    
    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();
    
    // Boutons OK/Cancel
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    
    QPushButton* btnOK = new QPushButton("OK");
    QPushButton* btnCancel = new QPushButton("Annuler");
    
    buttonLayout->addWidget(btnOK);
    buttonLayout->addWidget(btnCancel);
    
    mainLayout->addLayout(buttonLayout);
    
    // Connecter les signaux
    connect(btnOK, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

Trainer::TrainingParams TrainingParamsDialog::getParams() const {
    Trainer::TrainingParams params;
    params.learningRate = learningRateSpinBox_->value();
    params.numEpochs = static_cast<size_t>(numEpochsSpinBox_->value());
    params.batchSize = static_cast<size_t>(batchSizeSpinBox_->value());
    params.momentum = momentumSpinBox_->value();
    params.shuffle = shuffleCheckBox_->isChecked();
    
    return params;
}

} // namespace NeuroUIT

