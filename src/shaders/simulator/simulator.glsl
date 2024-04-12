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
    vec3 netAcc = vec3(0);
    for (int i = 0; i < n; i++) {
        if (i != gl_GlobalInvocationID.x) {
            Object other = val.obj[i];
            vec3 disp = other.position - current.position;

            // g = G m2 / r^2   (gravitational field strength is acceleration)
            float acc = (G * other.mass) / dot(disp, disp);

            netAcc += acc * normalize(disp);
        }
    }

    val.obj[gl_GlobalInvocationID.x].velocity += netAcc * dt;
    val.obj[gl_GlobalInvocationID.x].position += val.obj[gl_GlobalInvocationID.x].velocity * dt;
}
