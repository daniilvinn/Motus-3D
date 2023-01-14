/*
*   VERTEX SHADER
*/

#stage vertex

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 texCoord;

layout(location = 0) out vec3 fTexCoord;
layout(location = 1) out vec3 fWorldPosition;

layout(push_constant) uniform Transform {
	mat4 matrix;
} transform;

layout(set = 0, binding = 0) uniform CameraData {
    mat4 viewProjection;
    mat4 projection;
    mat4 view;
} camera;

void main() {
    fTexCoord = texCoord;
    fWorldPosition = vec3(transform.matrix * vec4(position, 1.0f));
    gl_Position = camera.viewProjection * vec4(fWorldPosition, 1.0f);
}


/*
*   FRAGMENT SHADER
*/
#stage fragment

layout(location = 0) in vec3 fTexCoord;
layout(location = 1) in vec3 fWorldPosition;

layout(set = 1, binding = 0) uniform sampler2D albedo;
layout(set = 1, binding = 1) uniform sampler2D normalMap;
layout(set = 1, binding = 2) uniform sampler2D roughnessMap;
layout(set = 1, binding = 3) uniform sampler2D metallicMap;

struct LightSource {
    vec4 position;
    vec4 color;
};

layout(set = 2, binding = 0) uniform LightSourcesArray {
    int size;
    LightSource lights[1024];
} light_sources;

const float PI = 3.14159265359;
vec3 MapNormal(){
    vec3 tangentNormal = texture(normalMap, fTexCoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fWorldPosition);
    vec3 Q2  = dFdy(fWorldPosition);
    vec2 st1 = dFdx(fTexCoord);
    vec2 st2 = dFdy(fTexCoord);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main() 
{
    vec4 initial_color = texture(albedo, fTexCoord);
    vec3 N = normalize(texture(material.normalMap, fTexCoord));
    vec V = normalize()

}