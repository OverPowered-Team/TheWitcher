#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 uvs;


uniform mat4 view; 
uniform mat4 model;
uniform mat4 projection;

uniform float density = 0;
uniform float gradient = 0;

out vec3 frag_pos;
out vec2 texCoords;
out float visibility; 

void main()
{
    vec4 pos = vec4(position, 1.0);
    frag_pos = vec3(model * pos);
    texCoords = vec2(uvs.x, uvs.y);
    gl_Position = projection * view * vec4(frag_pos, 1.0f); 
    
    vec4 worldPos = model * pos;
    vec4 positionRelativeToCam = view * worldPos;
    float distance = length(positionRelativeToCam.xyz);
    visibility = exp(-pow((distance * density), gradient));
    visibility = clamp(visibility, 0.0, 1.0);
};

#shader fragment
#version 330 core
struct Material {
    vec4 diffuse_color;

    sampler2D diffuseTexture;
    bool hasDiffuseTexture;

    bool emissive;
    
};
uniform Material objectMaterial;

uniform bool activeFog;
uniform vec3 backgroundColor;

// Ins
in vec2 texCoords;
in float visibility;

// Outs
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BloomColor;

void main()
{
    vec4 objectColor = objectMaterial.diffuse_color;

    if(objectMaterial.hasDiffuseTexture == true)
    {
        objectColor = objectColor * vec4(texture(objectMaterial.diffuseTexture, texCoords));

        if (objectColor.w < 0.0001)
            discard;
    }

    FragColor = objectColor;
     
    if(activeFog)
    {
        FragColor = mix(vec4(backgroundColor, 0.0), FragColor, visibility);
    }

    if(objectMaterial.emissive)
    {
        BloomColor = vec4(FragColor.rgb, 1.0f);
    }
    else 
    {
        BloomColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}
