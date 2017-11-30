#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QtCore>
#include <QImage>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _ui(new Ui::MainWindow) {
    _vplayer = new VideoPlayer();
    QObject::connect(_vplayer, SIGNAL(processedImage(const QImage &)), this, SLOT(updatePlayerUi(const QImage &)));
    _ui->setupUi(this);

    _glWidget = new GLWidget(this);
    _ui->container->addWidget(_glWidget);
}

MainWindow::~MainWindow() {
    delete _glWidget;
    delete _vplayer;
    delete _ui;
}

void MainWindow::updatePlayerUi(const QImage &image) {
    if (!image.isNull()) {
        _glWidget->updateBackgroundImage(image);
    }
}

void MainWindow::show() {
    QMainWindow::show();
    _vplayer->play();
}
