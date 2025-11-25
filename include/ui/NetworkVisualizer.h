#ifndef NETWORKVISUALIZER_H
#define NETWORKVISUALIZER_H

#include <QWidget>
#include <QPaintEvent>
#include <QPoint>
#include <memory>

namespace NeuroUIT {
    class Network;
}

/**
 * @brief Widget pour visualiser graphiquement un réseau de neurones
 */
class NetworkVisualizer : public QWidget {
    Q_OBJECT

public:
    explicit NetworkVisualizer(QWidget *parent = nullptr);
    
    /**
     * @brief Met à jour le réseau à visualiser
     */
    void updateNetwork(std::shared_ptr<NeuroUIT::Network> network);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void drawNetwork(QPainter& painter);
    
    std::shared_ptr<NeuroUIT::Network> network_;
    
    // Paramètres de visualisation
    static constexpr int NODE_RADIUS = 15;
    static constexpr int LAYER_SPACING = 200;
    static constexpr int NODE_SPACING = 40;
};

#endif // NETWORKVISUALIZER_H

