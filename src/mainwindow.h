#pragma once

#include <QMainWindow>
#include <QCheckBox>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include "realtime.h"
#include "utils/aspectratiowidget/aspectratiowidget.hpp"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    void initialize();
    void finish();

private:
    void connectUIElements();
    void connectTimeOfDay();
    void connectTerrainResolution();
    void connectXScale();
    void connectYScale();
    void connectTextureStride();
    void connectTerrainAmplification();
    void connectTerrainWireframe();
    void connectSaveImage();

    Realtime *realtime;
    AspectRatioWidget *aspectRatioWidget;
    QCheckBox *terrainWireframeBox;
    QPushButton *saveImage;
    QSlider *timeSlider;
    QSlider *terrainResolutionSlider;
    QSpinBox *timeBox;
    QSpinBox *terrainResolutionBox;
    QSlider *xScaleSlider;
    QSlider *yScaleSlider;
    QDoubleSpinBox *xScaleBox;
    QDoubleSpinBox *yScaleBox;
    QSlider *textureStrideSlider;
    QSpinBox *textureStrideBox;
    QSlider *terrainAmplificationSlider;
    QDoubleSpinBox *terrainAmplificationBox;

private slots:
    void onTerrainWireframe();
    void onSaveImage();
    void onValChangeTime(int newValue);
    void onTerrainResolutionChange(int newValue);
    void onValChangeXScaleSlider(int newValue);
    void onValChangeYScaleSlider(int newValue);
    void onValChangeXScaleBox(double newValue);
    void onValChangeYScaleBox(double newValue);
    void onValChangeTextureStrideSlider(int newValue);
    void onValChangeTextureStrideBox(int newValue);
    void onValChangeTerrainAmplificationSlider(int newValue);
    void onValChangeTerrainAmplificationBox(double newValue);
};
