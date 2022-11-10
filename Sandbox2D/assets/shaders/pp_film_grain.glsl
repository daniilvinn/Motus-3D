#stage compute
#version 460 core

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

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

    float strength = 16.0f;

    vec4 pixel_data = imageLoad(input_image, ivec2(gl_GlobalInvocationID.xy));
    float x = (pixel_x + 4.0f ) * (pixel_y + 4.0f ) * (0.5f * 10.0f);
	vec4 grain = vec4(mod((mod(x, 13.0) + 1.0) * (mod(x, 123.0) + 1.0), 0.01)-0.005) * strength;
    
    pixel_data.rgb = pixel_data.rgb * grain.rgb;
    imageStore(input_image, ivec2(pixel_x, pixel_y), pixel_data);
}