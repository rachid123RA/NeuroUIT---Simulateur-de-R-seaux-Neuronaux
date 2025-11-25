#include "ui/PredictionsChartWidget.h"
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <algorithm>
#include <cmath>

PredictionsChartWidget::PredictionsChartWidget(QWidget *parent)
    : QWidget(parent)
    , hasData_(false)
{
    setMinimumSize(500, 400);
}

void PredictionsChartWidget::updateData(const std::vector<double>& predictions,
                                       const std::vector<double>& actuals) {
    predictions_ = predictions;
    actuals_ = actuals;
    hasData_ = !predictions.empty() && !actuals.empty() && 
               predictions.size() == actuals.size();
    update();
}

void PredictionsChartWidget::clear() {
    predictions_.clear();
    actuals_.clear();
    hasData_ = false;
    update();
}

void PredictionsChartWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    if (!hasData_) {
        painter.setPen(Qt::gray);
        painter.setFont(QFont("Arial", 12));
        painter.drawText(rect(), Qt::AlignCenter, 
                        "Testez le réseau pour voir les prédictions");
        return;
    }
    
    drawChart(painter);
}

void PredictionsChartWidget::drawChart(QPainter& painter) {
    size_t numPoints = predictions_.size();
    if (numPoints == 0) return;
    
    // Trouver les min/max pour l'échelle
    double minVal = std::min(*std::min_element(predictions_.begin(), predictions_.end()),
                             *std::min_element(actuals_.begin(), actuals_.end()));
    double maxVal = std::max(*std::max_element(predictions_.begin(), predictions_.end()),
                             *std::max_element(actuals_.begin(), actuals_.end()));
    
    // Ajouter une marge
    double range = maxVal - minVal;
    if (range < 0.001) range = 1.0; // Éviter division par zéro
    minVal -= range * 0.1;
    maxVal += range * 0.1;
    range = maxVal - minVal;
    
    // Zone de dessin
    int chartWidth = width() - LEFT_MARGIN - MARGIN;
    int chartHeight = height() - MARGIN - BOTTOM_MARGIN;
    int startX = LEFT_MARGIN;
    int startY = MARGIN;
    
    // Dessiner les axes
    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(startX, startY + chartHeight, startX + chartWidth, startY + chartHeight);
    painter.drawLine(startX, startY, startX, startY + chartHeight);
    
    // Labels des axes
    painter.setFont(QFont("Arial", 9));
    painter.setPen(Qt::black);
    painter.drawText(QRect(0, startY + chartHeight / 2, LEFT_MARGIN - 5, 20),
                    Qt::AlignRight | Qt::AlignVCenter, "Valeur");
    painter.drawText(QRect(startX + chartWidth / 2, startY + chartHeight + 5, 
                          chartWidth, BOTTOM_MARGIN - 5),
                    Qt::AlignCenter, "Échantillon");
    
    // Ligne de référence (y = x)
    painter.setPen(QPen(Qt::gray, 1, Qt::DashLine));
    double refMin = std::min(minVal, 0.0);
    double refMax = std::max(maxVal, 1.0);
    int refX1 = startX + static_cast<int>((refMin - minVal) / range * chartWidth);
    int refY1 = startY + chartHeight - static_cast<int>((refMin - minVal) / range * chartHeight);
    int refX2 = startX + static_cast<int>((refMax - minVal) / range * chartWidth);
    int refY2 = startY + chartHeight - static_cast<int>((refMax - minVal) / range * chartHeight);
    painter.drawLine(refX1, refY1, refX2, refY2);
    
    // Dessiner les points
    painter.setPen(QPen(Qt::blue, 2));
    for (size_t i = 0; i < numPoints; ++i) {
        int x = startX + static_cast<int>(i * chartWidth / (numPoints - 1));
        int predY = startY + chartHeight - static_cast<int>((predictions_[i] - minVal) / range * chartHeight);
        int actualY = startY + chartHeight - static_cast<int>((actuals_[i] - minVal) / range * chartHeight);
        
        // Point prédiction (bleu)
        painter.setBrush(QBrush(Qt::blue));
        painter.drawEllipse(x - 3, predY - 3, 6, 6);
        
        // Point réel (rouge)
        painter.setBrush(QBrush(Qt::red));
        painter.drawEllipse(x - 3, actualY - 3, 6, 6);
        
        // Ligne entre prédiction et réel
        painter.setPen(QPen(Qt::gray, 1, Qt::DotLine));
        painter.drawLine(x, predY, x, actualY);
        painter.setPen(QPen(Qt::blue, 2));
    }
    
    // Légende
    painter.setFont(QFont("Arial", 9));
    int legendX = startX + chartWidth - 150;
    int legendY = startY + 10;
    
    painter.setBrush(QBrush(Qt::blue));
    painter.drawEllipse(legendX, legendY, 8, 8);
    painter.setPen(Qt::black);
    painter.drawText(legendX + 15, legendY + 8, "Prédictions");
    
    painter.setBrush(QBrush(Qt::red));
    painter.drawEllipse(legendX, legendY + 20, 8, 8);
    painter.drawText(legendX + 15, legendY + 28, "Réelles");
    
    // Valeurs min/max sur les axes
    painter.setPen(Qt::black);
    painter.drawText(QRect(startX - 55, startY + chartHeight - 10, 50, 20),
                    Qt::AlignRight, QString::number(minVal, 'f', 2));
    painter.drawText(QRect(startX - 55, startY - 10, 50, 20),
                    Qt::AlignRight, QString::number(maxVal, 'f', 2));
}

