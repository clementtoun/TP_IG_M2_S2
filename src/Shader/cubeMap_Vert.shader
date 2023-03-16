#version 450 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

layout (std140) uniform CameraInfo { // uniform size: 144
    //                    // base align  // aligned offset
    mat4 projMat;         // 64          // 0
    mat4 viewMat;         // 64          // 64
    vec4 viewPos;         // 16          // 128
};

void main()
{
    TexCoords = aPos;
    vec4 pos = projMat * mat4(mat3(viewMat)) * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}