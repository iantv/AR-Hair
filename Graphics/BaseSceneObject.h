#ifndef BASESCENEOBJECT_H
#define BASESCENEOBJECT_H
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

class BaseSceneObject
{
public:
    BaseSceneObject();
    virtual ~BaseSceneObject();
    virtual void init();
    void render();
protected:
    virtual void bindAll();
    virtual void releaseAll();
    virtual void initData();
    void setupVertexAttribs(QOpenGLBuffer *vbo, Base3DModel *model);
    virtual void setUniformVariables();
    void bindAttributes();
    bool _core;
    Base3DModel *_model;
    QOpenGLShaderProgram *_program;
    QOpenGLVertexArrayObject _vao;
    QOpenGLBuffer _vbo;
    QString _shaderVertPath;
    QString _shaderFragPath;
    QVector<QString> _attrName;
};

class BackgroundObject : public BaseSceneObject
{
public:
    BackgroundObject();
    ~BackgroundObject();
    void updateTexture(const QImage &image);
protected:
    void bindAll() override;
    void releaseAll() override;
    void initData() override;
    void textureBind(QOpenGLTexture *texture);
    void textureRelease(QOpenGLTexture *texture);
    QOpenGLTexture *_texture;
};

enum TransformedObjectType {
    HAIR, HEAD
};

class TransformedObject : public BackgroundObject
{
public:
    TransformedObject(TransformedObjectType type);
    ~TransformedObject();
    void updatePosition(cv::Mat &rmat, cv::Mat &tvec);
    void init() override;
    void updateProjectionMatrix(GLfloat aspect);
protected:
    void setUniformVariables() override;
    QMatrix4x4 calcMVP();
    void initData() override;
private:
    TransformedObjectType _type;
    QMatrix4x4 _projection;
    cv::Mat _rmat;
    cv::Mat _tvec;
    bool _posUpdated;
    QImage _imgTex;
};
#endif // BASESCENEOBJECT_H
