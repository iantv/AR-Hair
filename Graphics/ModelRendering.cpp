#include "ModelRendering.h"
#include <QDir>

ModelRendering::ModelRendering() {
    _texture = nullptr;
    _core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
}

ModelRendering::~ModelRendering() {
    delete _model;
    delete _program;
    if (_texture != nullptr) {
        _texture->destroy();
        delete _texture;
    }
}

void ModelRendering::init() {
    _program = new QOpenGLShaderProgram;
    _program->addShaderFromSourceFile(QOpenGLShader::Vertex, QDir::currentPath() + _shaderVertPath);
    _program->addShaderFromSourceFile(QOpenGLShader::Fragment, QDir::currentPath() + _shaderFragPath);
    this->bindAttributes();
    _program->link();

    _program->bind();
    _program->setUniformValue("texture", 0);

    _vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);
    _vbo.create();
    _vbo.bind();
    _vbo.allocate(_model->data(), _model->sizeData() * sizeof(GLfloat));
    this->setupVertexAttribs(&_vbo, _model);
    _program->release();
}

void ModelRendering::setupVertexAttribs(QOpenGLBuffer *vbo, Base3DModel *model) {
    vbo->bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    for (int i = 0; i < _attrName.size(); i++) {
        f->glEnableVertexAttribArray((GLuint)i);
        f->glVertexAttribPointer((GLuint)i, model->_size[i],
                                 GL_FLOAT, GL_FALSE, model->_count * sizeof(GLfloat),
                                 reinterpret_cast<void *>(model->_indx[i] * sizeof(GLfloat)));
    }
    vbo->release();
}

void ModelRendering::render() {
    QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);
    _program->bind();
    this->textureBind(_texture);

    this->setupVertexAttribs(&_vbo, _model);
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glDrawArrays(GL_TRIANGLES, 0, _model->vertexCount());

    this->textureRelease(_texture);
    _program->release();
}

void ModelRendering::updateTexture(const QImage &image) {
    if (_vao.isCreated()) {
        _vao.bind();
        if (_texture != nullptr) {
            _texture->destroy();
            delete _texture;
            _texture = nullptr;
        }
        _texture = new QOpenGLTexture(image);
    }
}

void ModelRendering::textureBind(QOpenGLTexture *texture) {
    if (texture != nullptr) {
        texture->bind();
    }
}

void ModelRendering::textureRelease(QOpenGLTexture *texture) {
    if (texture != nullptr) {
        texture->release();
    }
}

void ModelRendering::bindAttributes() {
    for (int i = 0; i < _attrName.size(); i++)
        _program->bindAttributeLocation(_attrName[i], (GLuint)i);
}

BackgroundRendering::BackgroundRendering() : ModelRendering::ModelRendering() {
    _model = new Base3DModel("3D models/background.obj");
    _shaderVertPath = "/shaders/background.vert";
    _shaderFragPath = "/shaders/background.frag";
    _attrName << "vertex" << "v_uvs";
}

BackgroundRendering::~BackgroundRendering() {

}
