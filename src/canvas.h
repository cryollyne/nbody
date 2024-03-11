#ifndef CANVAS_H
#define CANVAS_H

#include <qt/QtQuick/QQuickItem>
#include <qt/QtQuick/qquickwindow.h>
#include <QOpenGLShaderProgram>
#include <QQuickFramebufferObject>
#include <QTimer>
#include <QQueue>

#include <glm/vec3.hpp>
#include <iostream>
#include <variant>

namespace RenderCommand {
    class Render{};
    class Simulator{};

    using Command = std::variant<
          Render
        , Simulator
    >;
}

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

class Canvas;

class SimRenderer : public QQuickFramebufferObject::Renderer {
public: 
    SimRenderer(const QQuickFramebufferObject *fbo)
        : m_item((const Canvas*)fbo)
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
    uint32_t m_simulatorBuffObj;
};

class Canvas : public QQuickFramebufferObject {
    QQuickFramebufferObject::Renderer *createRenderer() const override;

    QTimer *m_simulatorTimer = new QTimer;
    QTimer *m_frameTimer = new QTimer;
    QQueue<RenderCommand::Command> *m_commandQueue = new QQueue<RenderCommand::Command>;
public slots:
    void tickSimulator();
    void updateRenderer();

    friend class SimRenderer;
};

#endif // !CANVAS_H
