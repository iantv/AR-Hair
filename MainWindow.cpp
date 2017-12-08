#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QtCore>
#include <QImage>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _ui(new Ui::MainWindow) {
    _vplayer = new VideoPlayer();
    QObject::connect(_vplayer, SIGNAL(processedImage(const QImage &, cv::Mat, cv::Mat)),
                     this, SLOT(updatePlayerUi(const QImage &, cv::Mat, cv::Mat)));
    _ui->setupUi(this);

    _glWidget = new GLWidget(this);
    _ui->container->addWidget(_glWidget);
}

MainWindow::~MainWindow() {
    delete _glWidget;
    delete _vplayer;
    delete _ui;
}

void MainWindow::updatePlayerUi(const QImage &image, cv::Mat rmat, cv::Mat tvec) {
    if (!image.isNull()) {
        _glWidget->updatePosition(rmat, tvec);
        _glWidget->updateBackgroundImage(image);
    }
}

void MainWindow::show() {
    QMainWindow::show();
    _vplayer->play();
}
