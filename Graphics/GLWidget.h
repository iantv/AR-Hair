#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QImage>
//#include <QPainter>
#include <QOpenGLTexture>
#include <Graphics/ModelRendering.h>
#include <Graphics/Base3DModel.h>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();
    void updateBackgroundImage(const QImage &image);
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
public slots:
    void cleanup();
private:
    QMatrix4x4 _camera;
    QMatrix4x4 _proj;
    static bool _transparent;
    ModelRendering *_background;
};

#endif // GLWIDGET_H
