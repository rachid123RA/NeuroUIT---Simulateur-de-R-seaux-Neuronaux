#include "ui/CorrelationMatrixWidget.h"
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <algorithm>
#include <cmath>

CorrelationMatrixWidget::CorrelationMatrixWidget(QWidget *parent)
    : QWidget(parent)
    , hasData_(false)
{
    setMinimumSize(400, 400);
}

void CorrelationMatrixWidget::updateMatrix(const std::vector<std::vector<double>>& matrix,
                                           const std::vector<QString>& labels) {
    matrix_ = matrix;
    labels_ = labels;
    hasData_ = !matrix.empty() && !matrix[0].empty();
    
    // Générer des labels par défaut si non fournis
    if (labels_.empty() && hasData_) {
        size_t numVars = matrix_.size();
        for (size_t i = 0; i < numVars; ++i) {
            labels_.push_back(QString("Var %1").arg(i + 1));
        }
    }
    
    update();
}

void CorrelationMatrixWidget::clear() {
    matrix_.clear();
    labels_.clear();
    hasData_ = false;
    update();
}

void CorrelationMatrixWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    if (!hasData_) {
        painter.setPen(Qt::gray);
        painter.setFont(QFont("Arial", 12));
        painter.drawText(rect(), Qt::AlignCenter, 
                        "Chargez un dataset pour voir la matrice de corrélation");
        return;
    }
    
    drawMatrix(painter);
}

void CorrelationMatrixWidget::drawMatrix(QPainter& painter) {
    size_t numVars = matrix_.size();
    if (numVars == 0) return;
    
    // Calculer la position de départ
    int startX = MARGIN + LABEL_SIZE;
    int startY = MARGIN + LABEL_SIZE;
    
    // Dessiner les cellules
    painter.setFont(QFont("Arial", 8));
    QFontMetrics fm(painter.font());
    
    for (size_t i = 0; i < numVars; ++i) {
        for (size_t j = 0; j < numVars; ++j) {
            int x = startX + static_cast<int>(j) * CELL_SIZE;
            int y = startY + static_cast<int>(i) * CELL_SIZE;
            
            QRect cellRect(x, y, CELL_SIZE, CELL_SIZE);
            
            // Couleur basée sur la corrélation
            double correlation = matrix_[i][j];
            QColor cellColor = getColorForCorrelation(correlation);
            painter.fillRect(cellRect, cellColor);
            
            // Bordure
            painter.setPen(QPen(Qt::black, 1));
            painter.drawRect(cellRect);
            
            // Texte (valeur de corrélation)
            QString text = QString::number(correlation, 'f', 2);
            painter.setPen(Qt::black);
            painter.drawText(cellRect, Qt::AlignCenter, text);
        }
    }
    
    // Dessiner les labels
    painter.setFont(QFont("Arial", 8));
    painter.setPen(Qt::black);
    
    // Labels des lignes
    for (size_t i = 0; i < numVars; ++i) {
        int y = startY + static_cast<int>(i) * CELL_SIZE + CELL_SIZE / 2;
        QString label = i < labels_.size() ? labels_[i] : QString("V%1").arg(i + 1);
        painter.drawText(QRect(0, y - 10, MARGIN, 20), Qt::AlignRight | Qt::AlignVCenter, label);
    }
    
    // Labels des colonnes
    for (size_t j = 0; j < numVars; ++j) {
        int x = startX + static_cast<int>(j) * CELL_SIZE + CELL_SIZE / 2;
        QString label = j < labels_.size() ? labels_[j] : QString("V%1").arg(j + 1);
        (void)fm.boundingRect(label); // Pour éviter le warning, on peut utiliser textRect si nécessaire
        painter.save();
        painter.translate(x, MARGIN - LABEL_SIZE);
        painter.rotate(-45);
        painter.drawText(0, 0, label);
        painter.restore();
    }
}

QColor CorrelationMatrixWidget::getColorForCorrelation(double correlation) const {
    // Normaliser entre -1 et 1 vers 0 et 1
    double normalized = (correlation + 1.0) / 2.0;
    
    // Gradient : bleu (négatif) -> blanc (0) -> rouge (positif)
    if (correlation < 0) {
        // Bleu vers blanc
        int blue = 255;
        int red = static_cast<int>(255 * normalized);
        int green = static_cast<int>(255 * normalized);
        return QColor(red, green, blue);
    } else {
        // Blanc vers rouge
        int red = 255;
        int blue = static_cast<int>(255 * (1.0 - normalized));
        int green = static_cast<int>(255 * (1.0 - normalized));
        return QColor(red, green, blue);
    }
}

