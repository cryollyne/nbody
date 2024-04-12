#ifndef CANVAS_H
#define CANVAS_H

#include <QQuickItem>
#include <qquickwindow.h>
#include <QOpenGLShaderProgram>
#include <QQuickFramebufferObject>
#include <QTimer>
#include <QQueue>

#include <variant>

#include "simulation_object_ui_data_type.h"
#include "simulation_data.h"

namespace RenderCommand {
    class Render{};
    class MoveCamera{
        public:
        float x;
        float y;
    };
    class ZoomCamera {
        public:
        float amount;
    };
    class FocusObject {
        public:
        int index;
    };
    class SetProjection {
        public:
        bool orthographic;
    };
    class SetFov {
        public:
        float fov; // radians
    };

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
    class SetTimeStep {
        public:
        float dt;
    };

    // runs asynchronously
    using RenderCommand = std::variant<
          Render
        , MoveCamera
        , ZoomCamera
        , FocusObject
        , SetProjection
        , SetFov
    >;

    // runs synchronously during the synchronization stage
    using SimulatorCommand = std::variant<
          Simulator
        , SynchronizeObjects
        , SetObject
        , AddObject
        , DeleteObject
        , SetTimeStep
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
    Q_PROPERTY(float sensitivity READ getSensitivity WRITE setSensitivity NOTIFY sensitivityChanged)
    Q_PROPERTY(float zoomSensitivity READ getZoomSensitivity WRITE setZoomSensitivity NOTIFY zoomSensitivityChanged)
    Q_PROPERTY(float fov READ getFov WRITE setFov NOTIFY fovChanged)
    Q_PROPERTY(float timeRatio READ getTimeRatio WRITE setTimeRatio NOTIFY timeRatioChanged)
    Q_PROPERTY(int focusIndex READ getFocusIndex WRITE setFocusIndex NOTIFY focusIndexChanged)
    Q_PROPERTY(bool cameraInvert READ getCameraInvert WRITE setCameraInvert NOTIFY cameraInvertChanged)
    Q_PROPERTY(bool zoomInvert READ getZoomInvert WRITE setZoomInvert NOTIFY zoomInvertChanged)
    Q_PROPERTY(bool orthographic READ isOrthographic WRITE setOrthographic NOTIFY orthographicChanged)
    Q_PROPERTY(bool isSimulationRunning READ isSimulationRunning WRITE setIsSimulationRunning NOTIFY isSimulationRunningChanged)


    QVariantList m_objects {};
    float m_simulatorTickRate = 60.0f;
    float m_frameUpdateRate = 30.0f;
    float m_objectUpdateRate = 10.0f;
    float m_cameraSensitivity = 0.005f;
    float m_zoomSensitivity = 0.001;
    float m_fov = 90;
    float m_timeRatio = 60*60*24*5;
    int m_focusIndex = -1;

    bool m_cameraInvert = false;
    bool m_zoomInvert = false;
    bool m_orthographic = false;
    bool m_isSimulationRunning = true;

public:
    QVariantList getObjects() const;
    float getTickRate() const;
    float getFrameUpdateRate() const;
    float getObjectUpdateRate() const;
    float getSensitivity() const;
    float getZoomSensitivity() const;
    float getFov() const;
    float getTimeRatio() const;
    int getFocusIndex() const;
    bool getCameraInvert() const;
    bool getZoomInvert() const;
    bool isOrthographic() const;
    bool isSimulationRunning() const;

    void setTickRate(float rate);
    void setFrameUpdateRate(float rate);
    void setObjectUpdateRate(float rate);
    void setSensitivity(float sensitivity);
    void setZoomSensitivity(float sensitivity);
    void setFov(float fov);
    void setTimeRatio(float ratio);
    void setFocusIndex(int index);
    void setCameraInvert (bool invert);
    void setZoomInvert(bool invert);
    void setOrthographic(bool ortho);
    void setIsSimulationRunning(bool r);

signals:
    void objectsChanged();
    void tickRateChanged();
    void frameUpdateRateChanged();
    void objectUpdateRateChanged();
    void sensitivityChanged();
    void zoomSensitivityChanged();
    void fovChanged();
    void timeRatioChanged();
    void focusIndexChanged();
    void cameraInvertChanged();
    void zoomInvertChanged();
    void orthographicChanged();
    void isSimulationRunningChanged();

public slots:
    void tickSimulator();
    void updateRenderer();
    void synchronizeObjects();

    void setObject(int index, QVector3D position, QVector3D velocity, float mass);
    void addObject();
    void deleteObject(int index);

    void moveCamera(float x, float y);
    void zoomCamera(float amount);

    friend class SimRenderer;
};

#endif // !CANVAS_H
