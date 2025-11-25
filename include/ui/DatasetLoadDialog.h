#ifndef DATASETLOADDIALOG_H
#define DATASETLOADDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>

namespace NeuroUIT {

/**
 * @brief Configuration pour charger un dataset
 */
struct DatasetLoadConfig {
    std::string filename;
    size_t numInputs;
    size_t numOutputs;
    bool hasHeader;
};

/**
 * @brief Dialog pour charger un dataset CSV
 */
class DatasetLoadDialog : public QDialog {
    Q_OBJECT

public:
    explicit DatasetLoadDialog(QWidget *parent = nullptr);
    
    /**
     * @brief Obtient la configuration saisie
     */
    DatasetLoadConfig getConfig() const;

private slots:
    void onBrowseFile();

private:
    void setupUI();
    
    QLineEdit* filePathEdit_;
    QPushButton* browseButton_;
    QSpinBox* numInputsSpinBox_;
    QSpinBox* numOutputsSpinBox_;
    QCheckBox* hasHeaderCheckBox_;
};

} // namespace NeuroUIT

#endif // DATASETLOADDIALOG_H










