#include <qt/QtQuick/qquickwindow.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObjectFormat>
#include "canvas.h"
#include "backend.h"
#include "dynamic_buffer.h"

#define LEN 2

std::ostream &operator<<(std::ostream &o, SimulatorData &sim) {
    return o << "pos: { " << sim.position.x << ", " << sim.position.y << ", " << sim.position.z << " }, "
             << "vel: { " << sim.velocity.x << ", " << sim.velocity.y << ", " << sim.velocity.z << " }, "
             << "mass: " << sim.mass;
}

class SimRenderer : public QQuickFramebufferObject::Renderer {
public:
    SimRenderer(const Canvas *fbo)
        : m_item(fbo)
        , m_renderer(nullptr)
        , m_simulator(nullptr)
    {}

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;
    void render() override;
    void synchronize(QQuickFramebufferObject *item) override;

private:

    void renderCanvas();
    void updateSimulator();

    const Canvas *m_item;
    QQueue<RenderCommand::Command> *m_commandQueue = new QQueue<RenderCommand::Command>;

    QOpenGLShaderProgram *m_renderer;
    QOpenGLShaderProgram *m_simulator;
    DynamicBufferArray m_simulatorBuffer;
};

QOpenGLFramebufferObject *SimRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    QOpenGLExtraFunctions *gl = QOpenGLContext::currentContext()->extraFunctions();

    if (!m_renderer) {
        m_renderer = new QOpenGLShaderProgram();
        m_renderer->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, Backend::DATADIR.absolutePath().append("/renderer/vertex.glsl"));
        m_renderer->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, Backend::DATADIR.absolutePath().append("/renderer/fragment.glsl"));
        m_renderer->bindAttributeLocation("vertices", 0);
        m_renderer->link();
    }

    if (!m_simulator) {
        m_simulator = new QOpenGLShaderProgram();
        QOpenGLShader *shader = new QOpenGLShader(QOpenGLShader::Compute);
        shader->compileSourceFile(Backend::DATADIR.absolutePath().append("/simulator/simulator.glsl"));
        m_simulator->addShader(shader);
        m_simulator->link();
        delete shader;

        SimulatorData *buffData = new SimulatorData[LEN] {
            {glm::vec3 {-1, 0, 0}, glm::vec3 {0, -0.3, 0}, 1e10},
            {glm::vec3 {+1, 0, 0}, glm::vec3 {0, +0.3, 0}, 1e10},
        };

        m_simulatorBuffer.addObject(buffData);
        m_simulatorBuffer.addObject(buffData + 1);

        delete[] buffData;
    }

    return new QOpenGLFramebufferObject(size, format);
}

void SimRenderer::render() {
    while (!m_commandQueue->isEmpty()) {
        RenderCommand::Command c = m_commandQueue->dequeue();
        switch (c.index()) {
            case 0: renderCanvas(); break;
            case 1: updateSimulator(); break;
            default:
                break;
        }
    }
}

void SimRenderer::synchronize(QQuickFramebufferObject *item) {
    Canvas *canvas = static_cast<Canvas*>(item);
    while (!canvas->m_commandQueue->isEmpty())
        m_commandQueue->enqueue(canvas->m_commandQueue->dequeue());
}

void SimRenderer::renderCanvas() {
    QOpenGLExtraFunctions *gl = QOpenGLContext::currentContext()->extraFunctions();

    m_item->window()->beginExternalCommands();
    m_renderer->bind();
    gl->glClearColor(0, 0, 0, 1);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_simulatorBuffer.buffObject());

    gl->glDisable(GL_DEPTH_TEST);
    gl->glEnable(GL_BLEND);
    gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    gl->glEnable(GL_PROGRAM_POINT_SIZE);

    gl->glDrawArraysInstanced(GL_POINTS, 0, m_simulatorBuffer.size(), m_simulatorBuffer.size());

    m_renderer->release();
    m_item->window()->endExternalCommands();
}

void SimRenderer::updateSimulator() {
    QOpenGLExtraFunctions *gl = QOpenGLContext::currentContext()->extraFunctions();

    m_simulator->bind();

    gl->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_simulatorBuffer.buffObject());
    gl->glDispatchCompute(m_simulatorBuffer.size(), 1, 1);

    m_simulator->release();
}

void Canvas::updateRenderer() {
    m_commandQueue->enqueue(RenderCommand::Render{});
    update();
}
void Canvas::tickSimulator() {
    m_commandQueue->enqueue(RenderCommand::Simulator{});
    update();
}

QQuickFramebufferObject::Renderer *Canvas::createRenderer() const {
    connect(m_simulatorTimer, &QTimer::timeout, this, &Canvas::tickSimulator, Qt::DirectConnection);
    connect(m_frameTimer, &QTimer::timeout, this, &Canvas::updateRenderer, Qt::DirectConnection);
    QMetaObject::invokeMethod(m_simulatorTimer, [this](){
        this->m_simulatorTimer->start(1000/60);
        this->m_frameTimer->start(1000/30);
    });
    return new SimRenderer(this);
}

