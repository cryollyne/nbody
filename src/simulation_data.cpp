#include "simulation_data.h"
std::ostream &operator<<(std::ostream &o, SimulatorData &sim) {
    return o << "pos: { " << sim.position.x << ", " << sim.position.y << ", " << sim.position.z << " }, "
             << "vel: { " << sim.velocity.x << ", " << sim.velocity.y << ", " << sim.velocity.z << " }, "
             << "mass: " << sim.mass;
}
