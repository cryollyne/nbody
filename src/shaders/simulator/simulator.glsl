#version 430 core
layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Object {
    vec3 position;
    vec3 velocity;
    float mass;
};

layout(binding = 0, std430) buffer SSBO {
    Object obj[];
} val;

uniform float dt;

const float G = 6.67e-11;


void main() {
    Object current = val.obj[gl_GlobalInvocationID.x];

    uint n = gl_NumWorkGroups.x;
    vec3 netForce = vec3(0);
    for (int i = 0; i < n; i++) {
        if (i != gl_GlobalInvocationID.x) {
            Object other = val.obj[i];
            vec3 disp = other.position - current.position;

            // F = G m1 m2 / r^2
            float force = (G * current.mass * other.mass) / dot(disp, disp);

            netForce += force * normalize(disp);
        }
    }

    vec3 acc = netForce / current.mass;

    val.obj[gl_GlobalInvocationID.x].velocity += acc * dt;
    val.obj[gl_GlobalInvocationID.x].position += val.obj[gl_GlobalInvocationID.x].velocity * dt;
}
