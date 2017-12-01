#ifndef MODELRENDERING_H
#define MODELRENDERING_H
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLContext>
#include <QOpenGLTexture>

#include <Graphics/Base3DModel.h>

class ModelRendering
{
public:
    ModelRendering();
    ~ModelRendering();
    void init();
    void render();
    void updateTexture(const QImage &image);
private:
    void setupVertexAttribs(QOpenGLBuffer *vbo, Base3DModel *model);
    Base3DModel *_model;
    QOpenGLShaderProgram *_program;
    QOpenGLTexture *_texture;
    QOpenGLVertexArrayObject _vao;
    QOpenGLBuffer _vbo;
};

#endif // MODELRENDERING_H
