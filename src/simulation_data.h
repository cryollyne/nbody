#ifndef SIMULATION_DATA_H
#define SIMULATION_DATA_H

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

#endif // !SIMULATION_OBJECT_DATA_TYPE_H
