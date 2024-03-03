#include "canvas.h"

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
        shader->compileSourceCode(
                                                   "#version 430 core\n"
                                                   "layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
                                                   "struct Object {\n"
                                                   "    vec3 position;\n"
                                                   "    vec3 velocity;\n"
                                                   "    float mass;\n"
                                                   "};\n"
                                                   "layout(binding = 0, std430) buffer SSBO {\n"
                                                   "    Object obj[];\n"
                                                   "} val;\n"
                                                   "void main() {\n"
                                                   "}\n"
                                                   );
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
    }
}

void Renderer::simulatorTick() {
    m_simulator->bind();

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_simulatorBuffObj);
    glDispatchCompute(LEN, 1, 1);
    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

    m_simulator->release();
}
