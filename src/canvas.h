#ifndef CANVAS_H
#define CANVAS_H

#include <qt/QtQuick/QQuickItem>
#include <qt/QtQuick/qquickwindow.h>
#include <QOpenGLShaderProgram>
#include <QQuickFramebufferObject>
#include <QTimer>

#include <glm/vec3.hpp>
#include <iostream>

#pragma pack()
struct SimulatorData {
    glm::vec3 position;
    char _pad[4];
    glm::vec3 velocity;
    float mass;

    SimulatorData()
        : position(glm::vec3 {0, 0, 0})
        , velocity(glm::vec3 {0, 0, 0})
        , mass(1)
    {}
    SimulatorData(glm::vec3 pos, glm::vec3 vel, float _mass)
        : position(pos)
        , velocity(vel)
        , mass(_mass)
    {}
};

std::ostream &operator<<(std::ostream &o, SimulatorData &sim);


class SimRenderer : public QQuickFramebufferObject::Renderer {
public: 
    SimRenderer(const QQuickFramebufferObject *fbo)
        : m_item(fbo)
        , m_renderer(nullptr)
        , m_simulator(nullptr)
    {}

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;
    void render() override;

private:

    const QQuickFramebufferObject *m_item;

    QOpenGLShaderProgram *m_renderer;
    QOpenGLShaderProgram *m_simulator;
    uint32_t m_simulatorBuffObj;
};

class Canvas : public QQuickFramebufferObject {
    QQuickFramebufferObject::Renderer *createRenderer() const override;

    QTimer *m_timer = new QTimer;

public slots:
    void updateRenderer();
};

#endif // !CANVAS_H
