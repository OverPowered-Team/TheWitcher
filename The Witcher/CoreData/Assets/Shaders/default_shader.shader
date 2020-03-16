#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 uvs;
layout(location = 2) in vec3 normals; 
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

const int MAX_BONES = 100;

uniform mat4 gBones[MAX_BONES];
uniform mat4 view; 
uniform mat4 model;
uniform mat4 projection;
uniform int animate;


out vec3 frag_pos;
out vec2 texCoords;
out vec3 norms;

void main()
{
    vec4 pos = vec4(position, 1.0);
    vec4 normal4 = vec4(normals, 1.0);
    if(animate == 1)
    {
          mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
            BoneTransform += gBones[BoneIDs[1]] * Weights[1];
            BoneTransform += gBones[BoneIDs[2]] * Weights[2];
            BoneTransform += gBones[BoneIDs[3]] * Weights[3];
            pos = BoneTransform * pos;
    }
    frag_pos = vec3(model * pos);
    texCoords = vec2(uvs.x, uvs.y);
    norms = mat3(transpose(inverse(model))) * normals;

    gl_Position = projection * view * vec4(frag_pos, 1.0f); 
};


#shader fragment
#version 330 core

// Properties array index
#define indexPosition 0
#define indexAmbient 1
#define indexDiffuse 2
#define indexSpecular 3
#define indexDirection 4

// Structs
struct DirectionalLight
{
    float intensity;
    vec3 dirLightProperties[5];
};

struct PointLight
{
    float intensity;
    vec3 pointLightProperties[4];
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
    float intensity;
    vec3 spotLightProperties[5];
    float constant;
    float linear;
    float quadratic;
    float cut_off;
    float outer_cut_off;
};

// Function declarations
vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);

// Uniforms
uniform sampler2D tex;
uniform vec3 diffuse_color;

// max_dir_lights, max_point_lights, max_spot_lights
uniform ivec3 max_lights;

uniform vec3 view_pos;

#define MAX_LIGHTS_PER_TYPE 10
uniform DirectionalLight dir_light[MAX_LIGHTS_PER_TYPE];
uniform PointLight point_light[MAX_LIGHTS_PER_TYPE];
uniform SpotLight spot_light[MAX_LIGHTS_PER_TYPE];

// Ins
in vec2 texCoords;
in vec3 frag_pos;
in vec3 norms;

// Outs
out vec4 FragColor;

void main()
{
    // ----------------------- Object Color ---------------------
    vec4 objectColor = vec4(0);
    vec4 textureColor = vec4(texture(tex, texCoords).rgb, 1.0);
    if(textureColor != vec4(0,0,0,1))
    {
        objectColor = textureColor * vec4(diffuse_color, 1.0f);
    }
    else 
    {
        objectColor = vec4(diffuse_color,1.0f);
    }

    // ----------------------------------------------------------


    // ------------------------- Light --------------------------

    vec3 result = vec3(0);
    vec3 norm = normalize(norms);
    vec3 view_dir = normalize(view_pos - frag_pos);

    // Light calculations
    for(int i = 0; i < max_lights.x; i++)
        result += CalculateDirectionalLight(dir_light[i], norm, view_dir);
    for(int i = 0; i < max_lights.y; i++)
        result += CalculatePointLight(point_light[i], norm, frag_pos, view_dir);    
    for(int i = 0; i < max_lights.z; i++)
        result += CalculateSpotLight(spot_light[i], norm, frag_pos, view_dir);   

    // ----------------------------------------------------------

    FragColor = vec4(result, 1.0) * objectColor;
}

// Function definitions
vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir)
{
    // Intensity
    float intensity = light.intensity;

    // Ambient
    vec3 ambient = light.dirLightProperties[indexAmbient];
    
    // Diffuse
    vec3 lightDir = normalize(-light.dirLightProperties[indexDirection]);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.dirLightProperties[indexDiffuse] * diff;
    
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(view_dir, reflectDir), 0.0), 32);
    vec3 specular = light.dirLightProperties[indexSpecular] * spec;

    return (ambient + diffuse + specular) * vec3(intensity, intensity, intensity);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
    // Intensity
    float intensity = light.intensity;

    // Ambient
    vec3 ambient = light.pointLightProperties[indexAmbient];

    // diffuse shading
    vec3 lightDir = normalize(light.pointLightProperties[indexPosition] - frag_pos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.pointLightProperties[indexDiffuse] * diff;

     // Specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(view_dir, reflectDir), 0.0), 32);
    vec3 specular = light.pointLightProperties[indexSpecular] * spec;

    // attenuation
    float distance = length(light.pointLightProperties[indexPosition] - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    return (ambient + diffuse + specular) * attenuation * vec3(intensity, intensity, intensity);
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
    // Intensity
    float intensity = light.intensity;

    // ambient
    vec3 ambient = light.spotLightProperties[indexAmbient];
    
    // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.spotLightProperties[indexPosition] - frag_pos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.spotLightProperties[indexDiffuse] * diff;  
    
    // specular
    vec3 viewDir = normalize(view_pos - frag_pos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.spotLightProperties[indexSpecular] * spec;  
    
    // soft edges
    float theta = dot(lightDir, normalize(-light.spotLightProperties[indexDirection])); 
    float epsilon = (light.cut_off - light.outer_cut_off);
    float edge_intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);
    diffuse  *= edge_intensity;
    specular *= edge_intensity;
    
    // attenuation
    float distance = length(light.spotLightProperties[indexPosition] - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient *= attenuation; 
    diffuse *= attenuation;
    specular *= attenuation;   
        
    return (ambient + diffuse + specular) * intensity;
}