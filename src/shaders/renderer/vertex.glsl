#version 430 core
struct Object {
    vec3 position;
    vec3 velocity;
    float mass;
};

uniform mat4 view;
uniform mat4 projection;
uniform float zoom;
uniform bool orthographic;

uniform int focus = -1;

const float minSize = 2;
const float maxSize = 20;

layout(binding = 0, std430) buffer SSBO {
    Object obj[];
} val;

vec4 perspectiveDivide(vec4 i) {
    return vec4(
        i.x/i.w,
        i.y/i.w,
        i.z,
        1
    );
}

void main() {
    vec4 position = vec4(val.obj[gl_InstanceID].position, 1);
    vec4 focusPosition = vec4(0);
    if (focus >= 0)
        focusPosition = vec4(val.obj[focus].position, 0);

    float size = pow(val.obj[gl_InstanceID].mass, 1.0/3.0);

    if (orthographic) {
        gl_PointSize = clamp(size/zoom, minSize, maxSize);
        gl_Position = projection * view*(position - focusPosition);
    } else {
        vec4 relPosition = projection * view*(position - focusPosition);
        gl_Position = perspectiveDivide(relPosition);
        gl_PointSize = clamp(size/relPosition.w, minSize, maxSize);
    }
}
