#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 uvs;
layout(location = 2) in vec3 normals; 
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;
layout (location = 5) in vec3 tangents;
layout (location = 6) in vec3 biTangents;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float iTime;
uniform float speed;

uniform int positionOrUV;

uniform float max_distance;
uniform float actual_distance;

uniform int water_texture_size;
out vec2 textureCoords;

vec2 waterTextureMovment(vec2 uv, vec3 cdir)
{    
    // Parallax height distortion with two directional waves at
    // slightly different angles.
    uv *= water_texture_size;
    vec2 a = 0.02 * cdir.xz / cdir.y; // Parallax offset
    float h = sin(uv.x + iTime); // Height at UV
    uv += a * h;
    h = sin(0.841471 * uv.x - 0.540302 * uv.y + iTime);
    uv += a * h;


    return uv;
}

void main() {
    //float posz = sin(iTime + position.y) * 0.1;
    vec4 pos = vec4(position.x, position.y, 0.0, 1.0f);
    vec3 dir = normalize(position);
    textureCoords = waterTextureMovment(vec2(uvs.x, uvs.y), dir);
    textureCoords.x += speed * 0.2;

    gl_Position = projection * view * model * pos;
};


#shader fragment
#version 330 core

struct Material {
    vec4 diffuse_color;

    sampler2D diffuseTexture;
    bool hasDiffuseTexture;

    sampler2D normalMap;
    bool hasNormalMap;
};

uniform Material objectMaterial;

in vec2 textureCoords;

out vec4 FragColor;

void main()
{
    if(objectMaterial.hasDiffuseTexture == true){
        FragColor = texture(objectMaterial.diffuseTexture, textureCoords);

        FragColor = mix(FragColor, objectMaterial.diffuse_color, 0.5);
    }
    else{
        FragColor = objectMaterial.diffuse_color;
    }
}

