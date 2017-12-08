#ifndef MODELRENDERING_H
#define MODELRENDERING_H
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLContext>
#include <QOpenGLTexture>
#include <QString>
#include <Graphics/Base3DModel.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

class ModelRendering
{
public:
    ModelRendering();
    virtual ~ModelRendering();
    virtual void init();
    void render();
    void updateTexture(const QImage &image);
protected:
    void setupVertexAttribs(QOpenGLBuffer *vbo, Base3DModel *model);
    virtual void textureBind(QOpenGLTexture *texture);
    void textureRelease(QOpenGLTexture *texture);
    virtual void setUniformVariables();
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

class HairRendering : public ModelRendering
{
public:
    HairRendering();
    ~HairRendering();
    void updatePosition(cv::Mat &rmat, cv::Mat &tvec);
    void init() override;
    void updateProjectionMatrix(GLfloat aspect);
protected:
    void setUniformVariables() override;
    QMatrix4x4 calcMVP();
private:
    QMatrix4x4 _projection;
    cv::Mat _rmat;
    cv::Mat _tvec;
    bool _posUpdated;
    QImage _imgTex;
};
#endif // MODELRENDERING_H
