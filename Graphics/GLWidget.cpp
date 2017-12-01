#include "GLWidget.h"
#include <QPainter>
#include <QResizeEvent>

bool GLWidget::_transparent = false;

GLWidget::GLWidget(QWidget *parent) :
    QOpenGLWidget(parent) {
    _background = new ModelRendering();
    _core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
    if (_transparent) {
        QSurfaceFormat fmt = format();
        fmt.setAlphaBufferSize(8);
        setFormat(fmt);
    }
}

GLWidget::~GLWidget() {
    delete _background;
}

void GLWidget::initializeGL() {
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, int(!_transparent));

    _background->init();

    _camera.setToIdentity();
    _camera.translate(0, 0, -1);
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, int(!_transparent));
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    _background->render();
}

void GLWidget::resizeGL(int width, int height) {
    _proj.setToIdentity();
    _proj.perspective(45.0f, GLfloat(width) / height, 0.01f, 100.0f);
}

void GLWidget::updateBackgroundImage(const QImage &image) {
    _background->updateTexture(image);
    this->update();
}

void GLWidget::cleanup()
{
    GLWidget::~GLWidget();
}
