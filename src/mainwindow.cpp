#include "mainwindow.h"
#include "settings.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QSettings>
#include <QLabel>
#include <QGroupBox>
#include <iostream>

void MainWindow::initialize() {
    realtime = new Realtime;
    aspectRatioWidget = new AspectRatioWidget(this);
    aspectRatioWidget->setAspectWidget(realtime, 3.f/4.f);
    QHBoxLayout *hLayout = new QHBoxLayout; // horizontal alignment
    QVBoxLayout *vLayout = new QVBoxLayout(); // vertical alignment
    vLayout->setAlignment(Qt::AlignTop);
    hLayout->addLayout(vLayout);
    hLayout->addWidget(aspectRatioWidget, 1);
    this->setLayout(hLayout);

    // Create labels in sidebox
    QFont font;
    font.setPointSize(12);
    font.setBold(true);

    QLabel *scene_label = new QLabel();
    scene_label->setText("Scene");
    scene_label->setFont(font);
    QLabel *time_of_day_label = new QLabel();
    time_of_day_label->setText("Time of Day:");

    QLabel *tesselation_label = new QLabel(); // Parameters label
    tesselation_label->setText("Tesselation");
    tesselation_label->setFont(font);
    QLabel *filters_label = new QLabel(); // Filters label
    filters_label->setText("Filters");
    filters_label->setFont(font);
    QLabel *param1_label = new QLabel(); // Parameter 1 label
    param1_label->setText("Terrain Resolution:");
    QLabel *x_scale_label = new QLabel(); // Near plane label
    x_scale_label->setText("X Terrain Scale:");
    QLabel *y_scale_label = new QLabel(); // Far plane label
    y_scale_label->setText("Y Terrain Scale:");

    QLabel *terrainAmplificationLabel = new QLabel(); // Parameter 1 label
    terrainAmplificationLabel->setText("Terrain Amplification:");

    QLabel *texture_label = new QLabel(); // Parameters label
    texture_label->setText("Textures");
    texture_label->setFont(font);

    QLabel *textureStrideLabel = new QLabel(); // Parameter 1 label
    textureStrideLabel->setText("Texture Stride:");



    // Create checkbox for kernel-based filter
    terrainWireframeBox = new QCheckBox();
    terrainWireframeBox->setText(QStringLiteral("Terrain Wireframe Shading"));
    terrainWireframeBox->setChecked(false);

    saveImage = new QPushButton();
    saveImage->setText(QStringLiteral("Save image"));

    // Creates the boxes containing the parameter sliders and number boxes
    QGroupBox *timeLayout = new QGroupBox();
    QHBoxLayout *l0 = new QHBoxLayout();

    QGroupBox *p1Layout = new QGroupBox(); // horizonal slider 1 alignment
    QHBoxLayout *l1 = new QHBoxLayout();

    QGroupBox *textureStrideLayout = new QGroupBox(); // horizonal slider 1 alignment
    QHBoxLayout *l2 = new QHBoxLayout();

    QGroupBox *terrainAmplificationLayout = new QGroupBox(); // horizonal slider 1 alignment
    QHBoxLayout *l3 = new QHBoxLayout();

    // Create slider controls to control parameters
    timeSlider = new QSlider(Qt::Orientation::Horizontal);
    timeSlider->setTickInterval(1);
    timeSlider->setMinimum(1);
    timeSlider->setMaximum(24);
    timeSlider->setValue(1);

    timeBox = new QSpinBox();
    timeBox->setMinimum(1);
    timeBox->setMaximum(24);
    timeBox->setSingleStep(1);
    timeBox->setValue(1);


    terrainResolutionSlider = new QSlider(Qt::Orientation::Horizontal); // Parameter 1 slider
    terrainResolutionSlider->setTickInterval(1);
    terrainResolutionSlider->setMinimum(5);
    terrainResolutionSlider->setMaximum(250);
    terrainResolutionSlider->setValue(100);

    terrainResolutionBox = new QSpinBox();
    terrainResolutionBox->setMinimum(5);
    terrainResolutionBox->setMaximum(250);
    terrainResolutionBox->setSingleStep(1);
    terrainResolutionBox->setValue(100);

    textureStrideSlider = new QSlider(Qt::Orientation::Horizontal); // Parameter 1 slider
    textureStrideSlider->setTickInterval(1);
    textureStrideSlider->setMinimum(1);
    textureStrideSlider->setMaximum(25);
    textureStrideSlider->setValue(10);

    textureStrideBox = new QSpinBox();
    textureStrideBox->setMinimum(1);
    textureStrideBox->setMaximum(25);
    textureStrideBox->setSingleStep(1);
    textureStrideBox->setValue(10);

    terrainAmplificationSlider = new QSlider(Qt::Orientation::Horizontal); // Parameter 1 slider
    terrainAmplificationSlider->setTickInterval(1);
    terrainAmplificationSlider->setMinimum(1);
    terrainAmplificationSlider->setMaximum(500);
    terrainAmplificationSlider->setValue(100);

    terrainAmplificationBox = new QDoubleSpinBox();
    terrainAmplificationBox->setMinimum(0.01f);
    terrainAmplificationBox->setMaximum(5.f);
    terrainAmplificationBox->setSingleStep(0.01f);
    terrainAmplificationBox->setValue(1.0f);

    // Adds the slider and number box to the parameter layouts
    l0->addWidget(timeSlider);
    l0->addWidget(timeBox);
    timeLayout->setLayout(l0);

    l1->addWidget(terrainResolutionSlider);
    l1->addWidget(terrainResolutionBox);
    p1Layout->setLayout(l1);

    l2->addWidget(textureStrideSlider);
    l2->addWidget(textureStrideBox);
    textureStrideLayout->setLayout(l2);

    l3->addWidget(terrainAmplificationSlider);
    l3->addWidget(terrainAmplificationBox);
    terrainAmplificationLayout->setLayout(l3);

    // Creates the boxes containing the camera sliders and number boxes
    QGroupBox *nearLayout = new QGroupBox(); // horizonal near slider alignment
    QHBoxLayout *lnear = new QHBoxLayout();
    QGroupBox *farLayout = new QGroupBox(); // horizonal far slider alignment
    QHBoxLayout *lfar = new QHBoxLayout();

    // Create slider controls to control near/far planes
    xScaleSlider = new QSlider(Qt::Orientation::Horizontal); // Near plane slider
    xScaleSlider->setTickInterval(1);
    xScaleSlider->setMinimum(1);
    xScaleSlider->setMaximum(10000);
    xScaleSlider->setValue(100);

    xScaleBox = new QDoubleSpinBox();
    xScaleBox->setMinimum(0.01f);
    xScaleBox->setMaximum(100.f);
    xScaleBox->setSingleStep(0.1f);
    xScaleBox->setValue(1.0f);

    yScaleSlider = new QSlider(Qt::Orientation::Horizontal); // Far plane slider
    yScaleSlider->setTickInterval(1);
    yScaleSlider->setMinimum(1);
    yScaleSlider->setMaximum(10000);
    yScaleSlider->setValue(100);

    yScaleBox = new QDoubleSpinBox();
    yScaleBox->setMinimum(0.01f);
    yScaleBox->setMaximum(100.f);
    yScaleBox->setSingleStep(0.1f);
    yScaleBox->setValue(1.0f);

    // Adds the slider and number box to the parameter layouts
    lnear->addWidget(xScaleSlider);
    lnear->addWidget(xScaleBox);
    nearLayout->setLayout(lnear);

    lfar->addWidget(yScaleSlider);
    lfar->addWidget(yScaleBox);
    farLayout->setLayout(lfar);

    vLayout->addWidget(saveImage);
    vLayout->addWidget(scene_label);
    vLayout->addWidget(time_of_day_label);
    vLayout->addWidget(timeLayout);
    vLayout->addWidget(tesselation_label);
    vLayout->addWidget(param1_label);
    vLayout->addWidget(p1Layout);
    vLayout->addWidget(x_scale_label);
    vLayout->addWidget(nearLayout);
    vLayout->addWidget(y_scale_label);
    vLayout->addWidget(farLayout);
    vLayout->addWidget(texture_label);
    vLayout->addWidget(textureStrideLabel);
    vLayout->addWidget(textureStrideLayout);
    vLayout->addWidget(terrainAmplificationLabel);
    vLayout->addWidget(terrainAmplificationLayout);
    vLayout->addWidget(filters_label);
    vLayout->addWidget(terrainWireframeBox);
    // Extra Credit:
    connectUIElements();

    // Set default values of 5 for tesselation parameters
    onValChangeTime(5);
    onTerrainResolutionChange(5);

    // Set default values for near and far planes
    onValChangeXScaleBox(1.f);
    onValChangeYScaleBox(1.f);
}

void MainWindow::finish() {
    realtime->finish();
    delete(realtime);
}

void MainWindow::connectUIElements() {
    connectTerrainWireframe();
    connectSaveImage();
    connectTimeOfDay();
    connectTerrainResolution();
    connectXScale();
    connectYScale();
    connectTextureStride();
    connectTerrainAmplification();
}

void MainWindow::connectTerrainWireframe() {
    connect(terrainWireframeBox, &QCheckBox::clicked, this, &MainWindow::onTerrainWireframe);
}

void MainWindow::connectSaveImage() {
    connect(saveImage, &QPushButton::clicked, this, &MainWindow::onSaveImage);
}

void MainWindow::connectTimeOfDay(){
    connect(timeSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeTime);
    connect(timeBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeTime);
}

void MainWindow::connectTerrainResolution() {
    connect(terrainResolutionSlider, &QSlider::valueChanged, this, &MainWindow::onTerrainResolutionChange);
    connect(terrainResolutionBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onTerrainResolutionChange);
}

void MainWindow::connectXScale() {
    connect(xScaleSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeXScaleSlider);
    connect(xScaleBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeXScaleBox);
}

void MainWindow::connectYScale() {
    connect(yScaleSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeYScaleSlider);
    connect(yScaleBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeYScaleBox);
}

void MainWindow::connectTextureStride() {
    connect(textureStrideSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeTextureStrideSlider);
    connect(textureStrideBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeTextureStrideBox);
}

void MainWindow::connectTerrainAmplification() {
    connect(terrainAmplificationSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeTerrainAmplificationSlider);
    connect(terrainAmplificationBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeTerrainAmplificationBox);
}

void MainWindow::onTerrainWireframe() {
    settings.terrainWireframe = !settings.terrainWireframe;
    realtime->settingsChanged();
}

void MainWindow::onSaveImage() {
    if (settings.sceneFilePath.empty()) {
        std::cout << "No scene file loaded." << std::endl;
        return;
    }
    std::string sceneName = settings.sceneFilePath.substr(0, settings.sceneFilePath.find_last_of("."));
    sceneName = sceneName.substr(sceneName.find_last_of("/")+1);
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Image"),
                                                    QDir::currentPath()
                                                        .append(QDir::separator())
                                                        .append("student_outputs")
                                                        .append(QDir::separator())
                                                        .append("lights-camera")
                                                        .append(QDir::separator())
                                                        .append("required")
                                                        .append(QDir::separator())
                                                        .append(sceneName), tr("Image Files (*.png)"));
    std::cout << "Saving image to: \"" << filePath.toStdString() << "\"." << std::endl;
    realtime->saveViewportImage(filePath.toStdString());
}

void MainWindow::onValChangeTime(int newValue) {
    timeSlider->setValue(newValue);
    timeBox->setValue(newValue);
    settings.timeOfDay = timeSlider->value();
    realtime->settingsChanged();
}

void MainWindow::onTerrainResolutionChange(int newValue) {
    terrainResolutionSlider->setValue(newValue);
    terrainResolutionBox->setValue(newValue);
    settings.terrainResolution = terrainResolutionSlider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeXScaleSlider(int newValue) {
    //xScaleSlider->setValue(newValue);
    xScaleBox->setValue(newValue / 100.f);
    settings.scaleX = xScaleBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeYScaleSlider(int newValue) {
    //yScaleSlider->setValue(newValue);
    yScaleBox->setValue(newValue / 100.f);
    settings.scaleY = yScaleBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeXScaleBox(double newValue) {
    xScaleSlider->setValue(int(newValue * 100.f));
    //xScaleBox->setValue(newValue);
    settings.scaleX = xScaleBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeYScaleBox(double newValue) {
    yScaleSlider->setValue(int(newValue * 100.f));
    //yScaleBox->setValue(newValue);
    settings.scaleY = yScaleBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeTextureStrideSlider(int newValue) {
    textureStrideSlider->setValue(newValue);
    textureStrideBox->setValue(newValue);
    settings.textureStride = textureStrideSlider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeTextureStrideBox(int newValue) {
    textureStrideSlider->setValue(newValue);
    textureStrideBox->setValue(newValue);
    settings.textureStride = textureStrideSlider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeTerrainAmplificationSlider(int newValue) {
    // terrainAmplificationSlider->setValue(newValue / 10.f);
    terrainAmplificationBox->setValue(newValue / 100.f);
    settings.terrainAmplification = terrainAmplificationBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeTerrainAmplificationBox(double newValue) {
    terrainAmplificationSlider->setValue(int(newValue * 100.f));
    // terrainAmplificationBox->setValue(newValue);
    settings.terrainAmplification = terrainAmplificationBox->value();
    realtime->settingsChanged();
}