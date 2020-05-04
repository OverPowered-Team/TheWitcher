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
uniform mat4 lightSpaceMatrix;
//uniform int animate;

out vec3 frag_pos;
out vec2 texCoords;
out vec3 norms;
out vec4 FragPosLightSpace;

void main()
{
    vec4 pos = vec4(position, 1.0);
    frag_pos = vec3(model * pos);
    texCoords = vec2(uvs.x, uvs.y);
    norms = transpose(inverse(mat3(model))) * normals;
    FragPosLightSpace = lightSpaceMatrix * vec4(frag_pos, 1.0);
    gl_Position = projection * view * vec4(frag_pos, 1.0); 
};


#shader fragment
#version 330 core

//functions
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);
vec3 DrawShadows(vec3 ObjectColor, vec3 normal, vec3 frag_pos, vec3 view_dir);

//uniforms
uniform bool hasDiffuseTexture;
uniform sampler2D diffuseTexture;

uniform sampler2D depthMap;
uniform vec3 diffuse_color;
uniform vec3 lightPos;
uniform vec3 viewPos;

// Ins
in vec2 texCoords;
in vec3 frag_pos;
in vec3 norms;
in vec4 FragPosLightSpace;

// Outs
out vec4 FragColor;

void main()
{
    // ----------------------- Object Color ---------------------
  
    vec3 objectColor = diffuse_color;
    if(hasDiffuseTexture == true)
    {
        objectColor = objectColor * texture(diffuseTexture, texCoords).rgb;
    }
    // ----------------------------------------------------------
    vec3 norm = normalize(norms);
    vec3 view_dir = normalize(viewPos - frag_pos);

    vec3 shadowColor = DrawShadows(objectColor,norm,frag_pos,view_dir);

    // ------------------------- Light --------------------------
    //shadowColor = shadowColor * objectColor;
    //float depthVal = texture(depthMap, texCoords).r;
    FragColor = vec4(shadowColor,1.0);
    // ----------------------------------------------------------


}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(depthMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  

    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

vec3 DrawShadows(vec3 ObjectColor, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
    vec3 lightColor = vec3(1.0);
    // ambient
    vec3 ambient = 0.15 * ObjectColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - frag_pos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + view_dir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace, normal, lightDir);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * ObjectColor;    
    
    return lighting;
}