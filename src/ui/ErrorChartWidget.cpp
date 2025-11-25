#include "ui/ErrorChartWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <cmath>
#include <algorithm>
#include <limits>

ErrorChartWidget::ErrorChartWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(800, 400);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

void ErrorChartWidget::addDataPoint(size_t epoch, double error) {
    if (epoch >= epochErrors_.size()) {
        epochErrors_.resize(epoch + 1);
    }
    epochErrors_[epoch] = error;
    update(); // Demander un repaint
}

void ErrorChartWidget::clear() {
    epochErrors_.clear();
    update();
}

void ErrorChartWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    painter.fillRect(rect(), Qt::white);
    
    if (epochErrors_.empty()) {
        painter.drawText(rect(), Qt::AlignCenter, "Aucune donnée à afficher");
        return;
    }
    
    drawChart(painter);
}

void ErrorChartWidget::drawChart(QPainter& painter) {
    int width = this->width();
    int height = this->height();
    
    int chartWidth = width - LEFT_MARGIN - MARGIN;
    int chartHeight = height - MARGIN - BOTTOM_MARGIN;
    
    // Trouver min et max pour normaliser
    auto [minIt, maxIt] = std::minmax_element(epochErrors_.begin(), epochErrors_.end());
    double minError = *minIt;
    double maxError = *maxIt;
    
    if (maxError == minError) {
        maxError = minError + 1.0; // Éviter division par zéro
    }
    
    // Dessiner les axes
    painter.setPen(QPen(Qt::black, 2));
    
    // Axe X (époques)
    painter.drawLine(LEFT_MARGIN, height - BOTTOM_MARGIN,
                    width - MARGIN, height - BOTTOM_MARGIN);
    
    // Axe Y (erreur)
    painter.drawLine(LEFT_MARGIN, MARGIN,
                    LEFT_MARGIN, height - BOTTOM_MARGIN);
    
    // Labels des axes
    painter.setFont(QFont("Arial", 10));
    painter.drawText(LEFT_MARGIN - 50, height - BOTTOM_MARGIN + 20, "Erreur");
    painter.drawText(width / 2 - 30, height - 10, "Époque");
    
    // Graduations et labels
    int numXTicks = std::min(10, static_cast<int>(epochErrors_.size()));
    for (int i = 0; i <= numXTicks; ++i) {
        double xPos = LEFT_MARGIN + (chartWidth * i) / numXTicks;
        painter.drawLine(xPos, height - BOTTOM_MARGIN - 5,
                        xPos, height - BOTTOM_MARGIN + 5);
        
        size_t epoch = (epochErrors_.size() - 1) * i / numXTicks;
        painter.drawText(xPos - 15, height - BOTTOM_MARGIN + 20,
                        QString::number(epoch));
    }
    
    int numYTicks = 10;
    for (int i = 0; i <= numYTicks; ++i) {
        double yPos = MARGIN + (chartHeight * (numYTicks - i)) / numYTicks;
        painter.drawLine(LEFT_MARGIN - 5, yPos,
                        LEFT_MARGIN + 5, yPos);
        
        double error = minError + (maxError - minError) * i / numYTicks;
        painter.drawText(LEFT_MARGIN - 55, yPos + 5,
                        QString::number(error, 'f', 4));
    }
    
    // Dessiner la courbe
    if (epochErrors_.size() < 2) {
        return;
    }
    
    QPainterPath path;
    bool firstPoint = true;
    
    painter.setPen(QPen(Qt::blue, 2));
    painter.setBrush(Qt::NoBrush);
    
    for (size_t i = 0; i < epochErrors_.size(); ++i) {
        if (epochErrors_[i] == 0.0 && i > 0) {
            continue; // Ignorer les zéros (époques non encore calculées)
        }
        
        double x = LEFT_MARGIN + (chartWidth * i) / (epochErrors_.size() - 1);
        double normalizedError = (epochErrors_[i] - minError) / (maxError - minError);
        double y = MARGIN + chartHeight * (1.0 - normalizedError);
        
        if (firstPoint) {
            path.moveTo(x, y);
            firstPoint = false;
        } else {
            path.lineTo(x, y);
        }
    }
    
    painter.drawPath(path);
    
    // Dessiner les points
    painter.setBrush(QBrush(Qt::blue));
    for (size_t i = 0; i < epochErrors_.size(); ++i) {
        if (epochErrors_[i] == 0.0 && i > 0) {
            continue;
        }
        
        double x = LEFT_MARGIN + (chartWidth * i) / (epochErrors_.size() - 1);
        double normalizedError = (epochErrors_[i] - minError) / (maxError - minError);
        double y = MARGIN + chartHeight * (1.0 - normalizedError);
        
        painter.drawEllipse(QPointF(x, y), 3, 3);
    }
}

