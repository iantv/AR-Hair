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

    this->setUniformVariables();
    this->setupVertexAttribs(&_vbo, _model);
    this->textureBind(_texture);

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

void ModelRendering::setUniformVariables() {

}

BackgroundRendering::BackgroundRendering() : ModelRendering::ModelRendering() {
    _model = new Base3DModel("3D models/background.obj");
    _shaderVertPath = "/shaders/background.vert";
    _shaderFragPath = "/shaders/background.frag";
    _attrName << "vertex" << "v_uvs";
}

BackgroundRendering::~BackgroundRendering() {

}

HairRendering::HairRendering() : ModelRendering::ModelRendering() {
    _model = new Base3DModel("3D models/hair/hair_v1.obj");
    _imgTex = QImage(QDir::currentPath() + "/3D models/hair/hair_texture_v1.png");
    _shaderVertPath = "/shaders/hair.vert";
    _shaderFragPath = "/shaders/hair.frag";
    _attrName << "vertex" << "v_uvs";
    _posUpdated = false;
}

HairRendering::~HairRendering() {

}

void HairRendering::updatePosition(cv::Mat &rmat, cv::Mat &tvec) {
    _rmat = rmat;
    _tvec = tvec;
    _posUpdated = true;
}

void HairRendering::setUniformVariables() {
    if (_posUpdated) {
        QMatrix4x4 mvp = calcMVP();
        _program->setUniformValue("mvp", mvp);
        _posUpdated = false;
    }
}

void HairRendering::init() {
    ModelRendering::init();
    this->updateTexture(_imgTex);
}

QMatrix4x4 HairRendering::calcMVP() {
    double rot[9] = { 0 };
    double tv[3] = { 0 };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            rot[i * 3 + j] = -_rmat.at<double>(i, j);
        }
    }

    for (int i = 0; i < 3; i++) {
        tv[i] = -_tvec.at<double>(i);
    }
    QMatrix4x4 view(
        rot[0], rot[1], rot[2], tv[0],
        rot[3], rot[4], rot[5], tv[1],
        rot[6], rot[7], rot[8], tv[2],
        0.f, 0.f, 0.f, 1.f
    );
    QMatrix4x4 model; model.setToIdentity();
    this->updateProjectionMatrix(4.0f / 3.0f);
    QMatrix4x4 res = _projection * view * model;
    return res;
}

void HairRendering::updateProjectionMatrix(GLfloat aspect) {
    _projection.setToIdentity();
    _projection.perspective(45.f, aspect, 0.1f, 100.0f);
}
