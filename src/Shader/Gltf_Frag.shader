#version 450 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

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

uniform int hasCubeMap;
uniform int shadowActive;

uniform sampler2D colorTexture;
uniform sampler2D metallicRoughnessTexture;
uniform sampler2D normalTexture;
uniform sampler2D aoTexture;
uniform sampler2D emissiveTexture;

uniform sampler2DArray shadowMap;
uniform samplerCube shadowCubeMap;
uniform samplerCube skybox;

uniform float far_plane;
uniform mat4 lightSpaceMatrices[16];
uniform float cascadePlaneDistances[16];
uniform int cascadeCount;

struct Material { // uniform size: 48
//shared metal/dielectric
    vec4 color;
    float metallic;
    float roughness;
    int useColorMap;
    int useMetallicRoughnessMap;
    int useNormalTexture;
    int useAoTexture;
    int useEmissive;
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

layout (std140) uniform CameraInfo { // uniform size: 144
    //                    // base align  // aligned offset
    mat4 projMat;         // 64          // 0
    mat4 viewMat;         // 64          // 64
    vec4 viewPos;         // 16          // 128
};

const float M_PI = 3.141592653589793;

float D_GGX(float alpha, float NdotH){
    float alpha2 = alpha*alpha;
    float denom = NdotH * NdotH * (alpha2 - 1.0) + 1.0;
    return alpha2 * max(NdotH, 0.0) / (M_PI * denom * denom);
}

float V_GGX(float alpha, float NdotL, float HdotL, float NdotV, float HdotV) {
    float alpha2 = alpha*alpha;
    float G1 = max(HdotL, 0.f) / (abs(NdotL) + sqrt(alpha2 + (1 - alpha2) * (NdotL*NdotL)));
    float G2 = max(HdotV, 0.f) / (abs(NdotV) + sqrt(alpha2 + (1 - alpha2) * (NdotV*NdotV)));
    return G1 * G2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - abs(cosTheta), 0.0, 1.0), 5.0);
}

float ShadowCalculation(vec3 fragPosWorldSpace, vec3 N, vec3 L)
{
    // select cascade layer
    vec4 fragPosViewSpace = viewMat * vec4(fragPosWorldSpace, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;
    for (int i = 0; i < cascadeCount; ++i)
    {
        if (depthValue < cascadePlaneDistances[i])
        {
            layer = i;
            break;
        }
    }
    if (layer == -1)
    {
        layer = cascadeCount;
    }

    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPosWorldSpace, 1.0);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (currentDepth > 1.0)
    {
        return 0.0;
    }
    // calculate bias (based on depth map resolution and slope)
    float bias = max(0.05 * (1.0 - dot(N, L)), 0.005);
    const float biasModifier = 0.5f;
    if (layer == cascadeCount)
    {
        bias *= 1 / (far_plane * biasModifier);
    }
    else
    {
        bias *= 1 / (cascadePlaneDistances[layer] * biasModifier);
    }

    bias = 0;

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

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
    float bias = 0.04;
    bias = 0;
    int samples = 20;
    float viewDistance = length(vec3(viewPos) - FragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(shadowCubeMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}

void main()
{
    vec4 color_rgba = material.useColorMap == 1 ? texture(colorTexture, TexCoords) : material.color;
    vec3 color_texture = color_rgba.rgb;
    float ao = material.useAoTexture == 1 ? pow(texture(aoTexture, TexCoords).r, 2.2) : 1.0;
    vec3 emissive = material.useEmissive == 1 ? pow(texture(emissiveTexture, TexCoords).rgb, vec3(2.2)) : vec3(0.0);
    float roughness = material.roughness;
    float metallic = material.metallic;
    if (material.useMetallicRoughnessMap == 1) {
        vec3 metalRoughnessSample = vec3(texture(metallicRoughnessTexture, TexCoords));
        roughness = metalRoughnessSample.g;
        metallic = metalRoughnessSample.b;
    }

    vec3 frag_color = vec3(0.0, 0.0, 0.0);

    vec3 c_diff = mix(color_texture, vec3(0.0,0.0,0.0), metallic);
    vec3 F0 = mix(vec3(0.04, 0.04, 0.04), color_texture, metallic);
    float alpha = roughness * roughness;

    vec3 V, L, N, H, R;
    V = normalize(vec3(viewPos) - FragPos);
    N = gl_FrontFacing ? Normal : -Normal;
    if(material.useNormalTexture == 1)
        N = normalize(TBN * normalize(texture(normalTexture, TexCoords).rgb * 2.0 - 1.0));
    N = normalize(N);

    float NdotV, NdotH, NdotL, HdotL, HdotV, shadow;
    vec3 F, f_diffuse, ambient, f_specular;

    if (hasCubeMap == 1) {
        R = reflect(-V, N);
        H = normalize(R - V);

        NdotV = dot(N,V);
        NdotH = dot(N,H);
        NdotL = dot(N,R);
        HdotL = dot(H,R);
        HdotV = dot(H,V);

        F = fresnelSchlick(HdotV, F0);
        f_diffuse = (1 - F) * c_diff ;
        ambient = f_diffuse * pow(texture(skybox, R).rgb, vec3(2.2)) * ao * 0.03 * vec3(color_rgba);
    }
    else {
        ambient = vec3(0.03) * vec3(color_rgba) * ao;
    }

    if (directionalLightNum == 1) {
        L = normalize(-directionalLight.dir.xyz);
        H = normalize(L + V);

        NdotH = dot(N,H);
        NdotL = dot(N,L);
        HdotL = dot(H,L);
        HdotV = dot(H,V);

        F = fresnelSchlick(max(HdotV, 0.0), F0);
        f_diffuse = (1 - F) * (1 / M_PI) * c_diff;
        f_specular = F * D_GGX(alpha, NdotH) * V_GGX(alpha, NdotL, HdotL, NdotV, HdotV);

        shadow = ShadowCalculation(FragPos, N, L) * shadowActive;

        frag_color += max(NdotL, 0.0) * (1.0 - shadow) * directionalLight.color.rgb * (f_diffuse + f_specular);
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

        F = fresnelSchlick(max(HdotV, 0.0), F0);
        f_diffuse = (1 - F) * (1 / M_PI) * c_diff;
        f_specular = F * D_GGX(alpha, NdotH) * V_GGX(alpha, NdotL, HdotL, NdotV, HdotV);

        shadow = PointShadowCalculation(FragPos - pointLight.pos.xyz) * shadowActive;

        frag_color += max(NdotL, 0.0) * (1.0 - shadow) * pointLight.color.rgb * (f_diffuse + f_specular) * attenuation;
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
    FragColor = vec4(frag_color + emissive + ambient, color_rgba.w);
}