#include "GLWidget.h"
#include <QPainter>
#include <QResizeEvent>

bool GLWidget::_transparent = false;

GLWidget::GLWidget(QWidget *parent) :
    QOpenGLWidget(parent) {
    _background = new BackgroundRendering();
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

    _background->init();

}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, int(!_transparent));

    _background->render();
}

void GLWidget::resizeGL(int width, int height) {
}

void GLWidget::updateBackgroundImage(const QImage &image) {
    _background->updateTexture(image);
    this->update();
}

void GLWidget::cleanup()
{
    GLWidget::~GLWidget();
}
