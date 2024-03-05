#ifndef CANVAS_H
#define CANVAS_H

#include <qt/QtQuick/QQuickItem>
#include <qt/QtQuick/qquickwindow.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_3_Core>
#include <QTimer>

#include <glm/vec3.hpp>
#include <iostream>

class Canvas;

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

class Renderer : public QObject, protected QOpenGLFunctions_4_3_Core {
    Q_OBJECT
public:
    // may return nullptr
    static Renderer *getRenderer();
    ~Renderer();

    void setT(qreal t) {m_t = t;}
    void setViewportSize(const QSize &size) {m_viewportSize = size;}
    void setWindow(QQuickWindow *win) { m_window = win;}

public slots:
    void init();
    void paint();
    
private:
    Renderer()
        : m_t(0)
        , m_simulatorRunner(new QTimer)
        , m_program(nullptr)
        , m_simulator(nullptr)
    {}
    static Renderer *singleton;

    
    // defined in simulator.cpp
    void initSimulator();
    void tickSimulator();
    void runSimulator();
    void stopSimulator();
    QTimer m_simulatorRunner;


    QSize m_viewportSize;
    qreal m_t;
    QOpenGLShaderProgram *m_program;
    QOpenGLShaderProgram *m_simulator;
    uint m_simulatorBuffObj;
    QQuickWindow *m_window = nullptr;

    friend class Canvas;
};



class Canvas : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
    QML_ELEMENT

public:
    Canvas()
        : m_t(0)
        , m_renderer(nullptr)
    {
        connect(
            this,
            &QQuickItem::windowChanged,
            this,
            &Canvas::handleWindowChanged
        );
    }

    qreal t() const { return m_t; }
    void setT(qreal t);

signals:
    void tChanged();

public slots:
    void sync();
    void cleanup();

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    void releaseResources() override;
    qreal m_t;
    Renderer *m_renderer;
};


#endif // !CANVAS_H
