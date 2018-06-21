#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <VideoPlayer.h>
#include <Graphics/GLWidget.h>
#include <QMainWindow>
#include <QKeyEvent>

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
protected:
    void keyPressEvent(QKeyEvent *event) override;
private slots:
    void updatePlayerUi(const QImage &image, cv::Mat rmat, cv::Mat tvec);
private:
    Ui::MainWindow *_ui;
    VideoPlayer *_vplayer;
    GLWidget *_glWidget;
};

#endif // MAINWINDOW_H
