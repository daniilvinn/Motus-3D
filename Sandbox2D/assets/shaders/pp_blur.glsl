#stage compute
#version 460 core

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1);
layout(rgba16f, set = 0, binding = 0) uniform image2D input_image;

bool PixelExists(ivec2 coords) {
    if(coords.x + 1 > imageSize(input_image).x) return false;
    if(coords.y + 1 > imageSize(input_image).y) return false;

    return true;
}

void main(){

    uint pixel_x = gl_GlobalInvocationID.x;
    uint pixel_y = gl_GlobalInvocationID.y;

    if(!PixelExists(ivec2(pixel_x, pixel_y))) {
        return;
    };

    float Pi = 6.28318530718; // Pi*2
    
    float Directions = 16.0;
    float Quality = 3.0;
    float Size = 8.0;
   
    vec2 Radius = Size/imageSize(input_image).xy;

}