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

class Canvas : public QQuickFramebufferObject {
    Q_OBJECT
    QQuickFramebufferObject::Renderer *createRenderer() const override;

    QTimer *m_simulatorTimer = new QTimer;
    QTimer *m_frameTimer = new QTimer;
    QQueue<RenderCommand::Command> *m_commandQueue = new QQueue<RenderCommand::Command>;



    Q_PROPERTY(float tickRate READ getTickRate WRITE setTickRate NOTIFY tickRateChanged)
    Q_PROPERTY(float frameUpdateRate READ getFrameUpdateRate WRITE setFrameUpdateRate NOTIFY frameUpdateRateChanged)
    Q_PROPERTY(bool isSimulationRunning READ isSimulationRunning WRITE setIsSimulationRunning NOTIFY isSimulationRunningChanged)

    float m_simulatorTickRate = 60.0f;
    float m_frameUpdateRate = 30.0f;
    bool m_isSimulationRunning = true;

public:
    float getTickRate() const;
    float getFrameUpdateRate() const;
    bool isSimulationRunning() const;

    void setTickRate(float rate);
    void setFrameUpdateRate(float rate);
    void setIsSimulationRunning(bool r);

signals:
    void tickRateChanged();
    void frameUpdateRateChanged();
    void isSimulationRunningChanged();

public slots:
    void tickSimulator();
    void updateRenderer();

    friend class SimRenderer;
};

#endif // !CANVAS_H
