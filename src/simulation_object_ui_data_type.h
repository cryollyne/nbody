#ifndef SIMULATION_OBJECT_UI_DATA_TYPE_H
#define SIMULATION_OBJECT_UI_DATA_TYPE_H

struct SimulatorObject {
    Q_GADGET

    Q_PROPERTY(QVector3D position MEMBER m_position)
    Q_PROPERTY(QVector3D velocity MEMBER m_velocity)
    Q_PROPERTY(float mass MEMBER m_mass)

public:
    QVector3D m_position;
    QVector3D m_velocity;
    float m_mass;

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
        : m_position(obj.m_position)
        , m_velocity(obj.m_velocity)
        , m_mass(obj.m_mass)
    {}
};
Q_DECLARE_METATYPE(SimulatorObject)

#endif // !SIMULATION_OBJECT_UI_DATA_TYPE_H
