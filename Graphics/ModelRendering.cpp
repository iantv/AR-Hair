#include "ModelRendering.h"
#include <QDir>

ModelRendering::ModelRendering() {
    _texture = nullptr;
    _core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
    _model = new Base3DModel("3D models/background.obj");
}

ModelRendering::~ModelRendering() {
    delete _model;
    delete _program;
    if (_texture != nullptr) {
        _texture->destroy();
    }
}

void ModelRendering::init() {
    _program = new QOpenGLShaderProgram;
    _program->addShaderFromSourceFile(QOpenGLShader::Vertex, QDir::currentPath() + "/shaders/background.vert");
    _program->addShaderFromSourceFile(QOpenGLShader::Fragment, QDir::currentPath() + "/shaders/background.frag");
    _program->bindAttributeLocation("vertex", (GLuint)VBO_VERTICES);
    _program->bindAttributeLocation("v_uvs", (GLuint)VBO_TEXCOORS);
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
    f->glEnableVertexAttribArray((GLuint)VBO_VERTICES);
    f->glEnableVertexAttribArray((GLuint)VBO_TEXCOORS);
    f->glVertexAttribPointer((GLuint)VBO_VERTICES, model->_size[VBO_VERTICES],
                             GL_FLOAT, GL_FALSE, model->_count * sizeof(GLfloat),
                             reinterpret_cast<void *>(model->_indx[VBO_VERTICES] * sizeof(GLfloat)));
    f->glVertexAttribPointer((GLuint)VBO_TEXCOORS, model->_size[VBO_TEXCOORS],
                             GL_FLOAT, GL_FALSE, model->_count * sizeof(GLfloat),
                             reinterpret_cast<void *>(model->_indx[VBO_TEXCOORS] * sizeof(GLfloat)));
    vbo->release();
}

void ModelRendering::render() {
    QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);
    _program->bind();
    if (_texture != nullptr) {
        _texture->bind();
    }

    this->setupVertexAttribs(&_vbo, _model);
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glDrawArrays(GL_TRIANGLES, 0, _model->vertexCount());

    if (_texture != nullptr) {
        _texture->release();
    }
    _program->release();
}

void ModelRendering::updateTexture(const QImage &image) {
    if (_vao.isCreated()) {
        _vao.bind();
        if (_texture != nullptr) {
            delete _texture;
        }
        _texture = new QOpenGLTexture(image);
    }
}
