#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QSplitter>
#include <QGroupBox>
#include <QProgressBar>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QStyle>
#include <memory>

class QTabWidget;

namespace NeuroUIT {
    class Controller;
}

// Forward declarations pour les classes UI (pas dans le namespace)
class NetworkVisualizer;
class ErrorChartWidget;
class MetricsWidget;
class ConfusionMatrixWidget;
class CorrelationMatrixWidget;
class PredictionsChartWidget;
class StatisticsWidget;
class ResultsDashboardWidget;

/**
 * @brief Fenêtre principale de l'application NeuroUIT
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /**
     * @brief Configure le contrôleur
     */
    void setController(std::shared_ptr<NeuroUIT::Controller> controller);

private slots:
    void onNewNetwork();
    void onLoadNetwork();
    void onSaveNetwork();
    void onLoadDataset();
    void onStartTraining();
    void onStopTraining();
    void onTestNetwork();
    void onExportResults();
    void onHelp();
    void onAbout();

    void onTrainingProgress(size_t epoch, double error);
    void onTrainingFinished();
    void onNetworkCreated();
    void onDatasetLoaded();

private slots:
    void onThemeChanged();
    void onShowWizard();
    void onToggleFullScreen();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupSplitters();
    void updateUIState();
    void applyTheme(bool darkMode);
    void showNotification(const QString& message, bool isError = false);
    
    // Contrôleur
    std::shared_ptr<NeuroUIT::Controller> controller_;
    
    // Widgets principaux
    QSplitter* mainSplitter_;
    QSplitter* leftSplitter_;
    QSplitter* rightSplitter_;
    
    // Panneaux
    QWidget* leftPanel_;        // Configuration
    QWidget* centerPanel_;      // Visualisation réseau
    QWidget* rightPanel_;       // Graphique et logs
    
    QTabWidget* tabWidget_;
    NetworkVisualizer* networkVisualizer_;
    ErrorChartWidget* errorChartWidget_;
    MetricsWidget* metricsWidget_;
    ConfusionMatrixWidget* confusionMatrixWidget_;
    CorrelationMatrixWidget* correlationMatrixWidget_;
    PredictionsChartWidget* predictionsChartWidget_;
    StatisticsWidget* statisticsWidget_;
    ResultsDashboardWidget* resultsDashboardWidget_;
    QTextEdit* logTextEdit_;
    QTextEdit* configTextEdit_;
    
    // Boutons
    QPushButton* btnNewNetwork_;
    QPushButton* btnLoadDataset_;
    QPushButton* btnStartTraining_;
    QPushButton* btnStopTraining_;
    QPushButton* btnTestNetwork_;
    QPushButton* btnWizard_;
    
    // Labels d'information
    QLabel* lblNetworkInfo_;
    QLabel* lblDatasetInfo_;
    QLabel* lblTrainingInfo_;
    
    // Progress
    QProgressBar* progressBar_;
    
    // Toolbar
    QToolBar* toolBar_;
    QAction* actionNewNetwork_;
    QAction* actionLoadNetwork_;
    QAction* actionSaveNetwork_;
    QAction* actionLoadDataset_;
    QAction* actionStartTraining_;
    QAction* actionTheme_;
    QAction* actionWizard_;
    QActionGroup* themeGroup_;
    
    // État
    bool hasNetwork_;
    bool hasDataset_;
    bool isTraining_;
    bool darkMode_;
};

#endif // MAINWINDOW_H

