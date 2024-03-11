#include "canvas.h"
#include <qt/QtQuick/qquickwindow.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLContext>
#include <qt/QtCore/QRunnable>

#include "backend.h"

class CleanupJob : public QRunnable {
public:
    CleanupJob(Renderer *renderer)
        : m_renderer(renderer) {
    }

    void run() override {delete m_renderer;}

    Renderer *m_renderer;
};

void Canvas::handleWindowChanged(QQuickWindow *win) {
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &Canvas::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &Canvas::cleanup, Qt::DirectConnection);

        win->setColor(Qt::black);
    }
}

void Canvas::sync() {
    if (!m_renderer) {
        m_renderer = Renderer::getRenderer();
        connect(window(), &QQuickWindow::beforeRendering, m_renderer, &Renderer::init, Qt::DirectConnection);
        connect(window(), &QQuickWindow::afterRenderPassRecording, m_renderer, &Renderer::paint, Qt::DirectConnection);
    }

    m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    m_renderer->setT(m_t);
    m_renderer->setWindow(window());
}

void Canvas::cleanup() {
    delete m_renderer;
    m_renderer = nullptr;
}

void Canvas::setT(qreal t) {
    if (t == m_t)
        return;
    m_t = t;
    emit tChanged();
    if (window())
        window()->update();
}

void Canvas::releaseResources() {
    window()->scheduleRenderJob(new CleanupJob(m_renderer), QQuickWindow::BeforeSynchronizingStage);
    m_renderer = nullptr;
}

Renderer *Renderer::singleton = nullptr;
Renderer *Renderer::getRenderer() {
    if (Renderer::singleton)
        return Renderer::singleton;

    return Renderer::singleton = new Renderer();
}

Renderer::~Renderer() {
    delete m_program;
}

void Renderer::init() {
    if (!m_program) {
        QSGRendererInterface *rif = m_window->rendererInterface();
        Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL);

        initializeOpenGLFunctions();
        m_program = new QOpenGLShaderProgram();
        m_program->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, Backend::DATADIR.absolutePath().append("/renderer/vertex.glsl"));
        m_program->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, Backend::DATADIR.absolutePath().append("/renderer/fragment.glsl"));
        m_program->bindAttributeLocation("vertices", 0);
        m_program->link();
    }

    if (!m_simulator) {
        initSimulator();
    }
}

void Renderer::paint() {
    m_window->beginExternalCommands();
    m_program->bind();

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_simulatorBuffObj);
    
    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glDrawArraysInstanced(GL_POINTS, 0, 2, 2);

    m_program->release();
    m_window->endExternalCommands();
}

