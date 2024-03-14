#include "dynamic_buffer.h"

#include <QOpenGLExtraFunctions>
#include <QOpenGLContext>

#include "canvas.h"
DynamicBufferArray::DynamicBufferArray()
    : m_length(0)
    , m_capacity(32)
{
    QOpenGLExtraFunctions *gl = QOpenGLContext::currentContext()->extraFunctions();
    
    gl->glGenBuffers(1, &m_buffObject);
    gl->glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffObject);
    gl->glBufferData(GL_SHADER_STORAGE_BUFFER, m_capacity*sizeof(SimulatorData), nullptr, GL_DYNAMIC_READ);
}

void DynamicBufferArray::addObject(const SimulatorData *obj) {
    QOpenGLExtraFunctions *gl = QOpenGLContext::currentContext()->extraFunctions();
    if (m_length >= m_capacity)
        realloc(m_capacity * 2);

    gl->glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffObject);
    gl->glBufferSubData(GL_SHADER_STORAGE_BUFFER, m_length*sizeof(SimulatorData), sizeof(SimulatorData), obj);
    m_length++;
}

void DynamicBufferArray::removeObject(uint32_t index) {
    if (index + 1 == m_length) {
        m_length--;
        return;
    }

    QOpenGLExtraFunctions *gl = QOpenGLContext::currentContext()->extraFunctions();
    
    uint32_t afterBuff; // data after the removed object
    uint32_t sizeAfter = (m_length - index - 1)*sizeof(SimulatorData); // size of data after removed object in bytes
    gl->glGenBuffers(1, &afterBuff);
    gl->glBindBuffer(GL_COPY_READ_BUFFER, afterBuff);
    gl->glBufferData(GL_COPY_READ_BUFFER, sizeAfter, nullptr, GL_DYNAMIC_READ);
    gl->glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffObject);

    gl->glCopyBufferSubData(GL_SHADER_STORAGE_BUFFER, GL_COPY_READ_BUFFER, (index + 1)*sizeof(SimulatorData), 0, sizeAfter);
    gl->glCopyBufferSubData(GL_COPY_READ_BUFFER ,GL_SHADER_STORAGE_BUFFER, 0, index*sizeof(SimulatorData), sizeAfter);

    gl->glDeleteBuffers(1, &afterBuff);
    gl->glBindBuffer(GL_COPY_READ_BUFFER, 0);

    m_length--;
}

void DynamicBufferArray::editObject(uint32_t index, const SimulatorData *obj) {
    QOpenGLExtraFunctions *gl = QOpenGLContext::currentContext()->extraFunctions();

    gl->glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffObject);
    gl->glBufferSubData(GL_SHADER_STORAGE_BUFFER, index*sizeof(SimulatorData), sizeof(SimulatorData), obj);
}


uint32_t DynamicBufferArray::buffObject() {
    return m_buffObject;
}

uint32_t DynamicBufferArray::size() {
    return m_length;
}

void DynamicBufferArray::realloc(uint32_t capacity) {
    QOpenGLExtraFunctions *gl = QOpenGLContext::currentContext()->extraFunctions();
    uint32_t oldBuffObject = m_buffObject;
    m_capacity = capacity;

    gl->glGenBuffers(1, &m_buffObject);
    gl->glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffObject);
    gl->glBufferData(GL_SHADER_STORAGE_BUFFER, capacity*sizeof(SimulatorData), nullptr, GL_DYNAMIC_READ);

    gl->glBindBuffer(GL_COPY_READ_BUFFER, oldBuffObject);
    gl->glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_SHADER_STORAGE_BUFFER, 0, 0, m_length*sizeof(SimulatorData));

    gl->glDeleteBuffers(1, &oldBuffObject);
}
