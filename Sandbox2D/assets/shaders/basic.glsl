#stage vertex
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;

layout(location = 0) out vec4 Color;

void main()
{
    gl_Position = vec4(aPos, 1.0f);
    Color = aColor;
}

#stage fragment
#version 460 core

layout(location = 0) in vec4 Color;
layout(location = 0) out vec4 finalColor;

void main(){
    finalColor = Color;
}