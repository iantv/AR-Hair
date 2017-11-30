#include "GLWidget.h"
#include <QPainter>
#include <QResizeEvent>

static const char *vertexShaderSource =
    "attribute vec4 vertex;\n"
    "attribute vec2 v_uvs;\n"
    "varying vec2 f_uvs;\n"
    "void main() {\n"
    "   f_uvs = v_uvs;\n"
    "   gl_Position = vertex;\n"
    "}\n";

static const char *fragmentShaderSource =
    "uniform sampler2D texture;"
    "varying highp vec2 f_uvs;"
    "void main() {\n"
    "   gl_FragColor = texture2D(texture, f_uvs);\n"
    "}\n";

bool GLWidget::_transparent = false;

GLWidget::GLWidget(QWidget *parent) :
    QOpenGLWidget(parent) {
    _bgTex = nullptr;
    _bgImageModel = Base3DModel("3D models/background.obj");
    _core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
    if (_transparent) {
        QSurfaceFormat fmt = format();
        fmt.setAlphaBufferSize(8);
        setFormat(fmt);
    }
}

GLWidget::~GLWidget() {
    delete _bgTex;
}

void GLWidget::initializeGL() {
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, int(!_transparent));

    _bgProgram = new QOpenGLShaderProgram;
    _bgProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    _bgProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    _bgProgram->bindAttributeLocation("vertex", (GLuint)VBO_VERTICES);
    _bgProgram->bindAttributeLocation("v_uvs", (GLuint)VBO_TEXCOORS);
    _bgProgram->link();

    _bgProgram->bind();
    _bgProgram->setUniformValue("texture", 0);

    _bgVAO.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&_bgVAO);
    _bgVBO.create();
    _bgVBO.bind();
    _bgVBO.allocate(_bgImageModel.data(), _bgImageModel.sizeData() * sizeof(GLfloat));
    this->setupVertexAttribs();
    _camera.setToIdentity();
    _camera.translate(0, 0, -1);
    _bgProgram->release();
}

void GLWidget::setupVertexAttribs()
{
    _bgVBO.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray((GLuint)VBO_VERTICES);
    f->glEnableVertexAttribArray((GLuint)VBO_TEXCOORS);
    f->glVertexAttribPointer((GLuint)VBO_VERTICES, _bgImageModel._size[VBO_VERTICES],
                             GL_FLOAT, GL_FALSE, _bgImageModel._count * sizeof(GLfloat),
                             reinterpret_cast<void *>(_bgImageModel._indx[VBO_VERTICES] * sizeof(GLfloat)));
    f->glVertexAttribPointer((GLuint)VBO_TEXCOORS, _bgImageModel._size[VBO_TEXCOORS],
                             GL_FLOAT, GL_FALSE, _bgImageModel._count * sizeof(GLfloat),
                             reinterpret_cast<void *>(_bgImageModel._indx[VBO_TEXCOORS] * sizeof(GLfloat)));
    _bgVBO.release();
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, int(!_transparent));
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    QOpenGLVertexArrayObject::Binder vaoBinder(&_bgVAO);
    _bgProgram->bind();
    if (_bgTex != nullptr) {
        _bgTex->bind();
    }

    this->setupVertexAttribs();

    glDrawArrays(GL_TRIANGLES, 0, _bgImageModel.vertexCount());

    if (_bgTex != nullptr) {
        _bgTex->release();
    }
    _bgProgram->release();
}

void GLWidget::resizeGL(int width, int height) {
    _proj.setToIdentity();
    _proj.perspective(45.0f, GLfloat(width) / height, 0.01f, 100.0f);
}

void GLWidget::updateBackgroundImage(const QImage &image) {
    if (_bgVAO.isCreated()) {
        _bgVAO.bind();
        if (_bgTex != nullptr) {
            delete _bgTex;
        }
        _bgTex = new QOpenGLTexture(image);
    }
    this->update();
}

void GLWidget::cleanup()
{
    if (_bgProgram == nullptr)
        return;
    makeCurrent();
    _bgVBO.destroy();
    delete _bgProgram;
    _bgProgram = 0;
    doneCurrent();
}
