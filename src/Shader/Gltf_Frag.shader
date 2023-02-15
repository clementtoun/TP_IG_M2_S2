#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 out_viewPos;
in vec4 FragPosLightSpace;

// struct flashLight {
//     vec3 pos;
//     vec3 dir;
//     float cutOff;
//     float outerCutOff;
//     vec3 color;
//     float linear;
//     float quadratic;
// };
// uniform flashLight fLight;

uniform sampler2D colorTexture;
uniform sampler2D metallicRoughnessTexture;
uniform sampler2D shadowMap;
uniform samplerCube shadowCubeMap;

uniform float far_plane;

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

struct PointLight {
    vec4 color;
    vec4 pos;
    vec4 attenuation;
};

struct DirectionalLight {
    vec4 color;
    vec4 dir;
};

layout (std140) uniform LightInfo { // uniform size: 1424
    //                                    // base align  // aligned offset
    //int ambientLightNum;                  // 4           // 0
    //int spotLightNum;                     // 4           // 12
    //AmbientLight ambientLight[8];         // 16          // 16
    DirectionalLight directionalLight; // 32          // 144
    PointLight pointLight;             // 48          // 400
    int directionalLightNum;
    int pointLightNum;
    //SpotLight spotLight[8];               // 80          // 784
};

const float M_PI = 3.141592653589793;

float D(float alpha, float NdotH){
    float alpha2 = alpha*alpha;
    float f = NdotH * NdotH * (alpha2 - 1.0) + 1.0;
    return alpha2 * max(NdotH, 0.f) / ( M_PI * f * f);
}

float V(float alpha, float NdotL, float HdotL, float NdotV, float HdotV) {
    float alpha2 = alpha*alpha;
    float G1 = max(HdotL, 0.f) / (abs(NdotL) + sqrt(alpha2 + (1 - alpha2) * (NdotL*NdotL)));
    float G2 = max(HdotV, 0.f) / (abs(NdotV) + sqrt(alpha2 + (1 - alpha2) * (NdotV*NdotV)));
    return G1 * G2;
}

vec3 gltf2_color(float NdotH, float NdotL, float NdotV, float HdotL, float HdotV, vec3 colorTexture, float roughness, float metallic){
    const vec3 dielectricSpecular = vec3(0.04, 0.04, 0.04);
    const vec3 black = vec3(0.f, 0.f, 0.f);

    vec3 c_diff = mix(colorTexture, black, metallic);
    vec3 f0 = mix(dielectricSpecular, colorTexture, metallic);
    float alpha = roughness*roughness;

    vec3 F = f0 + (1. - f0) * pow(max(1. - abs(HdotV), 0.f), 5.f);
    vec3 f_diffuse = (1. - F) * (1. / M_PI) * c_diff;
    vec3 f_specular = F * D(alpha, NdotH) * V(alpha, NdotL, HdotL, NdotV, HdotV);

    return f_diffuse + f_specular;
}

float ShadowCalculation(vec4 fragPosLightSpace, float NdotL)
{
        // perform perspective divide
        vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
        // transform to [0,1] range
        projCoords = projCoords * 0.5 + 0.5;
        // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
        float closestDepth = texture(shadowMap, projCoords.xy).r;
        // get depth of current fragment from light's perspective
        float currentDepth = projCoords.z;
        float bias = max(0.05 * (1.0 - NdotL), 0.005);
        // check whether current frag pos is in shadow
        // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
        // PCF
        float shadow = 0.0;
        vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
        for(int x = -1; x <= 1; ++x)
        {
            for(int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
            }
        }
        shadow /= 9.0;

        // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
        if(projCoords.z > 1.0)
            shadow = 0.0;

        return shadow;
}

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float PointShadowCalculation(vec3 fragToLight)
{
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // now test for shadows
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(out_viewPos - FragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(shadowCubeMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}

void main()
{
    vec3 color_texture = material.useColorMap == 1 ? vec3(texture(colorTexture, TexCoords)) : vec3(material.color);
    float roughness = material.roughness;
    float metallic = material.metallic;
    if (material.useMetallicRoughnessMap == 1) {
        vec3 metalRoughnessSample = vec3(texture(metallicRoughnessTexture, TexCoords));
        roughness = metalRoughnessSample.g;
        metallic = metalRoughnessSample.b;
    }

    vec3 frag_color = vec3(0.0, 0.0, 0.0);
    vec3 V, L, N, H;
    V = normalize(out_viewPos - FragPos);
    N = normalize(gl_FrontFacing ? Normal : -Normal);

    float NdotV = dot(N,V);
    float NdotH;
    float NdotL;
    float HdotL;
    float HdotV;

    if (directionalLightNum == 1) {
        L = normalize(-directionalLight.dir.xyz);
        H = normalize(L + V);

        NdotH = dot(N,H);
        NdotL = dot(N,L);
        HdotL = dot(H,L);
        HdotV = dot(H,V);

        float shadow = ShadowCalculation(FragPosLightSpace, NdotL);

        frag_color += max(NdotL, 0.0) * (1.0 - shadow) * directionalLight.color.rgb * gltf2_color(NdotH, NdotL, NdotV, HdotL, HdotV, color_texture, roughness, metallic);
    }

    float distance;
    float attenuation;

    if (pointLightNum == 1) {
        L = normalize(pointLight.pos.xyz - FragPos);
        H = normalize(L + V);
        distance = length(pointLight.pos.xyz - FragPos);
        attenuation = 1.0 / (pointLight.attenuation[3] + pointLight.attenuation[2] * distance + pointLight.attenuation[1] * distance);

        NdotH = dot(N,H);
        NdotL = dot(N,L);
        HdotL = dot(H,L);
        HdotV = dot(H,V);

        float shadow = PointShadowCalculation(FragPos - pointLight.pos.xyz);

        frag_color += max(NdotL, 0.0) * (1.0 - shadow) * pointLight.color.rgb * gltf2_color(NdotH, NdotL, NdotV, HdotL, HdotV, color_texture, roughness, metallic) * attenuation;
    }
//     L = normalize(fLight.pos - FragPos);
//     H = normalize(L + V);
//     distance = length(fLight.pos - FragPos);
//     attenuation = 1.0 / (1.0 + fLight.linear * distance + fLight.quadratic * (distance * distance));
//     float epsilon = fLight.cutOff - fLight.outerCutOff;
//     float intensity = clamp((dot(L, normalize(-fLight.dir)) - fLight.outerCutOff) / epsilon, 0.0, 1.0);
//
//     NdotH = dot(N,H);
//     NdotL = dot(N,L);
//     HdotL = dot(H,L);
//     HdotV = dot(H,V);
//
//     frag_color += max(NdotL, 0.0f) * fLight.color * gltf2_color(NdotH, NdotL, NdotV, HdotL, HdotV, color_texture, roughness, metallic) * attenuation * intensity;
    FragColor = vec4(frag_color, 1.0);
}