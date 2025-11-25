#ifndef CORRELATIONMATRIXWIDGET_H
#define CORRELATIONMATRIXWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <vector>
#include <string>

/**
 * @brief Widget pour afficher la matrice de corrélation
 */
class CorrelationMatrixWidget : public QWidget {
    Q_OBJECT

public:
    explicit CorrelationMatrixWidget(QWidget *parent = nullptr);
    
    /**
     * @brief Met à jour la matrice de corrélation
     * @param matrix Matrice de corrélation (valeurs entre -1 et 1)
     * @param labels Labels des variables (optionnel)
     */
    void updateMatrix(const std::vector<std::vector<double>>& matrix,
                     const std::vector<QString>& labels = std::vector<QString>());
    
    /**
     * @brief Réinitialise l'affichage
     */
    void clear();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void drawMatrix(QPainter& painter);
    QColor getColorForCorrelation(double correlation) const;
    
    std::vector<std::vector<double>> matrix_;
    std::vector<QString> labels_;
    bool hasData_;
    
    static constexpr int CELL_SIZE = 60;
    static constexpr int MARGIN = 50;
    static constexpr int LABEL_SIZE = 20;
};

#endif // CORRELATIONMATRIXWIDGET_H

