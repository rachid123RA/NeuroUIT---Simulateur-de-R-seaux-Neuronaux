#include "ui/ConfusionMatrixWidget.h"
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <algorithm>
#include <cmath>

ConfusionMatrixWidget::ConfusionMatrixWidget(QWidget *parent)
    : QWidget(parent)
    , hasData_(false)
{
    setMinimumSize(400, 400);
}

void ConfusionMatrixWidget::updateMatrix(const std::vector<std::vector<int>>& matrix,
                                        const std::vector<QString>& classLabels) {
    matrix_ = matrix;
    classLabels_ = classLabels;
    hasData_ = !matrix.empty() && !matrix[0].empty();
    
    // Générer des labels par défaut si non fournis
    if (classLabels_.empty() && hasData_) {
        size_t numClasses = matrix_.size();
        for (size_t i = 0; i < numClasses; ++i) {
            classLabels_.push_back(QString("Classe %1").arg(i));
        }
    }
    
    update();
}

void ConfusionMatrixWidget::clear() {
    matrix_.clear();
    classLabels_.clear();
    hasData_ = false;
    update();
}

void ConfusionMatrixWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    if (!hasData_) {
        painter.setPen(Qt::gray);
        painter.setFont(QFont("Arial", 12));
        painter.drawText(rect(), Qt::AlignCenter, "Testez le réseau pour voir la matrice de confusion");
        return;
    }
    
    drawMatrix(painter);
}

void ConfusionMatrixWidget::drawMatrix(QPainter& painter) {
    size_t numClasses = matrix_.size();
    if (numClasses == 0) return;
    
    // Trouver la valeur maximale pour la normalisation des couleurs
    int maxValue = 0;
    for (const auto& row : matrix_) {
        for (int value : row) {
            maxValue = std::max(maxValue, value);
        }
    }
    
    if (maxValue == 0) maxValue = 1; // Éviter division par zéro
    
    // Calculer la position de départ
    int startX = MARGIN + LABEL_SIZE;
    int startY = MARGIN + LABEL_SIZE;
    
    // Dessiner les cellules
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    QFontMetrics fm(painter.font());
    
    for (size_t i = 0; i < numClasses; ++i) {
        for (size_t j = 0; j < numClasses; ++j) {
            int x = startX + static_cast<int>(j) * CELL_SIZE;
            int y = startY + static_cast<int>(i) * CELL_SIZE;
            
            QRect cellRect(x, y, CELL_SIZE, CELL_SIZE);
            
            // Couleur basée sur la valeur
            QColor cellColor = getColorForValue(matrix_[i][j], maxValue);
            painter.fillRect(cellRect, cellColor);
            
            // Bordure
            painter.setPen(QPen(Qt::black, 1));
            painter.drawRect(cellRect);
            
            // Texte (valeur)
            painter.setPen(Qt::black);
            QString text = QString::number(matrix_[i][j]);
            painter.drawText(cellRect, Qt::AlignCenter, text);
        }
    }
    
    // Dessiner les labels des axes
    painter.setFont(QFont("Arial", 9));
    painter.setPen(Qt::black);
    
    // Labels des lignes (Prédictions)
    for (size_t i = 0; i < numClasses; ++i) {
        int y = startY + static_cast<int>(i) * CELL_SIZE + CELL_SIZE / 2;
        QString label = i < classLabels_.size() ? classLabels_[i] : QString("C%1").arg(i);
        painter.drawText(QRect(0, y - 10, MARGIN, 20), Qt::AlignRight | Qt::AlignVCenter, label);
    }
    
    // Labels des colonnes (Réelles)
    for (size_t j = 0; j < numClasses; ++j) {
        int x = startX + static_cast<int>(j) * CELL_SIZE + CELL_SIZE / 2;
        QString label = j < classLabels_.size() ? classLabels_[j] : QString("C%1").arg(j);
        QRect textRect = fm.boundingRect(label);
        painter.drawText(QRect(x - textRect.width() / 2, MARGIN - LABEL_SIZE, 
                              textRect.width(), LABEL_SIZE), 
                        Qt::AlignCenter, label);
    }
    
    // Titres des axes
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(QRect(0, 0, MARGIN, MARGIN), Qt::AlignCenter, "Prédit");
    painter.drawText(QRect(startX, 0, numClasses * CELL_SIZE, MARGIN), 
                    Qt::AlignCenter, "Réel");
}

QColor ConfusionMatrixWidget::getColorForValue(int value, int maxValue) const {
    if (maxValue == 0) return Qt::white;
    
    // Normaliser entre 0 et 1
    double normalized = static_cast<double>(value) / maxValue;
    
    // Gradient du blanc au vert foncé
    int green = static_cast<int>(255 * normalized);
    int red = static_cast<int>(255 * (1.0 - normalized * 0.5));
    int blue = static_cast<int>(255 * (1.0 - normalized * 0.5));
    
    return QColor(red, green, blue);
}

