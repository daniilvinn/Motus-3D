#stage vertex
#version 460 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;

layout(location = 0) out vec4 Color;

layout(push_constant) uniform Transform {
    mat4 matrix;
} transform;

layout(set = 0, binding = 0) uniform CameraData {
    mat4 viewProjection;
    mat4 projection;
    mat4 view;
} camera;

void main()
{
    gl_Position = camera.viewProjection * transform.matrix * vec4(aPos, 0.0f, 1.0f);
    Color = vec4(aColor, 1.0f);
}

#stage fragment
#version 460 core

layout(location = 0) in vec4 Color;
layout(location = 0) out vec4 finalColor;

void main(){
    finalColor = Color;
}