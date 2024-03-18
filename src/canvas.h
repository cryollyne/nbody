#ifndef CANVAS_H
#define CANVAS_H

#include <qt/QtQuick/QQuickItem>
#include <qt/QtQuick/qquickwindow.h>
#include <QOpenGLShaderProgram>
#include <QQuickFramebufferObject>
#include <QTimer>
#include <QQueue>

#include <variant>
#include <glm/vec3.hpp>

#include "simulation_object_ui_data_type.h"
#include "simulation_data.h"

namespace RenderCommand {
    class Render{};
    class Simulator{};
    class SynchronizeObjects{};
    class SetObject {
        public:
        uint32_t index;
        SimulatorData data;
    };
    class AddObject{};
    class DeleteObject {
        public:
        uint32_t index;
    };

    // runs asynchronously
    using RenderCommand = std::variant<
          Render
    >;

    // runs synchronously during the synchronization stage
    using SimulatorCommand = std::variant<
          Simulator
        , SynchronizeObjects
        , SetObject
        , AddObject
        , DeleteObject
    >;

    using Command = std::variant<
          RenderCommand
        , SimulatorCommand
    >;
}

class Canvas : public QQuickFramebufferObject {
    Q_OBJECT
    QQuickFramebufferObject::Renderer *createRenderer() const override;

    QTimer *m_simulatorTimer = new QTimer;
    QTimer *m_frameTimer = new QTimer;
    QTimer *m_objectUpdateTimer = new QTimer;
    QQueue<RenderCommand::Command> *m_commandQueue = new QQueue<RenderCommand::Command>;



    Q_PROPERTY(QVariantList objects READ getObjects NOTIFY objectsChanged)
    Q_PROPERTY(float tickRate READ getTickRate WRITE setTickRate NOTIFY tickRateChanged)
    Q_PROPERTY(float frameUpdateRate READ getFrameUpdateRate WRITE setFrameUpdateRate NOTIFY frameUpdateRateChanged)
    Q_PROPERTY(float objectUpdateRate READ getObjectUpdateRate WRITE setObjectUpdateRate NOTIFY objectUpdateRateChanged)
    Q_PROPERTY(bool isSimulationRunning READ isSimulationRunning WRITE setIsSimulationRunning NOTIFY isSimulationRunningChanged)

    QVariantList m_objects {};
    float m_simulatorTickRate = 60.0f;
    float m_frameUpdateRate = 30.0f;
    float m_objectUpdateRate = 10.0f;
    bool m_isSimulationRunning = true;

public:
    QVariantList getObjects() const;
    float getTickRate() const;
    float getFrameUpdateRate() const;
    float getObjectUpdateRate() const;
    bool isSimulationRunning() const;

    void setTickRate(float rate);
    void setFrameUpdateRate(float rate);
    void setObjectUpdateRate(float rate);
    void setIsSimulationRunning(bool r);

signals:
    void objectsChanged();
    void tickRateChanged();
    void frameUpdateRateChanged();
    void objectUpdateRateChanged();
    void isSimulationRunningChanged();

public slots:
    void tickSimulator();
    void updateRenderer();
    void synchronizeObjects();

    void setObject(int index, QVector3D position, QVector3D velocity, float mass);
    void addObject();
    void deleteObject(int index);

    friend class SimRenderer;
};

#endif // !CANVAS_H
