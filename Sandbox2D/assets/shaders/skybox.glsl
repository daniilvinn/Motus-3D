#stage vertex
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 0) out vec3 TexCoord;

layout(set = 0, binding = 0) uniform CameraData {
    mat4 viewProjection;
    mat4 projection;
    mat4 view;
} camera;

layout(push_constant) uniform Transform {
    mat4 matrix;
} transform;

void main(){
    TexCoord = aPos;
    vec4 pos = camera.projection * mat4(mat3(camera.view)) * transform.matrix * vec4(aPos, 1.0f);
    gl_Position = pos.xyww;
}

#stage fragment
#version 460 core

layout(location = 0) in vec3 TexCoord;
layout(location = 0) out vec4 finalColor;

layout(set = 1, binding = 0) uniform samplerCube aSampler;

void main(){
    finalColor = texture(aSampler, TexCoord);
}