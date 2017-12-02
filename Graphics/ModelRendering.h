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
protected:
    void setupVertexAttribs(QOpenGLBuffer *vbo, Base3DModel *model);
    void textureBind(QOpenGLTexture *texture);
    void textureRelease(QOpenGLTexture *texture);
    void bindAttributes();
    bool _core;
    Base3DModel *_model;
    QOpenGLShaderProgram *_program;
    QOpenGLTexture *_texture;
    QOpenGLVertexArrayObject _vao;
    QOpenGLBuffer _vbo;
    QString _shaderVertPath;
    QString _shaderFragPath;
    QVector<QString> _attrName;
};

class BackgroundRendering : public ModelRendering
{
public:
    BackgroundRendering();
    ~BackgroundRendering();
};

#endif // MODELRENDERING_H
