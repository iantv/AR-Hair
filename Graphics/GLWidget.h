#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QImage>
#include <QOpenGLTexture>
#include <Graphics/ModelRendering.h>
#include <Graphics/Base3DModel.h>
#include <opencv2/opencv.hpp>

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
    BackgroundRendering *_background;
    HairRendering *_hair;
};

#endif // GLWIDGET_H
