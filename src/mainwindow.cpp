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
    QLabel *camera_label = new QLabel(); // Camera label
    camera_label->setText("Camera");
    camera_label->setFont(font);
    QLabel *filters_label = new QLabel(); // Filters label
    filters_label->setText("Filters");
    filters_label->setFont(font);
    QLabel *ec_label = new QLabel(); // Extra Credit label
    ec_label->setText("Extra Credit");
    ec_label->setFont(font);
    QLabel *param1_label = new QLabel(); // Parameter 1 label
    param1_label->setText("Parameter 1:");
    QLabel *param2_label = new QLabel(); // Parameter 2 label
    param2_label->setText("Parameter 2:");
    QLabel *near_label = new QLabel(); // Near plane label
    near_label->setText("Near Plane:");
    QLabel *far_label = new QLabel(); // Far plane label
    far_label->setText("Far Plane:");


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
    QGroupBox *p2Layout = new QGroupBox(); // horizonal slider 2 alignment
    QHBoxLayout *l2 = new QHBoxLayout();

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

    // Adds the slider and number box to the parameter layouts
    l0->addWidget(timeSlider);
    l0->addWidget(timeBox);
    timeLayout->setLayout(l0);

    l1->addWidget(terrainResolutionSlider);
    l1->addWidget(terrainResolutionBox);
    p1Layout->setLayout(l1);

    // Creates the boxes containing the camera sliders and number boxes
    QGroupBox *nearLayout = new QGroupBox(); // horizonal near slider alignment
    QHBoxLayout *lnear = new QHBoxLayout();
    QGroupBox *farLayout = new QGroupBox(); // horizonal far slider alignment
    QHBoxLayout *lfar = new QHBoxLayout();

    // Create slider controls to control near/far planes
    nearSlider = new QSlider(Qt::Orientation::Horizontal); // Near plane slider
    nearSlider->setTickInterval(1);
    nearSlider->setMinimum(1);
    nearSlider->setMaximum(1000);
    nearSlider->setValue(10);

    nearBox = new QDoubleSpinBox();
    nearBox->setMinimum(0.01f);
    nearBox->setMaximum(10.f);
    nearBox->setSingleStep(0.1f);
    nearBox->setValue(0.1f);

    farSlider = new QSlider(Qt::Orientation::Horizontal); // Far plane slider
    farSlider->setTickInterval(1);
    farSlider->setMinimum(1000);
    farSlider->setMaximum(10000);
    farSlider->setValue(10000);

    farBox = new QDoubleSpinBox();
    farBox->setMinimum(10.f);
    farBox->setMaximum(100.f);
    farBox->setSingleStep(0.1f);
    farBox->setValue(100.f);

    // Adds the slider and number box to the parameter layouts
    lnear->addWidget(nearSlider);
    lnear->addWidget(nearBox);
    nearLayout->setLayout(lnear);

    lfar->addWidget(farSlider);
    lfar->addWidget(farBox);
    farLayout->setLayout(lfar);

    vLayout->addWidget(saveImage);
    vLayout->addWidget(scene_label);
    vLayout->addWidget(time_of_day_label);
    vLayout->addWidget(timeLayout);
    vLayout->addWidget(tesselation_label);
    vLayout->addWidget(param1_label);
    vLayout->addWidget(p1Layout);
    vLayout->addWidget(param2_label);
    vLayout->addWidget(p2Layout);
    vLayout->addWidget(camera_label);
    vLayout->addWidget(near_label);
    vLayout->addWidget(nearLayout);
    vLayout->addWidget(far_label);
    vLayout->addWidget(farLayout);
    vLayout->addWidget(filters_label);
    vLayout->addWidget(terrainWireframeBox);
    // Extra Credit:
    vLayout->addWidget(ec_label);
    connectUIElements();

    // Set default values of 5 for tesselation parameters
    onValChangeTime(5);
    onTerrainResolutionChange(5);

    // Set default values for near and far planes
    onValChangeNearBox(0.1f);
    onValChangeFarBox(10.f);
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
    connectNear();
    connectFar();
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

void MainWindow::connectNear() {
    connect(nearSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeNearSlider);
    connect(nearBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeNearBox);
}

void MainWindow::connectFar() {
    connect(farSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeFarSlider);
    connect(farBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeFarBox);
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

void MainWindow::onValChangeNearSlider(int newValue) {
    //nearSlider->setValue(newValue);
    nearBox->setValue(newValue/100.f);
    settings.nearPlane = nearBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFarSlider(int newValue) {
    //farSlider->setValue(newValue);
    farBox->setValue(newValue/100.f);
    settings.farPlane = farBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeNearBox(double newValue) {
    nearSlider->setValue(int(newValue*100.f));
    //nearBox->setValue(newValue);
    settings.nearPlane = nearBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFarBox(double newValue) {
    farSlider->setValue(int(newValue*100.f));
    //farBox->setValue(newValue);
    settings.farPlane = farBox->value();
    realtime->settingsChanged();
}