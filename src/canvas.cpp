#include "canvas.h"
#include <qt/QtQuick/qquickwindow.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLContext>
#include <qt/QtCore/QRunnable>

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
        m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex,
                                                    "#version 330 core\n"
                                                    "attribute highp vec4 vertices;"
                                                    "out vec2 coords;"
                                                    "void main() {"
                                                    "    gl_Position = vertices;"
                                                    "    coords = vertices.xy;"
                                                    "}");
        m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment,
                                                    "#version 330 core\n"
                                                    "uniform lowp float t;"
                                                    "in vec2 coords;"
                                                    "void main() {"
                                                    "    gl_FragColor = vec4(coords.xy, 0, 1);"
                                                    "}");
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
    m_program->enableAttributeArray(0);

    float val[] = {
        -1, -1,
        1, -1,
        -1, 1,
        1, 1
    };

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    m_program->setAttributeArray(0, GL_FLOAT, val, 2);
    m_program->setUniformValue("t", (float)m_t);
    
    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_program->disableAttributeArray(0);
    m_program->release();
    m_window->endExternalCommands();
}

