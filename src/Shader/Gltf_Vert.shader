#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

layout (std140) uniform CameraInfo { // uniform size: 144
    //                    // base align  // aligned offset
    mat4 projMat;         // 64          // 0
    mat4 viewMat;         // 64          // 64
    vec4 viewPos;         // 16          // 128
};

layout (std140) uniform ModelInfo { // uniform size: 64
    //                    // base align  // aligned offset
    mat4 modelMat;        // 64          // 0
    mat4 normalMat;       // 64          // 64
};

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 out_viewPos;
out mat3 TBN;

void main()
{
    FragPos = vec3(modelMat * vec4(aPos, 1.0));
    Normal = mat3(normalMat) * aNormal;
    TexCoords = aTexCoords;
    vec3 T = normalize(vec3(modelMat * vec4(aTangent, 0.0)));
    vec3 N = normalize(vec3(modelMat * vec4(aNormal, 0.0)));
    vec3 B = normalize(vec3(modelMat * vec4(aBitangent, 0.0)));
    TBN = mat3(T, B, N);
    gl_Position = projMat * viewMat * modelMat * vec4(aPos, 1.0);
}