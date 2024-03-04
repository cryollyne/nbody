#version 430 core
struct Object {
    vec3 position;
    vec3 velocity;
    float mass;
};

layout(binding = 0, std430) buffer SSBO {
    Object obj[];
} val;

void main() {
    gl_PointSize = 5.0;
    gl_Position = vec4(val.obj[gl_InstanceID].position, 1);
}
