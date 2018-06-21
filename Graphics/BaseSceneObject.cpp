#include "BaseSceneObject.h"
#include <QDir>

BaseSceneObject::BaseSceneObject() {
    _core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
}

void BaseSceneObject::initData() {

}

BaseSceneObject::~BaseSceneObject() {
    delete _model;
    delete _program;
}

void BaseSceneObject::init() {
    this->initData();
    _program = new QOpenGLShaderProgram;
    _program->addShaderFromSourceFile(QOpenGLShader::Vertex, _shaderVertPath);
    _program->addShaderFromSourceFile(QOpenGLShader::Fragment, _shaderFragPath);
    this->bindAttributes();
    _program->link();
    _program->bind();

    _vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);
    _vbo.create();
    _vbo.bind();
    _vbo.allocate(_model->data(), _model->sizeData() * sizeof(GLfloat));
    this->setupVertexAttribs(&_vbo, _model);
    _program->release();
}

void BaseSceneObject::reinit() {
    _program->bind();
    _vbo.destroy();
    _vbo.create();
    _vbo.bind();
    _vbo.allocate(_model->data(), _model->sizeData() * sizeof(GLfloat));
    this->setupVertexAttribs(&_vbo, _model);
    _program->release();

}

void BaseSceneObject::setupVertexAttribs(QOpenGLBuffer *vbo, Base3DModel *model) {
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

void BaseSceneObject::render() {
    QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);
    this->bindAll();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glDrawArrays(GL_TRIANGLES, 0, _model->vertexCount());
    this->releaseAll();
}

void BaseSceneObject::bindAll() {
    _program->bind();
    this->setUniformVariables();
    this->setupVertexAttribs(&_vbo, _model);
}

void BaseSceneObject::releaseAll() {
    _program->release();
}

void BackgroundObject::updateTexture(const QImage &image) {
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

void BackgroundObject::textureBind(QOpenGLTexture *texture) {
    if (texture != nullptr) {
        texture->bind();
    }
}

void BackgroundObject::textureRelease(QOpenGLTexture *texture) {
    if (texture != nullptr) {
        texture->release();
    }
}

void BaseSceneObject::bindAttributes() {
    for (int i = 0; i < _attrName.size(); i++)
        _program->bindAttributeLocation(_attrName[i], (GLuint)i);
}

void BaseSceneObject::setUniformVariables() {

}

BackgroundObject::BackgroundObject() : BaseSceneObject::BaseSceneObject() {
    _texture = nullptr;
}

void BackgroundObject::initData() {
    _model = new Base3DModel("3D models/background.obj");
    _shaderVertPath = ":/Shaders/bg.vert";
    _shaderFragPath = ":/Shaders/bg.frag";
    _attrName << "vertex" << "v_uvs";
}

void BackgroundObject::bindAll() {
    BaseSceneObject::bindAll();
    this->textureBind(_texture);
}

void BackgroundObject::releaseAll() {
    this->textureRelease(_texture);
    BaseSceneObject::releaseAll();
}

BackgroundObject::~BackgroundObject() {
    if (_texture != nullptr) {
        _texture->destroy();
        delete _texture;
    }
}

TransformedObject::TransformedObject(TransformedObjectType type, unsigned int idx) : BackgroundObject::BackgroundObject() {
    _type = type;
    _idx = idx;
}

void TransformedObject::initData() {
    if (_type == HAIR) {
        _model = new Base3DModel(QString(HAIR_OBJ).arg(QString::number(_idx)).toStdString().c_str());
        //_imgTex = QImage(":/Hairstyles/1.tex").mirrored();
        _imgTex = QImage(QString(HAIR_TEX).arg(QString::number(_idx).toStdString().c_str())).mirrored();

        _shaderVertPath = ":/Shaders/hair.vert";
        _shaderFragPath = ":/Shaders/hair.frag";
        _attrName << "vertex" << "v_uvs";
    } else if (_type == HEAD) {
        _model = new Base3DModel("3D models/head.obj");
        _shaderVertPath = ":/Shaders/head.vert";
        _shaderFragPath = ":/Shaders/head.frag";
        _attrName << "vertex";
    }
    _posUpdated = false;
}

TransformedObject::~TransformedObject() {

}

void TransformedObject::updatePosition(cv::Mat &rmat, cv::Mat &tvec) {
    _rmat = rmat;
    _tvec = tvec;
    _posUpdated = true;
}

void TransformedObject::setUniformVariables() {
    if (_posUpdated) {
        QMatrix4x4 mvp = calcMVP();
        _program->setUniformValue("mvp", mvp);
        _posUpdated = false;
    }
}

void TransformedObject::init() {
    BaseSceneObject::init();
    if (_type == HAIR)
        this->updateTexture(_imgTex);
}

void TransformedObject::reinit() {
    this->updateHairData();
    BaseSceneObject::reinit();
}

QMatrix4x4 TransformedObject::calcMVP() {
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
    this->updateProjectionMatrix();
    QMatrix4x4 res = _projection * view * model;
    return res;
}

void TransformedObject::updateProjectionMatrix() {
    _projection.setToIdentity();
    //_projection.ortho(0.f, 640.f, 0.f, 480.f, 1.f, -1.f);
    _projection.perspective(45.f, 4.0f / 3.0f, 0.1f, 100.f);
}

void TransformedObject::incIdx() {
    _idx = MIN(_idx + 1, 13);
}

void TransformedObject::decIdx() {
    _idx = MAX(_idx - 1, 1);
}

void TransformedObject::updateHairData() {
    delete _model;
    _model = new Base3DModel(QString(HAIR_OBJ).arg(_idx).toStdString().c_str());
    _imgTex = QImage(QString(HAIR_TEX).arg(_idx).toStdString().c_str()).mirrored();
    this->updateTexture(_imgTex);
}
