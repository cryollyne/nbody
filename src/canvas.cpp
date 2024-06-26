#include <qquickwindow.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObjectFormat>
#include <cmath>
#include <random>
#include <algorithm>
#include <limits>
#include "canvas.h"
#include "backend.h"
#include "dynamic_buffer.h"
#include "simulation_data.h"

class SimRenderer : public QQuickFramebufferObject::Renderer {
public:
    SimRenderer()
        : m_renderer(nullptr)
        , m_simulator(nullptr)
    {}

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;
    void render() override;
    void synchronize(QQuickFramebufferObject *item) override;
    void synchronizeObjects(Canvas *item);

private:
    void initSimulatorObjects();

    void renderCanvas();
    void moveCamera(RenderCommand::MoveCamera cmd);
    void zoomCamera(RenderCommand::ZoomCamera cmd);
    void focusObject(RenderCommand::FocusObject cmd);

    void updateSimulator();
    void setObject(RenderCommand::SetObject obj);
    void addObject();
    void deleteObject(uint32_t index);

    Canvas *m_item;
    QQueue<RenderCommand::RenderCommand> *m_commandQueue = new QQueue<RenderCommand::RenderCommand>;

    QOpenGLShaderProgram *m_renderer;
    QOpenGLShaderProgram *m_simulator;
    DynamicBufferArray m_simulatorBuffer;

    QMatrix4x4 m_cameraModel = QMatrix4x4();
    int m_focusIndex = -1;
    float m_dt = 60*24*5;
    float m_aspectRatio;
    float m_zoom = 5.0e11f;
    float m_fov = M_PI / 2;

    bool m_orthographic = false;
};

QOpenGLFramebufferObject *SimRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

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

        initSimulatorObjects();
    }

    m_aspectRatio = (float)size.height()/(float)size.width();

    return new QOpenGLFramebufferObject(size, format);
}

void SimRenderer::initSimulatorObjects() {
    auto makeObject = [this](float distance, float velocity, float angle, float mass, float y=0.0) {
        m_simulatorBuffer.addObject({
            vec3{(float)cos(angle)*distance, (float)sin(angle)*distance, y},
            vec3{(float)-sin(angle)*velocity, (float)cos(angle)*velocity, 0},
            mass
        });
    };
    srand(0xbeef); // use same seed for consistant random values
    auto randAng = []() {return (rand()%(6283))/1000.0;};

    m_simulatorBuffer.addObject({vec3 {0, 0, 0}, vec3 {0, 0, 0}, 1.989e30}); // sun

    // earth
    float earthAng = randAng();
    vec3 earthPos = vec3{(float)cos(earthAng)*1.496e11f, (float)sin(earthAng)*1.496e11f, 0};
    vec3 earthVel = vec3{(float)-sin(earthAng)*2.98e4f, (float)cos(earthAng)*2.98e4f, 0};
    m_simulatorBuffer.addObject({ earthPos, earthVel, 5.972e24 });

    // moon
    float moonAng = randAng();
    vec3 moonPos = vec3{(float)cos(moonAng)*3.84e8f, (float)sin(moonAng)*3.84e8f, 0};
    vec3 moonVel = vec3{(float)-sin(moonAng)*1023.0f, (float)cos(moonAng)*1023.0f, 0};
    m_simulatorBuffer.addObject({
        vec3{
            earthPos.x + moonPos.x,
            earthPos.y + moonPos.y,
            earthPos.z + moonPos.z,
        },
        vec3 {
            earthVel.x + moonVel.x,
            earthVel.y + moonVel.y,
            earthVel.z + moonVel.z,
        },
        7.348e22
    });


    makeObject(5.79e10, 4.74e4, randAng(), 3.30e23);    // Mercury
    makeObject(1.082e11, 3.5e4, randAng(), 4.870e24);   // Venus
    makeObject(2.280e11, 2.41e4, randAng(), 6.42e23);   // Mars

    makeObject(7.775e11, 1.31e4, randAng(), 1.898e27);  // Jupiter
    makeObject(1.432e12, 9.7e3, randAng(), 5.68e26);    // Saturn
    makeObject(2.867e12, 6.8e3, randAng(), 8.68e25);    // Uranus
    makeObject(4.515e12, 5.4e3, randAng(), 1.02e26);    // Neptune

    // astroid belt
    for (int i = 0; i < 100; i++) {
        float dist = ((rand() % 1000)/1000.0) * 149000000000 + 329000000000;
        float vel = ((rand() % 1000)/1000.0) * 8000 + 19000;
        float y = ((rand() % 1000)/1000.0) * 40000000000 - 20000000000;
        makeObject(dist, vel, randAng(), 100, y);
    }
}

void SimRenderer::render() {
    if (m_commandQueue->isEmpty())
        renderCanvas();

    while (!m_commandQueue->isEmpty()) {
        RenderCommand::RenderCommand c = m_commandQueue->dequeue();
        switch (c.index()) {
            case 0: renderCanvas(); break;
            case 1: moveCamera(std::get<RenderCommand::MoveCamera>(c)); break;
            case 2: zoomCamera(std::get<RenderCommand::ZoomCamera>(c)); break;
            case 3: focusObject(std::get<RenderCommand::FocusObject>(c)); break;
            case 4: m_orthographic = std::get<RenderCommand::SetProjection>(c).orthographic; break;
            case 5: m_fov = std::get<RenderCommand::SetFov>(c).fov; break;
        }
    }
}

void SimRenderer::synchronize(QQuickFramebufferObject *item) {
    Canvas *canvas = static_cast<Canvas*>(item);
    m_item = canvas;
    while (!canvas->m_commandQueue->isEmpty()) {
        RenderCommand::Command cmd = canvas->m_commandQueue->dequeue();
        if (cmd.index() == 0)
            m_commandQueue->enqueue(std::get<RenderCommand::RenderCommand>(cmd));
        else {
            RenderCommand::SimulatorCommand c = std::get<RenderCommand::SimulatorCommand>(cmd);
            switch (c.index()) {
                case 0: updateSimulator(); break;
                case 1: synchronizeObjects(canvas); break;
                case 2: setObject(std::get<RenderCommand::SetObject>(c)); break;
                case 3: addObject(); break;
                case 4: deleteObject(std::get<RenderCommand::DeleteObject>(c).index); break;
                case 5: m_dt = std::get<RenderCommand::SetTimeStep>(c).dt; break;
            }
        }
    }
}

void SimRenderer::synchronizeObjects(Canvas *canvas) {
    QOpenGLExtraFunctions *gl = QOpenGLContext::currentContext()->extraFunctions();

    uint32_t length = m_simulatorBuffer.size();

    gl->glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_simulatorBuffer.buffObject());
    SimulatorData *data = (SimulatorData*)gl->glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, length*sizeof(SimulatorData), GL_MAP_READ_BIT);
    canvas->m_objects.clear();
    for (uint32_t i = 0; i < length; i++) {
        SimulatorData &current = data[i];
        QVariant obj;
        obj.setValue(SimulatorObject {
            current.position,
            current.velocity,
            current.mass
        });
        canvas->m_objects.append(obj);
    }
    gl->glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    emit canvas->objectsChanged();
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

    if (m_orthographic) {
        QMatrix4x4 proj = QMatrix4x4(
            1/m_zoom * m_aspectRatio, 0, 0, 0,
            0, 1/m_zoom, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 1
        );
        m_renderer->setUniformValue("orthographic", true);
        m_renderer->setUniformValue("projection", proj);
        m_renderer->setUniformValue("zoom", m_zoom);
    } else {
        float n = m_zoom/16;    // near clip plane
        float f = 1024*m_zoom;  // far clip plane
        float s = 1/tan(m_fov/2);
        QMatrix4x4 proj = QMatrix4x4(
            s*m_aspectRatio,    0,      0,      0,
            0,                  s,      0,      0,
            0,                  0,      1/f,    (m_zoom - n)/f - 1,
            0,                  0,      1,      m_zoom
        );
        m_renderer->setUniformValue("orthographic", false);
        m_renderer->setUniformValue("projection", proj);
    }

    m_renderer->setUniformValue("focus", m_focusIndex);
    m_renderer->setUniformValue("view", m_cameraModel.inverted());

    gl->glDrawArraysInstanced(GL_POINTS, 0, m_simulatorBuffer.size(), m_simulatorBuffer.size());

    m_renderer->release();
    m_item->window()->endExternalCommands();
}

void SimRenderer::moveCamera(RenderCommand::MoveCamera cmd) {
    float x = cmd.x;
    float y = cmd.y;

    float theta = sqrt(x*x + y*y);
    QVector3D rotAxis = (m_cameraModel * QVector3D{-y, x, 0}).normalized();

    QMatrix3x3 I = QMatrix3x3();
    float wData[] = {
        0,              -rotAxis.z(),   rotAxis.y(),
        rotAxis.z(),    0,              -rotAxis.x(),
        -rotAxis.y(),   rotAxis.x(),    0
    };
    QMatrix3x3 W = QMatrix3x3(wData);

    // Rodrigues rotation formula
    QMatrix3x3 rot = I + (float)sin(theta)*W + (float)(1-cos(theta))*W*W;

    QMatrix4x4 rot4 = QMatrix4x4(
        rot(0, 0), rot(0, 1), rot(0, 2), 0,
        rot(1, 0), rot(1, 1), rot(1, 2), 0,
        rot(2, 0), rot(2, 1), rot(2, 2), 0,
        0,          0,          0,          1
    );

    m_cameraModel = rot4 * m_cameraModel;
}

void SimRenderer::zoomCamera(RenderCommand::ZoomCamera cmd) {
    m_zoom = std::clamp(
        m_zoom * cmd.amount,
        std::numeric_limits<float>::min(),
        std::numeric_limits<float>::max()
    );
}

void SimRenderer::focusObject(RenderCommand::FocusObject cmd) {
    m_focusIndex = cmd.index;
}

void SimRenderer::updateSimulator() {
    QOpenGLExtraFunctions *gl = QOpenGLContext::currentContext()->extraFunctions();

    m_simulator->bind();

    m_simulator->setUniformValue("dt", m_dt);

    gl->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_simulatorBuffer.buffObject());
    gl->glDispatchCompute(m_simulatorBuffer.size(), 1, 1);

    m_simulator->release();
}

void SimRenderer::setObject(RenderCommand::SetObject obj) {
    m_simulatorBuffer.editObject(obj.index, &obj.data);
}
void SimRenderer::addObject() {
    m_simulatorBuffer.addObject(SimulatorData{});
}
void SimRenderer::deleteObject(uint32_t index) {
    m_simulatorBuffer.removeObject(index);
}

void Canvas::updateRenderer() {
    m_commandQueue->enqueue(RenderCommand::Render{});
    update();
}
void Canvas::tickSimulator() {
    m_commandQueue->enqueue(RenderCommand::Simulator{});
    update();
}
void Canvas::synchronizeObjects() {
    m_commandQueue->enqueue(RenderCommand::SynchronizeObjects{});
    update();
}
void Canvas::setObject(int index, QVector3D position, QVector3D velocity, float mass) {
    SimulatorData data {
        vec3{position.x(), position.y(), position.z()},
        vec3{velocity.x(), velocity.y(), velocity.z()},
        mass
    };
    m_commandQueue->enqueue(RenderCommand::SetObject{static_cast<uint32_t>(index), data});
    if (!m_isSimulationRunning) {
        updateRenderer();
    }
}
void Canvas::addObject() {
    m_commandQueue->enqueue(RenderCommand::AddObject{});
    updateRenderer();
    synchronizeObjects();
}
void Canvas::deleteObject(int index) {
    m_commandQueue->enqueue(RenderCommand::DeleteObject{static_cast<uint32_t>(index)});
    updateRenderer();
    synchronizeObjects();
}
void Canvas::moveCamera(float x, float y) {
    m_commandQueue->enqueue(RenderCommand::MoveCamera{
        (m_cameraInvert?1:-1)*m_cameraSensitivity*x,
        (m_cameraInvert?1:-1)*m_cameraSensitivity*y
    });
    if (!m_isSimulationRunning)
        updateRenderer();
}
void Canvas::zoomCamera(float amount) {
    m_commandQueue->enqueue(RenderCommand::ZoomCamera{
        (float)exp( (m_zoomInvert?1:-1) * amount * m_zoomSensitivity )
    });
    if (!m_isSimulationRunning)
        updateRenderer();
}

QQuickFramebufferObject::Renderer *Canvas::createRenderer() const {
    connect(m_simulatorTimer, &QTimer::timeout, this, &Canvas::tickSimulator, Qt::DirectConnection);
    connect(m_frameTimer, &QTimer::timeout, this, &Canvas::updateRenderer, Qt::DirectConnection);
    connect(m_objectUpdateTimer, &QTimer::timeout, this, &Canvas::synchronizeObjects, Qt::DirectConnection);
    QMetaObject::invokeMethod(m_simulatorTimer, [this](){
        this->m_simulatorTimer->start(1000.0/m_simulatorTickRate);
        this->m_frameTimer->start(1000.0/m_frameUpdateRate);
        this->m_objectUpdateTimer->start(1000.0/m_objectUpdateRate);
    });
    return new SimRenderer();
}



QVariantList Canvas::getObjects() const { return m_objects; }

float Canvas::getTickRate() const { return m_simulatorTickRate; }
void Canvas::setTickRate(float rate) {
    if (rate == m_simulatorTickRate)
        return;
    m_simulatorTickRate = rate;
    m_commandQueue->enqueue(RenderCommand::SetTimeStep{m_timeRatio / m_simulatorTickRate});
    emit tickRateChanged();

    if (m_simulatorTimer->isActive()) {
        QMetaObject::invokeMethod(m_simulatorTimer, [&,this](){
            this->m_simulatorTimer->stop();
            this->m_simulatorTimer->start(1000.0/rate);
        });
    }
}

float Canvas::getFrameUpdateRate() const { return m_frameUpdateRate; }
void Canvas::setFrameUpdateRate(float rate) {
    if (rate == m_frameUpdateRate)
        return;
    m_frameUpdateRate = rate;
    emit tickRateChanged();

    if (m_frameTimer->isActive()) {
        QMetaObject::invokeMethod(m_frameTimer, [&,this](){
            this->m_frameTimer->stop();
            this->m_frameTimer->start(1000.0/rate);
        });
    }
}

float Canvas::getObjectUpdateRate() const { return m_objectUpdateRate; }
void Canvas::setObjectUpdateRate(float rate) {
    if (rate == m_objectUpdateRate)
        return;

    m_objectUpdateRate = rate;
    emit objectUpdateRateChanged();

    if (m_objectUpdateTimer->isActive()) {
        QMetaObject::invokeMethod(m_objectUpdateTimer, [&,this](){
            this->m_objectUpdateTimer->stop();
            this->m_objectUpdateTimer->start(1000.0/rate);
        });
    }
}

float Canvas::getSensitivity() const { return m_cameraSensitivity; }
void Canvas::setSensitivity(float sensitivity) {
    if (sensitivity == m_cameraSensitivity)
        return;
    m_cameraSensitivity = sensitivity;
    emit sensitivityChanged();
}

float Canvas::getZoomSensitivity() const { return m_zoomSensitivity; }
void Canvas::setZoomSensitivity(float sensitivity) {
    if (sensitivity == m_zoomSensitivity)
        return;
    m_zoomSensitivity = sensitivity;
    emit zoomSensitivityChanged();
}

float Canvas::getFov() const { return m_fov; }
void Canvas::setFov(float fov) {
    if (m_fov == fov)
        return;
    m_fov = fov;
    emit fovChanged();
    m_commandQueue->enqueue(RenderCommand::SetFov{(float)M_PI/180 * fov});
    updateRenderer();
}

float Canvas::getTimeRatio() const { return m_timeRatio; }
void Canvas::setTimeRatio(float ratio) {
    if (m_timeRatio == ratio)
        return;
    m_timeRatio = ratio;
    m_commandQueue->enqueue(RenderCommand::SetTimeStep{m_timeRatio / m_simulatorTickRate});
    emit timeRatioChanged();
}

int Canvas::getFocusIndex() const { return m_focusIndex; }
void Canvas::setFocusIndex(int index) {
    if (m_focusIndex == index)
        return;
    m_focusIndex = index;
    m_commandQueue->enqueue(RenderCommand::FocusObject{index});
    if (!m_isSimulationRunning)
        updateRenderer();
    emit focusIndexChanged();
}

bool Canvas::getCameraInvert() const { return m_cameraInvert; }
void Canvas::setCameraInvert(bool invert) {
    if (invert == m_cameraInvert)
        return;
    m_cameraInvert = invert;
    emit cameraInvertChanged();
}

bool Canvas::getZoomInvert() const { return m_zoomInvert; }
void Canvas::setZoomInvert(bool invert) {
    if (invert == m_zoomInvert)
        return;
    m_zoomInvert = invert;
    emit zoomInvertChanged();
}

bool Canvas::isOrthographic() const { return m_orthographic; }
void Canvas::setOrthographic(bool ortho) {
    if (m_orthographic == ortho)
        return;
    m_orthographic = ortho;
    emit orthographicChanged();
    m_commandQueue->enqueue(RenderCommand::SetProjection{ortho});
    updateRenderer();
}

bool Canvas::isSimulationRunning() const { return m_isSimulationRunning; }
void Canvas::setIsSimulationRunning(bool r) {
    if (r != m_isSimulationRunning) {
        m_isSimulationRunning = r;
        emit isSimulationRunningChanged();
    }

    if (r) {
        QMetaObject::invokeMethod(m_simulatorTimer, [this](){
            this->m_simulatorTimer->start(1000.0/m_simulatorTickRate);
            this->m_frameTimer->start(1000.0/m_frameUpdateRate);
            this->m_objectUpdateTimer->start(1000.0/m_objectUpdateRate);
        });
    } else {
        QMetaObject::invokeMethod(m_simulatorTimer, [this](){
            this->m_simulatorTimer->stop();
            this->m_frameTimer->stop();
            this->m_objectUpdateTimer->stop();
        });
    }
}
