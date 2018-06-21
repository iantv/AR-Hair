#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QImage>
#include <QOpenGLTexture>
#include <Graphics/BaseSceneObject.h>
#include <Graphics/Base3DModel.h>
#include <opencv2/opencv.hpp>
#include <QPushButton>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();
    void updateBackgroundImage(const QImage &image);
    void updatePosition(cv::Mat &rmat, cv::Mat &tvec);
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
public slots:
    void cleanup();
private:
    static bool _transparent;
    BackgroundObject *_background;
    TransformedObject *_hair;
    TransformedObject *_head;
};

#endif // GLWIDGET_H
