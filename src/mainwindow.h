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
    void connectNear();
    void connectFar();
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
    QSlider *nearSlider;
    QSlider *farSlider;
    QDoubleSpinBox *nearBox;
    QDoubleSpinBox *farBox;

private slots:
    void onTerrainWireframe();
    void onSaveImage();
    void onValChangeTime(int newValue);
    void onTerrainResolutionChange(int newValue);
    void onValChangeNearSlider(int newValue);
    void onValChangeFarSlider(int newValue);
    void onValChangeNearBox(double newValue);
    void onValChangeFarBox(double newValue);
};
