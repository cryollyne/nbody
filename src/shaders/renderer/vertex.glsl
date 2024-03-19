#version 430 core
struct Object {
    vec3 position;
    vec3 velocity;
    float mass;
};

uniform mat4 view;

layout(binding = 0, std430) buffer SSBO {
    Object obj[];
} val;

void main() {
    vec4 position = vec4(val.obj[gl_InstanceID].position, 1);
    gl_PointSize = 5.0;
    gl_Position = view*position;
}
