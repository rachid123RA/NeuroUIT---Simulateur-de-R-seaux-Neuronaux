#include "ui/NetworkConfigDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QListWidget>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QListWidgetItem>

NetworkConfigDialog::NetworkConfigDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Configuration du reseau");
    setModal(true);
    resize(600, 500);
    
    // Style moderne
    setStyleSheet(
        "QDialog { background-color: #ffffff; }"
        "QLabel { color: #333333; }"
        "QPushButton { "
        "background-color: #2196F3; "
        "color: white; "
        "border: none; "
        "border-radius: 5px; "
        "padding: 8px 16px; "
        "font-weight: bold; "
        "}"
        "QPushButton:hover { background-color: #1976D2; }"
        "QPushButton:pressed { background-color: #1565C0; }"
        "QPushButton:disabled { background-color: #cccccc; color: #666666; }"
        "QListWidget { "
        "border: 2px solid #e0e0e0; "
        "border-radius: 5px; "
        "background-color: #fafafa; "
        "}"
        "QListWidget::item:selected { "
        "background-color: #2196F3; "
        "color: white; "
        "}"
        "QSpinBox, QComboBox { "
        "border: 2px solid #e0e0e0; "
        "border-radius: 5px; "
        "padding: 5px; "
        "background-color: white; "
        "}"
        "QSpinBox:focus, QComboBox:focus { "
        "border-color: #2196F3; "
        "}"
    );
    
    setupUI();
}

void NetworkConfigDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QLabel* titleLabel = new QLabel("Configuration de l'architecture du reseau");
    titleLabel->setStyleSheet(
        "font-weight: bold; "
        "font-size: 16pt; "
        "color: #2196F3; "
        "padding: 10px; "
    );
    mainLayout->addWidget(titleLabel);
    
    QLabel* descLabel = new QLabel("Definissez le nombre de neurones pour chaque couche. Vous pouvez modifier la couche d'entree.");
    descLabel->setStyleSheet("color: #666666; padding: 5px;");
    descLabel->setWordWrap(true);
    mainLayout->addWidget(descLabel);
    
    // Liste des couches
    QHBoxLayout* layerLayout = new QHBoxLayout;
    
    QVBoxLayout* listLayout = new QVBoxLayout;
    listLayout->addWidget(new QLabel("Couches:"));
    layerListWidget_ = new QListWidget;
    listLayout->addWidget(layerListWidget_);
    layerLayout->addLayout(listLayout);
    
    // Contrôles pour ajouter/modifier les couches
    QVBoxLayout* controlsLayout = new QVBoxLayout;
    
    QFormLayout* formLayout = new QFormLayout;
    
    numNeuronsSpinBox_ = new QSpinBox;
    numNeuronsSpinBox_->setMinimum(1);
    numNeuronsSpinBox_->setMaximum(1000);
    numNeuronsSpinBox_->setValue(2);
    formLayout->addRow("Nombre de neurones:", numNeuronsSpinBox_);
    
    activationComboBox_ = new QComboBox;
    activationComboBox_->addItem("Sigmoid", static_cast<int>(NeuroUIT::ActivationType::Sigmoid));
    activationComboBox_->addItem("Tanh", static_cast<int>(NeuroUIT::ActivationType::Tanh));
    activationComboBox_->addItem("ReLU", static_cast<int>(NeuroUIT::ActivationType::ReLU));
    activationComboBox_->addItem("Linéaire", static_cast<int>(NeuroUIT::ActivationType::Linear));
    formLayout->addRow("Fonction d'activation:", activationComboBox_);
    
    controlsLayout->addLayout(formLayout);
    controlsLayout->addStretch();
    
    btnAddLayer_ = new QPushButton("Ajouter couche");
    btnAddLayer_->setStyleSheet(
        "QPushButton { background-color: #4CAF50; }"
        "QPushButton:hover { background-color: #45a049; }"
    );
    btnUpdateLayer_ = new QPushButton("Modifier couche");
    btnUpdateLayer_->setEnabled(false);
    btnUpdateLayer_->setStyleSheet(
        "QPushButton { background-color: #FF9800; }"
        "QPushButton:hover { background-color: #F57C00; }"
        "QPushButton:disabled { background-color: #cccccc; }"
    );
    btnRemoveLayer_ = new QPushButton("Supprimer couche");
    btnRemoveLayer_->setEnabled(false);
    btnRemoveLayer_->setStyleSheet(
        "QPushButton { background-color: #F44336; }"
        "QPushButton:hover { background-color: #D32F2F; }"
        "QPushButton:disabled { background-color: #cccccc; }"
    );
    
    controlsLayout->addWidget(btnAddLayer_);
    controlsLayout->addWidget(btnUpdateLayer_);
    controlsLayout->addWidget(btnRemoveLayer_);
    
    layerLayout->addLayout(controlsLayout);
    
    mainLayout->addLayout(layerLayout);
    
    // Boutons OK/Cancel
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    
    QPushButton* btnOK = new QPushButton("OK");
    QPushButton* btnCancel = new QPushButton("Annuler");
    
    buttonLayout->addWidget(btnOK);
    buttonLayout->addWidget(btnCancel);
    
    mainLayout->addLayout(buttonLayout);
    
    // Connecter les signaux
    connect(btnAddLayer_, &QPushButton::clicked, this, &NetworkConfigDialog::onAddLayer);
    connect(btnUpdateLayer_, &QPushButton::clicked, this, &NetworkConfigDialog::onUpdateLayer);
    connect(btnRemoveLayer_, &QPushButton::clicked, this, &NetworkConfigDialog::onRemoveLayer);
    connect(layerListWidget_, &QListWidget::itemSelectionChanged, 
            this, &NetworkConfigDialog::onLayerSelectionChanged);
    // Modification automatique quand on change les valeurs
    connect(numNeuronsSpinBox_, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &NetworkConfigDialog::onUpdateLayer);
    connect(activationComboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &NetworkConfigDialog::onUpdateLayer);
    connect(btnOK, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    
    // Ajouter une couche d'entrée par défaut (2 neurones)
    layerSizes_.push_back(2);
    layerActivations_.push_back(NeuroUIT::ActivationType::Sigmoid); // Par défaut, mais pas utilisé pour l'entrée
    updateLayerList();
    layerListWidget_->setCurrentRow(0);
    onLayerSelectionChanged();
}

void NetworkConfigDialog::onAddLayer() {
    size_t numNeurons = static_cast<size_t>(numNeuronsSpinBox_->value());
    NeuroUIT::ActivationType activation = static_cast<NeuroUIT::ActivationType>(
        activationComboBox_->currentData().toInt()
    );
    
    layerSizes_.push_back(numNeurons);
    layerActivations_.push_back(activation);
    
    updateLayerList();
}

void NetworkConfigDialog::onUpdateLayer() {
    int currentRow = layerListWidget_->currentRow();
    if (currentRow >= 0 && currentRow < static_cast<int>(layerSizes_.size())) {
        // Désactiver temporairement les signaux pour éviter les boucles infinies
        numNeuronsSpinBox_->blockSignals(true);
        activationComboBox_->blockSignals(true);
        
        size_t numNeurons = static_cast<size_t>(numNeuronsSpinBox_->value());
        NeuroUIT::ActivationType activation = static_cast<NeuroUIT::ActivationType>(
            activationComboBox_->currentData().toInt()
        );
        
        layerSizes_[currentRow] = numNeurons;
        if (currentRow > 0) { // La couche d'entrée n'a pas d'activation
            layerActivations_[currentRow] = activation;
        }
        
        updateLayerList();
        layerListWidget_->setCurrentRow(currentRow); // Resélectionner la couche modifiée
        
        // Réactiver les signaux
        numNeuronsSpinBox_->blockSignals(false);
        activationComboBox_->blockSignals(false);
    }
}

void NetworkConfigDialog::onRemoveLayer() {
    int currentRow = layerListWidget_->currentRow();
    if (currentRow >= 0 && currentRow < static_cast<int>(layerSizes_.size()) && layerSizes_.size() > 1) {
        layerSizes_.erase(layerSizes_.begin() + currentRow);
        layerActivations_.erase(layerActivations_.begin() + currentRow);
        updateLayerList();
        
        // Sélectionner la couche suivante ou précédente
        if (currentRow >= static_cast<int>(layerSizes_.size())) {
            layerListWidget_->setCurrentRow(static_cast<int>(layerSizes_.size()) - 1);
        } else {
            layerListWidget_->setCurrentRow(currentRow);
        }
        onLayerSelectionChanged();
    }
}

void NetworkConfigDialog::onLayerSelectionChanged() {
    int currentRow = layerListWidget_->currentRow();
    bool hasSelection = currentRow >= 0 && currentRow < static_cast<int>(layerSizes_.size());
    
    btnUpdateLayer_->setEnabled(hasSelection);
    btnRemoveLayer_->setEnabled(hasSelection && layerSizes_.size() > 1); // Au moins une couche doit rester
    
    // Charger les valeurs de la couche sélectionnée dans les contrôles
    // Bloquer les signaux pour éviter les modifications automatiques pendant le chargement
    if (hasSelection) {
        numNeuronsSpinBox_->blockSignals(true);
        activationComboBox_->blockSignals(true);
        
        numNeuronsSpinBox_->setValue(static_cast<int>(layerSizes_[currentRow]));
        activationComboBox_->setCurrentIndex(
            activationComboBox_->findData(static_cast<int>(layerActivations_[currentRow]))
        );
        
        // Désactiver la fonction d'activation pour la couche d'entrée (elle n'en a pas)
        if (currentRow == 0) {
            activationComboBox_->setEnabled(false);
            activationComboBox_->setToolTip("La couche d'entree n'a pas de fonction d'activation");
        } else {
            activationComboBox_->setEnabled(true);
            activationComboBox_->setToolTip("");
        }
        
        numNeuronsSpinBox_->blockSignals(false);
        activationComboBox_->blockSignals(false);
    }
}

void NetworkConfigDialog::updateLayerList() {
    layerListWidget_->clear();
    
    for (size_t i = 0; i < layerSizes_.size(); ++i) {
        QString layerName;
        if (i == 0) {
            layerName = QString("Couche d'entrée: %1 neurones").arg(layerSizes_[i]);
        } else if (i == layerSizes_.size() - 1) {
            QString actName;
            switch (layerActivations_[i]) {
                case NeuroUIT::ActivationType::Sigmoid: actName = "Sigmoid"; break;
                case NeuroUIT::ActivationType::Tanh: actName = "Tanh"; break;
                case NeuroUIT::ActivationType::ReLU: actName = "ReLU"; break;
                case NeuroUIT::ActivationType::Linear: actName = "Linéaire"; break;
            }
            layerName = QString("Couche de sortie: %1 neurones (%2)")
                .arg(layerSizes_[i]).arg(actName);
        } else {
            QString actName;
            switch (layerActivations_[i]) {
                case NeuroUIT::ActivationType::Sigmoid: actName = "Sigmoid"; break;
                case NeuroUIT::ActivationType::Tanh: actName = "Tanh"; break;
                case NeuroUIT::ActivationType::ReLU: actName = "ReLU"; break;
                case NeuroUIT::ActivationType::Linear: actName = "Linéaire"; break;
            }
            layerName = QString("Couche cachée %1: %2 neurones (%3)")
                .arg(i).arg(layerSizes_[i]).arg(actName);
        }
        
        layerListWidget_->addItem(layerName);
    }
}

NetworkConfigDialog::NetworkConfig NetworkConfigDialog::getConfig() const {
    NetworkConfig config;
    config.architecture = layerSizes_;
    
    // Les activations commencent à partir de la deuxième couche
    if (layerSizes_.size() > 1) {
        config.activations.assign(layerActivations_.begin() + 1, layerActivations_.end());
    }
    
    return config;
}

