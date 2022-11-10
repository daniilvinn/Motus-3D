#stage compute
#version 460 core

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(rgba16f, set = 0, binding = 0) uniform image2D input_image;

layout(set = 1, binding = 1) uniform PostProcessingData {
    float gamma;
    float exposure;
} data;

bool PixelExists(ivec2 coords) {
    if(coords.x + 1 > imageSize(input_image).x) return false;
    if(coords.y + 1 > imageSize(input_image).y) return false;

    return true;
}

void main()
{
    uint pixel_x = gl_GlobalInvocationID.x;
    uint pixel_y = gl_GlobalInvocationID.y;

    if(!PixelExists(ivec2(pixel_x, pixel_y))) {
        return;
    };

    vec4 pixel_data = imageLoad(input_image, ivec2(gl_GlobalInvocationID.xy));
    pixel_data.rgb = vec3(1.0) - exp(-pixel_data.rgb * data.exposure);

    imageStore(input_image, ivec2(pixel_x, pixel_y), pixel_data);
}