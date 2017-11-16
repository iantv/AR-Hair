#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QtCore>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _ui(new Ui::MainWindow) {
    _vplayer = new VideoPlayer();
    QObject::connect(_vplayer, SIGNAL(processedImage(QImage)), this, SLOT(updatePlayerUi(QImage)));
    _ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete _vplayer;
    delete _ui;
}

void MainWindow::updatePlayerUi(QImage image) {
    if (!image.isNull()) {
        _ui->label->setAlignment(Qt::AlignCenter);
        _ui->label->setPixmap(QPixmap::fromImage(image).scaled(_ui->label->size(),
                              Qt::KeepAspectRatio, Qt::FastTransformation));
    }
}

void MainWindow::show() {
    QMainWindow::show();
    _vplayer->play();
}
