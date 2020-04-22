#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 uvs;
layout(location = 2) in vec3 normals; 
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;
layout (location = 5) in vec3 tangents;
layout (location = 6) in vec3 biTangents;

uniform mat4 model;


uniform mat4 lightSpaceMatrix; 

void main()
{
    // --------------- OUTS ---------------
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0);
};


#shader fragment
#version 330 core

out vec4 FragColor;
void main()
{
    // ----------------------- Object Color ---------------------
  FragColor = vec4(1.0f);
}
