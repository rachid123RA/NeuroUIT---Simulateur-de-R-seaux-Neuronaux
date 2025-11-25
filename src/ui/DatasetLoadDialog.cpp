#include "ui/DatasetLoadDialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QFileDialog>

namespace NeuroUIT {

DatasetLoadDialog::DatasetLoadDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Charger un dataset");
    setModal(true);
    resize(400, 200);
    
    setupUI();
}

void DatasetLoadDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QLabel* titleLabel = new QLabel("Chargement d'un dataset CSV");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14pt;");
    mainLayout->addWidget(titleLabel);
    
    QFormLayout* formLayout = new QFormLayout;
    
    // Sélection du fichier
    QHBoxLayout* fileLayout = new QHBoxLayout;
    filePathEdit_ = new QLineEdit;
    browseButton_ = new QPushButton("Parcourir...");
    
    fileLayout->addWidget(filePathEdit_);
    fileLayout->addWidget(browseButton_);
    
    formLayout->addRow("Fichier CSV:", fileLayout);
    
    // Nombre d'entrées et sorties
    numInputsSpinBox_ = new QSpinBox;
    numInputsSpinBox_->setMinimum(1);
    numInputsSpinBox_->setMaximum(1000);
    numInputsSpinBox_->setValue(2);
    formLayout->addRow("Nombre d'entrées:", numInputsSpinBox_);
    
    numOutputsSpinBox_ = new QSpinBox;
    numOutputsSpinBox_->setMinimum(1);
    numOutputsSpinBox_->setMaximum(1000);
    numOutputsSpinBox_->setValue(1);
    formLayout->addRow("Nombre de sorties:", numOutputsSpinBox_);
    
    // En-tête
    hasHeaderCheckBox_ = new QCheckBox("Le fichier contient des en-têtes");
    hasHeaderCheckBox_->setChecked(false);
    formLayout->addRow("", hasHeaderCheckBox_);
    
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
    connect(browseButton_, &QPushButton::clicked, this, &DatasetLoadDialog::onBrowseFile);
    connect(btnOK, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

void DatasetLoadDialog::onBrowseFile() {
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Sélectionner un fichier CSV",
        "",
        "Fichiers CSV (*.csv);;Tous les fichiers (*.*)"
    );
    
    if (!filename.isEmpty()) {
        filePathEdit_->setText(filename);
    }
}

DatasetLoadConfig DatasetLoadDialog::getConfig() const {
    DatasetLoadConfig config;
    config.filename = filePathEdit_->text().toStdString();
    config.numInputs = static_cast<size_t>(numInputsSpinBox_->value());
    config.numOutputs = static_cast<size_t>(numOutputsSpinBox_->value());
    config.hasHeader = hasHeaderCheckBox_->isChecked();
    
    return config;
}

} // namespace NeuroUIT










