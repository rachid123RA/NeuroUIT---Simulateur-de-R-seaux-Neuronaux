#ifndef ERRORCHARTWIDGET_H
#define ERRORCHARTWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <vector>
#include <algorithm>

/**
 * @brief Widget pour afficher le graphique d'évolution de l'erreur
 */
class ErrorChartWidget : public QWidget {
    Q_OBJECT

public:
    explicit ErrorChartWidget(QWidget *parent = nullptr);
    
    /**
     * @brief Ajoute un point de données (époque, erreur)
     */
    void addDataPoint(size_t epoch, double error);
    
    /**
     * @brief Vide le graphique
     */
    void clear();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void drawChart(QPainter& painter);
    
    std::vector<double> epochErrors_;
    
    static constexpr int MARGIN = 50;
    static constexpr int BOTTOM_MARGIN = 40;
    static constexpr int LEFT_MARGIN = 60;
};

#endif // ERRORCHARTWIDGET_H

