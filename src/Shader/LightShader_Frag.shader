#version 450 core

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
    FragColor = vec4(pow(vec3(material.color), vec3(2.2)), 1.0);
}