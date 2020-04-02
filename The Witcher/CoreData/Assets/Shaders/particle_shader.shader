#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 uvs;


uniform mat4 view; 
uniform mat4 model;
uniform mat4 projection;


out vec3 frag_pos;
out vec2 texCoords;

void main()
{
    vec4 pos = vec4(position, 1.0);
    frag_pos = vec3(model * pos);
    texCoords = vec2(uvs.x, uvs.y);
    gl_Position = projection * view * vec4(frag_pos, 1.0f); 
    
};

#shader fragment
#version 330 core
struct Material {
    vec3 diffuse_color;

    sampler2D diffuseTexture;
    bool hasDiffuseTexture;

    
};
uniform Material objectMaterial;
// Ins
in vec2 texCoords;
// Outs
out vec4 FragColor;

void main()
{
    vec4 objectColor = vec4(objectMaterial.diffuse_color, 1.0f);

    if(objectMaterial.hasDiffuseTexture == true)
    {
        objectColor = objectColor * vec4(texture(objectMaterial.diffuseTexture, texCoords));

        if (objectColor.w < 0.0001)
            discard;
    }

    FragColor = objectColor;
     
}
