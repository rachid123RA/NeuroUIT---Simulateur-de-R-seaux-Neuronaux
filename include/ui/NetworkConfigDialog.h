#ifndef NETWORKCONFIGDIALOG_H
#define NETWORKCONFIGDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QComboBox>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <vector>
#include "core/ActivationFunction.h"

/**
 * @brief Dialog pour configurer un nouveau réseau
 */
class NetworkConfigDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Configuration d'un réseau (architecture et activations)
     */
    struct NetworkConfig {
        std::vector<size_t> architecture;
        std::vector<NeuroUIT::ActivationType> activations;
    };

    explicit NetworkConfigDialog(QWidget *parent = nullptr);
    
    /**
     * @brief Obtient la configuration saisie
     */
    NetworkConfig getConfig() const;

private slots:
    void onAddLayer();
    void onRemoveLayer();
    void onLayerSelectionChanged();
    void onUpdateLayer();

private:
    void setupUI();
    void updateLayerList();
    
    QListWidget* layerListWidget_;
    QSpinBox* numNeuronsSpinBox_;
    QComboBox* activationComboBox_;
    QPushButton* btnAddLayer_;
    QPushButton* btnUpdateLayer_;
    QPushButton* btnRemoveLayer_;
    
    std::vector<size_t> layerSizes_;
    std::vector<NeuroUIT::ActivationType> layerActivations_;
};

#endif // NETWORKCONFIGDIALOG_H
