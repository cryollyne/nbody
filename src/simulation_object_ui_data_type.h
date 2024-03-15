#ifndef SIMULATION_OBJECT_UI_DATA_TYPE_H
#define SIMULATION_OBJECT_UI_DATA_TYPE_H

class SimulatorObject : public QObject {
    Q_OBJECT
    QVector3D m_position;
    QVector3D m_velocity;
    float m_mass;

    Q_PROPERTY(QVector3D pos READ getPosition NOTIFY positionChanged)
    Q_PROPERTY(QVector3D vel READ getVelocity NOTIFY velocityChanged)
    Q_PROPERTY(float mass READ getMass NOTIFY massChanged)

public:
    QVector3D getPosition() const { return m_position; }
    QVector3D getVelocity() const { return m_velocity; }
    float getMass() const { return m_mass; }

signals:
    void positionChanged();
    void velocityChanged();
    void massChanged();

public:
    SimulatorObject &operator=(const SimulatorObject &obj) {
        m_position = obj.m_position;
        m_velocity = obj.m_velocity;
        m_mass = obj.m_mass;
        return *this;
    }
    SimulatorObject(glm::vec3 pos, glm::vec3 vel, float mass)
        : m_position(pos.x, pos.y, pos.z)
        , m_velocity(vel.x, vel.y, vel.z)
        , m_mass(mass)
    {}
    SimulatorObject()
        : m_position()
        , m_velocity()
        , m_mass()
    {}
    SimulatorObject(const SimulatorObject &obj)
        : QObject(nullptr)
        , m_position(obj.m_position)
        , m_velocity(obj.m_velocity)
        , m_mass(obj.m_mass)
    {}
};
Q_DECLARE_METATYPE(SimulatorObject)

#endif // !SIMULATION_OBJECT_UI_DATA_TYPE_H
