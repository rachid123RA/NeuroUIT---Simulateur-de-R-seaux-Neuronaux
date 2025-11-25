#ifndef CONFUSIONMATRIXWIDGET_H
#define CONFUSIONMATRIXWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <vector>

/**
 * @brief Widget pour afficher la matrice de confusion
 */
class ConfusionMatrixWidget : public QWidget {
    Q_OBJECT

public:
    explicit ConfusionMatrixWidget(QWidget *parent = nullptr);
    
    /**
     * @brief Met à jour la matrice de confusion
     * @param matrix Matrice de confusion (lignes = prédictions, colonnes = réelles)
     * @param classLabels Labels des classes (optionnel)
     */
    void updateMatrix(const std::vector<std::vector<int>>& matrix,
                     const std::vector<QString>& classLabels = std::vector<QString>());
    
    /**
     * @brief Réinitialise l'affichage
     */
    void clear();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void drawMatrix(QPainter& painter);
    QColor getColorForValue(int value, int maxValue) const;
    
    std::vector<std::vector<int>> matrix_;
    std::vector<QString> classLabels_;
    bool hasData_;
    
    static constexpr int CELL_SIZE = 80;
    static constexpr int MARGIN = 50;
    static constexpr int LABEL_SIZE = 20;
};

#endif // CONFUSIONMATRIXWIDGET_H

