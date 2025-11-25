#include "ui/MainWindow.h"
#include "controller/Controller.h"
#include "ui/NetworkVisualizer.h"
#include "ui/ErrorChartWidget.h"
#include "ui/MetricsWidget.h"
#include "ui/ConfusionMatrixWidget.h"
#include "ui/CorrelationMatrixWidget.h"
#include "ui/PredictionsChartWidget.h"
#include "ui/StatisticsWidget.h"
#include "ui/ResultsDashboardWidget.h"
#include "ui/NetworkConfigDialog.h"
#include "ui/DatasetLoadDialog.h"
#include "ui/TrainingParamsDialog.h"
#include "training/Trainer.h"
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QTabWidget>
#include <QSplitter>
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressBar>
#include <QDialog>
#include <QGroupBox>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , hasNetwork_(false)
    , hasDataset_(false)
    , isTraining_(false)
    , darkMode_(false)
{
    setupUI();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupSplitters();
    applyTheme(false); // Mode clair par défaut
    updateUIState();
    
    // Titre de la fenêtre
    setWindowTitle("NeuroUIT - Simulateur de Réseaux Neuronaux");
    setWindowIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    
    // Taille minimale et par défaut
    setMinimumSize(800, 600);
    resize(1200, 800);
}

MainWindow::~MainWindow() {
}

void MainWindow::setController(std::shared_ptr<NeuroUIT::Controller> controller) {
    controller_ = controller;
    if (controller_) {
        // Connecter les signaux du contrôleur
        connect(controller_.get(), SIGNAL(trainingProgress(size_t, double)),
                this, SLOT(onTrainingProgress(size_t, double)));
        connect(controller_.get(), SIGNAL(trainingFinished()),
                this, SLOT(onTrainingFinished()));
        connect(controller_.get(), SIGNAL(networkCreated()),
                this, SLOT(onNetworkCreated()));
        connect(controller_.get(), SIGNAL(datasetLoaded()),
                this, SLOT(onDatasetLoaded()));
    }
}

void MainWindow::setupUI() {
    // Créer le splitter principal (horizontal)
    mainSplitter_ = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(mainSplitter_);
    
    // === PANEL GAUCHE : Configuration ===
    leftPanel_ = new QWidget;
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel_);
    leftLayout->setContentsMargins(10, 10, 10, 10);
    
    // Titre du panel
    QLabel* leftTitle = new QLabel("⚙️ Configuration");
    leftTitle->setStyleSheet("font-weight: bold; font-size: 14pt; padding: 5px;");
    leftLayout->addWidget(leftTitle);
    
    // Informations réseau
    QGroupBox* networkGroup = new QGroupBox("Réseau");
    QVBoxLayout* networkLayout = new QVBoxLayout(networkGroup);
    lblNetworkInfo_ = new QLabel("Aucun réseau");
    lblNetworkInfo_->setWordWrap(true);
    networkLayout->addWidget(lblNetworkInfo_);
    leftLayout->addWidget(networkGroup);
    
    // Informations dataset
    QGroupBox* datasetGroup = new QGroupBox("Dataset");
    QVBoxLayout* datasetLayout = new QVBoxLayout(datasetGroup);
    lblDatasetInfo_ = new QLabel("Aucun dataset");
    lblDatasetInfo_->setWordWrap(true);
    datasetLayout->addWidget(lblDatasetInfo_);
    leftLayout->addWidget(datasetGroup);
    
    // Boutons d'action compactés
    btnNewNetwork_ = new QPushButton("➕ Nouveau réseau");
    btnLoadDataset_ = new QPushButton("📂 Charger dataset");
    btnWizard_ = new QPushButton("🧙 Assistant");
    btnWizard_->setStyleSheet("font-weight: bold; background-color: #4CAF50; color: white; padding: 8px;");
    
    leftLayout->addWidget(btnNewNetwork_);
    leftLayout->addWidget(btnLoadDataset_);
    leftLayout->addWidget(btnWizard_);
    leftLayout->addStretch();
    
    connect(btnNewNetwork_, &QPushButton::clicked, this, &MainWindow::onNewNetwork);
    connect(btnLoadDataset_, &QPushButton::clicked, this, &MainWindow::onLoadDataset);
    connect(btnWizard_, &QPushButton::clicked, this, &MainWindow::onShowWizard);
    
    // Zone de configuration textuelle (pour afficher les hyperparamètres)
    configTextEdit_ = new QTextEdit;
    configTextEdit_->setReadOnly(true);
    configTextEdit_->setMaximumHeight(150);
    configTextEdit_->setPlaceholderText("Paramètres d'entraînement...");
    leftLayout->addWidget(configTextEdit_);
    
    // === PANEL CENTRAL : Visualisation réseau ===
    centerPanel_ = new QWidget;
    QVBoxLayout* centerLayout = new QVBoxLayout(centerPanel_);
    centerLayout->setContentsMargins(10, 10, 10, 10);
    centerLayout->setSpacing(10);
    
    QLabel* centerTitle = new QLabel("Visualisation du Reseau de Neurones");
    centerTitle->setStyleSheet(
        "font-weight: bold; "
        "font-size: 16pt; "
        "padding: 10px; "
        "background-color: #2196F3; "
        "color: white; "
        "border-radius: 5px; "
    );
    centerTitle->setAlignment(Qt::AlignCenter);
    centerLayout->addWidget(centerTitle);
    
    // Container pour centrer parfaitement le visualiseur
    QWidget* visualizerContainer = new QWidget;
    QVBoxLayout* containerLayout = new QVBoxLayout(visualizerContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setAlignment(Qt::AlignCenter);
    
    networkVisualizer_ = new NetworkVisualizer;
    networkVisualizer_->setMinimumSize(700, 500);
    networkVisualizer_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    containerLayout->addWidget(networkVisualizer_, 0, Qt::AlignCenter);
    
    centerLayout->addWidget(visualizerContainer, 1); // Stretch factor pour centrer
    
    // === PANEL DROIT : Graphique et logs ===
    rightPanel_ = new QWidget;
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel_);
    rightLayout->setContentsMargins(5, 5, 5, 5);
    
    tabWidget_ = new QTabWidget;
    
    // Widgets de visualisation
    errorChartWidget_ = new ErrorChartWidget;
    metricsWidget_ = new MetricsWidget;
    confusionMatrixWidget_ = new ConfusionMatrixWidget;
    correlationMatrixWidget_ = new CorrelationMatrixWidget;
    predictionsChartWidget_ = new PredictionsChartWidget;
    statisticsWidget_ = new StatisticsWidget;
    resultsDashboardWidget_ = new ResultsDashboardWidget;
    
    logTextEdit_ = new QTextEdit;
    logTextEdit_->setReadOnly(true);
    // setMaximumBlockCount n'existe pas dans Qt 6, on limite manuellement si nécessaire
    logTextEdit_->setFont(QFont("Consolas", 9));
    
    // Ajouter les onglets - Dashboard en premier pour être visible
    tabWidget_->addTab(resultsDashboardWidget_, "Resultats");
    tabWidget_->addTab(errorChartWidget_, "Erreur");
    tabWidget_->addTab(metricsWidget_, "Metriques");
    tabWidget_->addTab(confusionMatrixWidget_, "Confusion");
    tabWidget_->addTab(correlationMatrixWidget_, "Correlation");
    tabWidget_->addTab(predictionsChartWidget_, "Predictions");
    tabWidget_->addTab(statisticsWidget_, "Statistiques");
    tabWidget_->addTab(logTextEdit_, "Logs");
    
    rightLayout->addWidget(tabWidget_);
    
    // Progress bar
    progressBar_ = new QProgressBar;
    progressBar_->setVisible(false);
    rightLayout->addWidget(progressBar_);
    
    // Informations d'entraînement
    lblTrainingInfo_ = new QLabel("Prêt");
    lblTrainingInfo_->setAlignment(Qt::AlignCenter);
    rightLayout->addWidget(lblTrainingInfo_);
    
    // Ajouter les panels au splitter principal
    mainSplitter_->addWidget(leftPanel_);
    mainSplitter_->addWidget(centerPanel_);
    mainSplitter_->addWidget(rightPanel_);
    
    // Proportions initiales : 20% gauche, 50% centre, 30% droite
    mainSplitter_->setSizes({250, 600, 350});
    
    // S'assurer que le splitter est visible
    mainSplitter_->show();
    
    // Boutons d'action pour l'entraînement (dans le panel droit)
    QHBoxLayout* trainingButtons = new QHBoxLayout;
    btnStartTraining_ = new QPushButton("▶️ Lancer");
    btnStopTraining_ = new QPushButton("⏹️ Arrêter");
    btnTestNetwork_ = new QPushButton("🧪 Tester");
    
    btnStopTraining_->setEnabled(false);
    btnStartTraining_->setStyleSheet("font-weight: bold; background-color: #2196F3; color: white; padding: 8px;");
    btnStopTraining_->setStyleSheet("font-weight: bold; background-color: #f44336; color: white; padding: 8px;");
    
    trainingButtons->addWidget(btnStartTraining_);
    trainingButtons->addWidget(btnStopTraining_);
    trainingButtons->addWidget(btnTestNetwork_);
    
    rightLayout->insertLayout(0, trainingButtons);
    
    // Connecter les boutons
    connect(btnStartTraining_, &QPushButton::clicked, this, &MainWindow::onStartTraining);
    connect(btnStopTraining_, &QPushButton::clicked, this, &MainWindow::onStopTraining);
    connect(btnTestNetwork_, &QPushButton::clicked, this, &MainWindow::onTestNetwork);
}

void MainWindow::setupMenuBar() {
    // Menu Fichier
    QMenu* fileMenu = menuBar()->addMenu("&Fichier");
    fileMenu->addAction("&Nouveau reseau", QKeySequence::New, this, &MainWindow::onNewNetwork);
    fileMenu->addAction("&Charger reseau", QKeySequence::Open, this, &MainWindow::onLoadNetwork);
    fileMenu->addAction("&Sauvegarder reseau", QKeySequence::Save, this, &MainWindow::onSaveNetwork);
    fileMenu->addSeparator();
    fileMenu->addAction("&Charger dataset", this, &MainWindow::onLoadDataset);
    fileMenu->addAction("&Exporter resultats", this, &MainWindow::onExportResults);
    fileMenu->addSeparator();
    fileMenu->addAction("&Quitter", QKeySequence::Quit, this, &QWidget::close);
    
    // Menu Vue
    QMenu* viewMenu = menuBar()->addMenu("&Vue");
    QAction* lightTheme = viewMenu->addAction("Thème clair");
    QAction* darkTheme = viewMenu->addAction("Thème sombre");
    viewMenu->addSeparator();
    viewMenu->addAction("Assistant", this, &MainWindow::onShowWizard);
    viewMenu->addAction("Plein ecran", QKeySequence::FullScreen, this, &MainWindow::onToggleFullScreen);
    
    themeGroup_ = new QActionGroup(this);
    themeGroup_->addAction(lightTheme);
    themeGroup_->addAction(darkTheme);
    lightTheme->setCheckable(true);
    darkTheme->setCheckable(true);
    lightTheme->setChecked(true);
    
    connect(lightTheme, &QAction::triggered, [this]() { applyTheme(false); });
    connect(darkTheme, &QAction::triggered, [this]() { applyTheme(true); });
    
    // Menu Édition
    QMenu* editMenu = menuBar()->addMenu("&Édition");
    editMenu->addAction("&Configurer réseau", this, &MainWindow::onNewNetwork);
    editMenu->addAction("&Paramètres d'entraînement", this, &MainWindow::onStartTraining);
    
    // Menu Aide
    QMenu* helpMenu = menuBar()->addMenu("&Aide");
    helpMenu->addAction("&Aide", this, &MainWindow::onHelp);
    helpMenu->addAction("&À propos", this, &MainWindow::onAbout);
}

void MainWindow::setupToolBar() {
    toolBar_ = addToolBar("Outils");
    toolBar_->setMovable(false);
    
    actionNewNetwork_ = toolBar_->addAction("Nouveau");
    actionLoadNetwork_ = toolBar_->addAction("Charger");
    actionSaveNetwork_ = toolBar_->addAction("Sauvegarder");
    toolBar_->addSeparator();
    actionLoadDataset_ = toolBar_->addAction("Dataset");
    toolBar_->addSeparator();
    actionStartTraining_ = toolBar_->addAction("Lancer");
    toolBar_->addSeparator();
    actionWizard_ = toolBar_->addAction("Assistant");
    actionTheme_ = toolBar_->addAction("Thème");
    
    actionNewNetwork_->setShortcut(QKeySequence::New);
    actionLoadNetwork_->setShortcut(QKeySequence::Open);
    actionSaveNetwork_->setShortcut(QKeySequence::Save);
    
    connect(actionNewNetwork_, &QAction::triggered, this, &MainWindow::onNewNetwork);
    connect(actionLoadNetwork_, &QAction::triggered, this, &MainWindow::onLoadNetwork);
    connect(actionSaveNetwork_, &QAction::triggered, this, &MainWindow::onSaveNetwork);
    connect(actionLoadDataset_, &QAction::triggered, this, &MainWindow::onLoadDataset);
    connect(actionStartTraining_, &QAction::triggered, this, &MainWindow::onStartTraining);
    connect(actionWizard_, &QAction::triggered, this, &MainWindow::onShowWizard);
}

void MainWindow::setupSplitters() {
    // Les splitters sont déjà configurés dans setupUI
    // Ajouter des styles
    mainSplitter_->setChildrenCollapsible(false);
}

void MainWindow::setupStatusBar() {
    statusBar()->showMessage("Prêt");
}

void MainWindow::updateUIState() {
    btnNewNetwork_->setEnabled(!isTraining_);
    btnLoadDataset_->setEnabled(!isTraining_ && hasNetwork_);
    btnStartTraining_->setEnabled(!isTraining_ && hasNetwork_ && hasDataset_);
    btnStopTraining_->setEnabled(isTraining_);
    btnTestNetwork_->setEnabled(!isTraining_ && hasNetwork_ && hasDataset_);
}

void MainWindow::onNewNetwork() {
    if (!controller_) return;
    
    NetworkConfigDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        auto config = dialog.getConfig();
        if (controller_->createNetwork(config.architecture, config.activations)) {
            statusBar()->showMessage("Réseau créé avec succès", 3000);
        } else {
            QMessageBox::warning(this, "Erreur", "Impossible de créer le réseau");
        }
    }
}

void MainWindow::onLoadNetwork() {
    if (!controller_) return;
    
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Charger un réseau",
        "",
        "Fichiers NeuroUIT (*.nui);;Tous les fichiers (*.*)"
    );
    
    if (!filename.isEmpty()) {
        if (controller_->loadNetwork(filename.toStdString())) {
            statusBar()->showMessage("Réseau chargé avec succès", 3000);
        } else {
            QMessageBox::warning(this, "Erreur", "Impossible de charger le réseau");
        }
    }
}

void MainWindow::onSaveNetwork() {
    if (!controller_) return;
    
    QString filename = QFileDialog::getSaveFileName(
        this,
        "Sauvegarder le réseau",
        "",
        "Fichiers NeuroUIT (*.nui);;Tous les fichiers (*.*)"
    );
    
    if (!filename.isEmpty()) {
        if (controller_->saveNetwork(filename.toStdString())) {
            statusBar()->showMessage("Réseau sauvegardé avec succès", 3000);
        } else {
            QMessageBox::warning(this, "Erreur", "Impossible de sauvegarder le réseau");
        }
    }
}

void MainWindow::onLoadDataset() {
    if (!controller_) return;
    
    NeuroUIT::DatasetLoadDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        auto config = dialog.getConfig();
        if (controller_->loadDataset(config.filename, config.numInputs, 
                                   config.numOutputs, config.hasHeader)) {
            statusBar()->showMessage("Dataset chargé avec succès", 3000);
        } else {
            QMessageBox::warning(this, "Erreur", "Impossible de charger le dataset");
        }
    }
}

void MainWindow::onStartTraining() {
    if (!controller_) return;
    
    NeuroUIT::TrainingParamsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        auto params = dialog.getParams();
        
        // Afficher les paramètres dans le panel de configuration
        QString configText = QString(
            "📊 Paramètres d'entraînement\n\n"
            "Taux d'apprentissage: %1\n"
            "Époques: %2\n"
            "Batch size: %3\n"
            "Momentum: %4\n"
            "Mélanger: %5"
        ).arg(params.learningRate)
         .arg(params.numEpochs)
         .arg(params.batchSize)
         .arg(params.momentum)
         .arg(params.shuffle ? "Oui" : "Non");
        configTextEdit_->setPlainText(configText);
        
        isTraining_ = true;
        updateUIState();
        statusBar()->showMessage("Entraînement en cours...");
        
        // Afficher la barre de progression
        progressBar_->setMaximum(static_cast<int>(params.numEpochs));
        progressBar_->setValue(0);
        progressBar_->setVisible(true);
        
        // Vider les logs précédents
        errorChartWidget_->clear();
        logTextEdit_->clear();
        logTextEdit_->insertHtml("<b>🚀 Démarrage de l'entraînement...</b><br>");
        
        controller_->startTraining(params);
    }
}

void MainWindow::onStopTraining() {
    if (!controller_) return;
    
    controller_->stopTraining();
    statusBar()->showMessage("Arrêt de l'entraînement demandé...");
}

void MainWindow::onTestNetwork() {
    if (!controller_) return;
    
    // Obtenir les résultats détaillés
    auto results = controller_->getDetailedTestResults();
    
    // Mettre à jour le dashboard de résultats (principal - premier onglet)
    resultsDashboardWidget_->updateResults(controller_);
    
    // Mettre à jour les métriques
    MetricsWidget::Metrics metrics;
    metrics.isClassification = results.isClassification;
    metrics.mse = results.mse;
    metrics.mae = results.mae;
    metrics.r2 = results.r2;
    metrics.accuracy = results.accuracy;
    metrics.precision = results.precision;
    metrics.recall = results.recall;
    metrics.f1Score = results.f1Score;
    
    // Ajouter les informations d'entraînement
    auto trainingErrors = controller_->getTrainingErrors();
    if (!trainingErrors.empty()) {
        metrics.epochsCompleted = trainingErrors.size();
        metrics.finalError = trainingErrors.back();
        metrics.minError = *std::min_element(trainingErrors.begin(), trainingErrors.end());
    }
    
    metricsWidget_->updateMetrics(metrics);
    
    // Mettre à jour la matrice de confusion
    if (!results.confusionMatrix.empty()) {
        std::vector<QString> labels = {"Classe 0", "Classe 1"};
        confusionMatrixWidget_->updateMatrix(results.confusionMatrix, labels);
    }
    
    // Mettre à jour le graphique de prédictions
    if (!results.predictions.empty() && !results.actuals.empty()) {
        predictionsChartWidget_->updateData(results.predictions, results.actuals);
    }
    
    // Basculer automatiquement vers l'onglet Résultats pour voir le dashboard
    tabWidget_->setCurrentIndex(0);
    
    QString message = QString("Test terminé - Erreur: %1").arg(results.mse, 0, 'f', 6);
    statusBar()->showMessage(message, 5000);
    
    logTextEdit_->insertHtml(QString("<b style='color: blue'>[Test] Résultats:</b><br>"));
    logTextEdit_->insertHtml(QString("  • MSE: <b>%1</b><br>").arg(results.mse, 0, 'f', 6));
    logTextEdit_->insertHtml(QString("  • MAE: <b>%1</b><br>").arg(results.mae, 0, 'f', 6));
    if (results.isClassification) {
        logTextEdit_->insertHtml(QString("  • Précision: <b>%1%</b><br>").arg(results.accuracy * 100, 0, 'f', 2));
        logTextEdit_->insertHtml(QString("  • F1-Score: <b>%1</b><br>").arg(results.f1Score, 0, 'f', 4));
    } else {
        logTextEdit_->insertHtml(QString("  • R²: <b>%1</b><br>").arg(results.r2, 0, 'f', 4));
    }
    logTextEdit_->insertHtml("<br>");
}

void MainWindow::onExportResults() {
    if (!controller_) return;
    
    QString filename = QFileDialog::getSaveFileName(
        this,
        "Exporter les résultats",
        "",
        "Fichiers CSV (*.csv);;Tous les fichiers (*.*)"
    );
    
    if (!filename.isEmpty()) {
        if (controller_->exportResults(filename.toStdString())) {
            statusBar()->showMessage("Résultats exportés avec succès", 3000);
        } else {
            QMessageBox::warning(this, "Erreur", "Impossible d'exporter les résultats");
        }
    }
}

void MainWindow::onHelp() {
    QMessageBox::information(this, "Aide",
        "NeuroUIT - Simulateur de Réseaux Neuronaux\n\n"
        "1. Créez ou chargez un réseau\n"
        "2. Chargez un dataset CSV\n"
        "3. Configurez et lancez l'entraînement\n"
        "4. Visualisez les résultats\n\n"
        "Format CSV: input1,input2,...,output1,output2,..."
    );
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "À propos de NeuroUIT",
        "NeuroUIT v1.0\n\n"
        "Simulateur de Réseaux Neuronaux avec Interface Graphique\n\n"
        "Auteur: Rachid\n"
        "Licence d'Excellence IA & Ingénierie des Données\n"
        "Université Ibn Tofail Kénitra"
    );
}

void MainWindow::onTrainingProgress(size_t epoch, double error) {
    QString message = QString("Époque %1 - Erreur: %2").arg(epoch).arg(error, 0, 'f', 6);
    statusBar()->showMessage(message);
    
    // Mettre à jour le graphique
    errorChartWidget_->addDataPoint(epoch, error);
    
    // Mettre à jour les logs avec couleur
    QString logEntry = QString("<span style='color: %1'>[Époque %2] Erreur: %3</span><br>")
        .arg(error < 0.1 ? "green" : error < 0.5 ? "orange" : "red")
        .arg(epoch)
        .arg(error, 0, 'f', 6);
    logTextEdit_->insertHtml(logEntry);
    
    // Mettre à jour la barre de progression
    if (progressBar_->isVisible() && progressBar_->maximum() > 0) {
        int progress = static_cast<int>(epoch);
        if (progress <= progressBar_->maximum()) {
            progressBar_->setValue(progress);
        }
    }
    
    // Mettre à jour l'info d'entraînement
    lblTrainingInfo_->setText(QString("Époque: %1 / Erreur: %2")
        .arg(epoch)
        .arg(error, 0, 'f', 4));
}

void MainWindow::onTrainingFinished() {
    isTraining_ = false;
    updateUIState();
    statusBar()->showMessage("✅ Entraînement terminé", 3000);
    
    // Masquer la barre de progression
    progressBar_->setVisible(false);
    
    // Message de succès
    logTextEdit_->insertHtml("<b style='color: green'>✅ Entraînement terminé avec succès !</b><br>");
    
    if (controller_) {
        networkVisualizer_->updateNetwork(controller_->getNetwork());
        
        // Mettre à jour les métriques d'entraînement
        auto trainingErrors = controller_->getTrainingErrors();
        if (!trainingErrors.empty()) {
            MetricsWidget::Metrics metrics;
            metrics.epochsCompleted = trainingErrors.size();
            metrics.finalError = trainingErrors.back();
            metrics.minError = *std::min_element(trainingErrors.begin(), trainingErrors.end());
            // Le temps sera calculé ailleurs si nécessaire
            metricsWidget_->updateMetrics(metrics);
        }
    }
    
    showNotification("Entraînement terminé avec succès !", false);
}

void MainWindow::onNetworkCreated() {
    hasNetwork_ = true;
    updateUIState();
    
    if (controller_) {
        auto arch = controller_->getNetwork()->getArchitecture();
        QString info = QString("✅ Réseau créé\n\nArchitecture:\n");
        for (size_t i = 0; i < arch.size(); ++i) {
            if (i == 0) info += "  • Entrée: ";
            else if (i == arch.size() - 1) info += "  • Sortie: ";
            else info += "  • Cachée: ";
            info += QString::number(arch[i]) + " neurones\n";
        }
        lblNetworkInfo_->setText(info);
        
        networkVisualizer_->updateNetwork(controller_->getNetwork());
        
        showNotification("Réseau créé avec succès !", false);
    }
}

void MainWindow::onDatasetLoaded() {
    hasDataset_ = true;
    updateUIState();
    
    if (controller_) {
        auto stats = controller_->getDatasetStatistics();
        QString info = QString("✅ Dataset chargé\n\n"
                              "📊 Statistiques:\n"
                              "  • Échantillons: %1\n"
                              "  • Entrées: %2\n"
                              "  • Sorties: %3\n")
            .arg(stats.numSamples)
            .arg(stats.numInputs)
            .arg(stats.numOutputs);
        lblDatasetInfo_->setText(info);
        
        // Mettre à jour le widget de statistiques
        statisticsWidget_->updateStatistics(stats);
        
        // Mettre à jour la matrice de corrélation
        auto correlationMatrix = controller_->getCorrelationMatrix();
        if (!correlationMatrix.empty()) {
            std::vector<QString> labels;
            size_t numVars = correlationMatrix.size();
            for (size_t i = 0; i < numVars; ++i) {
                if (i < stats.numInputs) {
                    labels.push_back(QString("Input %1").arg(i + 1));
                } else {
                    labels.push_back(QString("Output %1").arg(i - stats.numInputs + 1));
                }
            }
            correlationMatrixWidget_->updateMatrix(correlationMatrix, labels);
        }
        
        showNotification("Dataset chargé avec succès !", false);
    }
}

