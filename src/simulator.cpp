#include "canvas.h"
#include "backend.h"

#include <iostream>
std::ostream &operator<<(std::ostream &o, SimulatorData &sim) {
    return o << "pos: { " << sim.position.x << ", " << sim.position.y << ", " << sim.position.z << " }, "
             << "vel: { " << sim.velocity.x << ", " << sim.velocity.y << ", " << sim.velocity.z << " }, "
             << "mass: " << sim.mass;
}

#define LEN 2

void Renderer::initSimulator() {
    if (!m_simulator) {
        std::cout << "init compute shaders\n";
        m_simulator = new QOpenGLShaderProgram();
        QOpenGLShader *shader = new QOpenGLShader(QOpenGLShader::Compute);
        shader->compileSourceFile(Backend::DATADIR.absolutePath().append("/simulator/simulator.glsl"));
        m_simulator->addShader(shader);
        m_simulator->link();

        delete shader;

        // TODO: allow dynamic alteration of data
        SimulatorData *buffData = new SimulatorData[LEN] {
            {glm::vec3 {-0.5f, 0, 0}, glm::vec3{0, 0.1f, 0}, 1e10f},
            {glm::vec3 {0.5f, 0, 0}, glm::vec3{0, -0.1f, 0}, 1e10f},
        };
        uint buffObj;
        glGenBuffers(1, &buffObj);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffObj);
        glBufferData(GL_SHADER_STORAGE_BUFFER, LEN*sizeof(SimulatorData), buffData, GL_DYNAMIC_READ);
        m_simulatorBuffObj = buffObj;
        delete[] buffData;

        connect(&m_simulatorRunner, &QTimer::timeout, this, &Renderer::tickSimulator, Qt::DirectConnection);
    }
}

void Renderer::tickSimulator() {
    m_simulator->bind();

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_simulatorBuffObj);
    glDispatchCompute(LEN, 1, 1);
    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

    m_simulator->release();
}

void Renderer::runSimulator() {
    QMetaObject::invokeMethod(this, [this]{
        if (!m_simulatorRunner.isActive()) {
            m_simulatorRunner.start(100);
        }
    }, Qt::QueuedConnection);
}
void Renderer::stopSimulator() {
    QMetaObject::invokeMethod(this, [this]{
        if (m_simulatorRunner.isActive()) {
            m_simulatorRunner.stop();
        }
    }, Qt::QueuedConnection);
}
