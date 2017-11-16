#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <VideoPlayer.h>

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void show();
private slots:
    void updatePlayerUi(const QImage &image);
private:
    Ui::MainWindow *_ui;
    VideoPlayer *_vplayer;
};

#endif // MAINWINDOW_H
