#version 430 core
struct Object {
    vec3 position;
    vec3 velocity;
    float mass;
};

uniform mat4 view;
uniform mat4 projection;
uniform bool orthographic;

uniform int focus = -1;

layout(binding = 0, std430) buffer SSBO {
    Object obj[];
} val;

void main() {
    vec4 position = vec4(val.obj[gl_InstanceID].position, 1);
    vec4 focusPosition = vec4(0);
    if (focus >= 0)
        focusPosition = vec4(val.obj[focus].position, 0);
    gl_PointSize = 5.0;
    if (orthographic) {
        gl_Position = projection * view*(position - focusPosition);
    } else {
        // TODO
    }
}
