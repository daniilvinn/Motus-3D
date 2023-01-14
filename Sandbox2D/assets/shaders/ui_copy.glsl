#stage vertex
#version 460 core

layout (location = 0) out vec2 texCoord;

vec2 vertexPositions[] = {
    vec2(-1.0f, -1.0f),
    vec2(1.0f, -1.0f),
    vec2(1.0f,  1.0f),
    vec2(-1.0f, 1.0f),
}

vec2 textureCoordinates[] = {
    vec2(0.0f, 1.0f),
    vec2(1.0f, 1.0f),
    vec2(1.0f, 0.0f),
    vec2(0.0f, 0.0f),
}

void main(){

    gl_Position = vec3(vertexPositions[gl_VertexID], 0.0f);
    texCoord = textureCoordinates[gl_VertexID];
}

#stage fragment
#version 460 core

layout(location = 0) in vec2 texCoord;
layout(location = 0) out vec4 color;
layout(set = 0, binding = 0) uniform sampler2D rendered_UI;

void main() {
    color = texture(rendered_UI, texCoord);
}