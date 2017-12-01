#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QImage>
#include <QPainter>
#include <Graphics/Base3DModel.h>
#include <QOpenGLTexture>

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
    void setupVertexAttribs(QOpenGLBuffer *vbo, Base3DModel *model);
    QMatrix4x4 _camera;
    bool _core;
    QMatrix4x4 _proj;
    static bool _transparent;
    Base3DModel _bgImageModel;
    QOpenGLShaderProgram *_bgProgram;
    QOpenGLTexture *_bgTex;
    QOpenGLVertexArrayObject _bgVAO;
    QOpenGLBuffer _bgVBO;
};

#endif // GLWIDGET_H
