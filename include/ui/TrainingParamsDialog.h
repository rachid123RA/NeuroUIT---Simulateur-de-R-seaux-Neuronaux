#ifndef TRAININGPARAMSDIALOG_H
#define TRAININGPARAMSDIALOG_H

#include <QDialog>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QFormLayout>
#include "training/Trainer.h"

namespace NeuroUIT {

/**
 * @brief Dialog pour configurer les paramètres d'entraînement
 */
class TrainingParamsDialog : public QDialog {
    Q_OBJECT

public:
    explicit TrainingParamsDialog(QWidget *parent = nullptr);
    
    /**
     * @brief Obtient les paramètres saisies
     */
    Trainer::TrainingParams getParams() const;

private:
    void setupUI();
    
    QDoubleSpinBox* learningRateSpinBox_;
    QSpinBox* numEpochsSpinBox_;
    QSpinBox* batchSizeSpinBox_;
    QDoubleSpinBox* momentumSpinBox_;
    QCheckBox* shuffleCheckBox_;
};

} // namespace NeuroUIT

#endif // TRAININGPARAMSDIALOG_H










