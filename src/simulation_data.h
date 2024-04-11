#ifndef SIMULATION_DATA_H
#define SIMULATION_DATA_H

#include <iostream>

#pragma pack()
struct vec3 {
    float x;
    float y;
    float z;
};

#pragma pack()
struct SimulatorData {
    vec3 position;
    char _pad[4];
    vec3 velocity;
    float mass;

    SimulatorData()
        : position(vec3 {0, 0, 0})
        , velocity(vec3 {0, 0, 0})
        , mass(1)
    {}
    SimulatorData(vec3 pos, vec3 vel, float _mass)
        : position(pos)
        , velocity(vel)
        , mass(_mass)
    {}
};

std::ostream &operator<<(std::ostream &o, SimulatorData &sim);

#endif // !SIMULATION_OBJECT_DATA_TYPE_H
