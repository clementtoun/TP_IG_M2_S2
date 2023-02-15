#version 330 core

out vec4 FragColor;

struct Material { // uniform size: 48
//shared metal/dielectric
    vec4 color;
    float metallic;
    float roughness;
    int useColorMap;
    int useMetallicRoughnessMap;
};

layout (std140) uniform MaterialInfo {
    Material material;
};

void main()
{
    FragColor = material.color;
}