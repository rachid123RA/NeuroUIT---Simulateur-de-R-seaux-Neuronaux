QT += core widgets charts

CONFIG += c++17

TARGET = NeuroUIT
TEMPLATE = app

# Include directories
INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD

# Headers
HEADERS += \
    include/core/Neuron.h \
    include/core/Layer.h \
    include/core/Network.h \
    include/core/ActivationFunction.h \
    include/dataset/DatasetManager.h \
    include/training/Trainer.h \
    include/persistence/Persistence.h \
    include/ui/MainWindow.h \
    include/ui/NetworkConfigDialog.h \
    include/ui/DatasetLoadDialog.h \
    include/ui/TrainingParamsDialog.h \
    include/ui/NetworkVisualizer.h \
    include/ui/ErrorChartWidget.h \
    include/ui/MetricsWidget.h \
    include/ui/ConfusionMatrixWidget.h \
    include/ui/CorrelationMatrixWidget.h \
    include/ui/PredictionsChartWidget.h \
    include/ui/StatisticsWidget.h \
    include/ui/ResultsDashboardWidget.h \
    include/controller/Controller.h

# Sources
SOURCES += \
    src/main.cpp \
    src/core/Neuron.cpp \
    src/core/Layer.cpp \
    src/core/Network.cpp \
    src/core/ActivationFunction.cpp \
    src/dataset/DatasetManager.cpp \
    src/training/Trainer.cpp \
    src/persistence/Persistence.cpp \
    src/ui/MainWindow.cpp \
    src/ui/MainWindow_helpers.cpp \
    src/ui/NetworkConfigDialog.cpp \
    src/ui/DatasetLoadDialog.cpp \
    src/ui/TrainingParamsDialog.cpp \
    src/ui/NetworkVisualizer.cpp \
    src/ui/ErrorChartWidget.cpp \
    src/ui/MetricsWidget.cpp \
    src/ui/ConfusionMatrixWidget.cpp \
    src/ui/CorrelationMatrixWidget.cpp \
    src/ui/PredictionsChartWidget.cpp \
    src/ui/StatisticsWidget.cpp \
    src/ui/ResultsDashboardWidget.cpp \
    src/controller/Controller.cpp

# Output directory
DESTDIR = $$PWD/bin
OBJECTS_DIR = $$PWD/build/obj
MOC_DIR = $$PWD/build/moc
RCC_DIR = $$PWD/build/rcc
UI_DIR = $$PWD/build/ui

# Windows specific
win32 {
    CONFIG += console
    DEFINES += _CRT_SECURE_NO_WARNINGS
}

