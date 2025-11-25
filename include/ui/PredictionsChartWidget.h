#ifndef PREDICTIONSCHARTWIDGET_H
#define PREDICTIONSCHARTWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <vector>

/**
 * @brief Widget pour visualiser les prédictions vs valeurs réelles
 */
class PredictionsChartWidget : public QWidget {
    Q_OBJECT

public:
    explicit PredictionsChartWidget(QWidget *parent = nullptr);
    
    /**
     * @brief Met à jour les données de prédiction
     * @param predictions Valeurs prédites
     * @param actuals Valeurs réelles
     */
    void updateData(const std::vector<double>& predictions,
                   const std::vector<double>& actuals);
    
    /**
     * @brief Réinitialise l'affichage
     */
    void clear();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void drawChart(QPainter& painter);
    
    std::vector<double> predictions_;
    std::vector<double> actuals_;
    bool hasData_;
    
    static constexpr int MARGIN = 60;
    static constexpr int BOTTOM_MARGIN = 40;
    static constexpr int LEFT_MARGIN = 60;
};

#endif // PREDICTIONSCHARTWIDGET_H

