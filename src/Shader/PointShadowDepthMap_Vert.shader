#version 450 core
layout (location = 0) in vec3 aPos;

layout (std140) uniform ModelInfo { // uniform size: 64
    //                    // base align  // aligned offset
    mat4 modelMat;        // 64          // 0
    mat4 normalMat;       // 64          // 64
};

void main()
{
    gl_Position = modelMat * vec4(aPos, 1.0);
}