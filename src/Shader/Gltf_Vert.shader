#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

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

void main()
{
    gl_Position = projMat * viewMat * modelMat * vec4(aPos, 1.0);
    vec4 pos = modelMat * vec4(aPos,1.0);
    FragPos = vec3(pos.xyz) / pos.w;
    Normal = mat3(normalMat) * aNormal;
    TexCoords = aTexCoords;
    out_viewPos = vec3(viewPos);
}