#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <VideoPlayer.h>
#include <Graphics/GLWidget.h>

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
    void updatePlayerUi(const QImage &image, cv::Mat rmat, cv::Mat tvec);
private:
    Ui::MainWindow *_ui;
    VideoPlayer *_vplayer;
    GLWidget *_glWidget;
};

#endif // MAINWINDOW_H
