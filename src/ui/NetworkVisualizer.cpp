#include "ui/NetworkVisualizer.h"
#include "core/Network.h"
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <QFontMetrics>
#include <cmath>
#include <algorithm>

NetworkVisualizer::NetworkVisualizer(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(800, 600);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    
    // Style moderne
    setStyleSheet(
        "NetworkVisualizer { "
        "background-color: #f8f9fa; "
        "border: 2px solid #e0e0e0; "
        "border-radius: 10px; "
        "}"
    );
}

void NetworkVisualizer::updateNetwork(std::shared_ptr<NeuroUIT::Network> network) {
    network_ = network;
    update(); // Demander un repaint
}

void NetworkVisualizer::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    // Fond avec dégradé subtil
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(248, 249, 250));
    gradient.setColorAt(1, QColor(255, 255, 255));
    painter.fillRect(rect(), gradient);
    
    if (!network_) {
        painter.setPen(QColor(150, 150, 150));
        painter.setFont(QFont("Arial", 14, QFont::Normal));
        painter.drawText(rect(), Qt::AlignCenter, "Aucun reseau a visualiser\nCreez un reseau pour voir la visualisation");
        return;
    }
    
    drawNetwork(painter);
}

void NetworkVisualizer::drawNetwork(QPainter& painter) {
    auto architecture = network_->getArchitecture();
    if (architecture.empty()) {
        return;
    }
    
    int numLayers = static_cast<int>(architecture.size());
    int widgetWidth = this->width();
    int widgetHeight = this->height();
    
    // Marges pour centrer le réseau
    int marginX = 80;
    int marginY = 60;
    int availableWidth = widgetWidth - 2 * marginX;
    int availableHeight = widgetHeight - 2 * marginY;
    
    // Calculer l'espacement optimal entre les couches
    int layerSpacing = numLayers > 1 ? availableWidth / (numLayers - 1) : availableWidth / 2;
    
    // Trouver la couche avec le plus de neurones pour le centrage vertical
    int maxNodes = *std::max_element(architecture.begin(), architecture.end());
    
    // Calculer les positions des neurones - CENTRAGE PARFAIT
    std::vector<std::vector<QPoint>> nodePositions;
    std::vector<QString> layerLabels;
    
    for (int layerIdx = 0; layerIdx < numLayers; ++layerIdx) {
        int numNodes = static_cast<int>(architecture[layerIdx]);
        std::vector<QPoint> layerPositions;
        
        // Position X centrée
        int x = marginX + (numLayers > 1 ? layerIdx * layerSpacing : availableWidth / 2);
        
        // Position Y centrée verticalement pour chaque couche
        int totalHeight = numNodes * NODE_SPACING;
        int startY = marginY + (availableHeight - totalHeight) / 2 + NODE_SPACING / 2;
        
        for (int nodeIdx = 0; nodeIdx < numNodes; ++nodeIdx) {
            int y = startY + nodeIdx * NODE_SPACING;
            layerPositions.push_back(QPoint(x, y));
        }
        
        nodePositions.push_back(layerPositions);
        
        // Labels des couches
        if (layerIdx == 0) {
            layerLabels.push_back("Entree");
        } else if (layerIdx == numLayers - 1) {
            layerLabels.push_back("Sortie");
        } else {
            layerLabels.push_back(QString("Cachee %1").arg(layerIdx));
        }
    }
    
    // Dessiner les connexions avec transparence
    painter.setPen(QPen(QColor(200, 200, 200, 120), 1.5));
    for (int layerIdx = 0; layerIdx < numLayers - 1; ++layerIdx) {
        const auto& currentLayer = nodePositions[layerIdx];
        const auto& nextLayer = nodePositions[layerIdx + 1];
        
        for (const QPoint& from : currentLayer) {
            for (const QPoint& to : nextLayer) {
                painter.drawLine(from, to);
            }
        }
    }
    
    // Couleurs modernes pour chaque type de couche
    QColor inputColor(33, 150, 243);      // Bleu Material Design
    QColor hiddenColor(76, 175, 80);      // Vert Material Design
    QColor outputColor(244, 67, 54);      // Rouge Material Design
    
    // Dessiner les neurones avec style moderne
    QFont labelFont("Arial", 9, QFont::Bold);
    QFontMetrics fm(labelFont);
    
    for (int layerIdx = 0; layerIdx < numLayers; ++layerIdx) {
        QColor nodeColor;
        QColor borderColor;
        QString layerType;
        
        if (layerIdx == 0) {
            nodeColor = inputColor;
            borderColor = QColor(25, 118, 210);
            layerType = "Entree";
        } else if (layerIdx == numLayers - 1) {
            nodeColor = outputColor;
            borderColor = QColor(198, 40, 40);
            layerType = "Sortie";
        } else {
            nodeColor = hiddenColor;
            borderColor = QColor(56, 142, 60);
            layerType = QString("Cachee %1").arg(layerIdx);
        }
        
        // Dessiner le label de la couche
        painter.setPen(QColor(80, 80, 80));
        painter.setFont(QFont("Arial", 11, QFont::Bold));
        int labelX = nodePositions[layerIdx][0].x();
        int labelY = marginY - 20;
        QRect labelRect(labelX - 50, labelY, 100, 20);
        painter.drawText(labelRect, Qt::AlignCenter, layerLabels[layerIdx]);
        
        // Dessiner le nombre de neurones
        painter.setFont(QFont("Arial", 8));
        painter.setPen(QColor(120, 120, 120));
        int countY = nodePositions[layerIdx].back().y() + NODE_SPACING;
        QRect countRect(labelX - 40, countY, 80, 15);
        painter.drawText(countRect, Qt::AlignCenter, QString("%1 neurones").arg(architecture[layerIdx]));
        
        // Dessiner chaque neurone avec ombre et dégradé
        for (const QPoint& pos : nodePositions[layerIdx]) {
            // Ombre du neurone
            QPainterPath shadowPath;
            shadowPath.addEllipse(pos.x() + 2, pos.y() + 2, NODE_RADIUS * 2, NODE_RADIUS * 2);
            painter.fillPath(shadowPath, QColor(0, 0, 0, 30));
            
            // Dégradé pour le neurone
            QRadialGradient gradient(pos.x(), pos.y(), NODE_RADIUS);
            gradient.setColorAt(0, nodeColor.lighter(120));
            gradient.setColorAt(1, nodeColor);
            
            // Cercle du neurone avec dégradé
            painter.setBrush(QBrush(gradient));
            painter.setPen(QPen(borderColor, 2.5));
            painter.drawEllipse(pos, NODE_RADIUS, NODE_RADIUS);
            
            // Point central pour effet 3D
            painter.setBrush(QBrush(nodeColor.lighter(150)));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(pos.x() - 3, pos.y() - 3, 6, 6);
        }
    }
}
