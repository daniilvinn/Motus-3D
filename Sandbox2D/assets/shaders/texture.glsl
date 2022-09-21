#stage vertex
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

layout(location = 1) out vec2 fragTexCoord;

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
    gl_Position = camera.viewProjection * transform.matrix * vec4(aPos, 1.0f);
    fragTexCoord = aTexCoord;
}

#stage fragment
#version 460 core

layout(location = 1) in vec2 fragTexCoord;
layout(location = 0) out vec4 finalColor;

layout(set = 1, binding = 0) uniform sampler2D textureSampler;

void main(){
    vec4 fragColor = texture(textureSampler, fragTexCoord);
    finalColor.rgb = pow(fragColor.rgb, vec3(1.0f / 2.2f));
    finalColor.a = fragColor.a;
}