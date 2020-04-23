#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 uvs;
layout(location = 2) in vec3 normals; 
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;
layout (location = 5) in vec3 tangents;
layout (location = 6) in vec3 biTangents;

const int MAX_BONES = 100;

uniform mat4 gBones[MAX_BONES];
uniform int animate;

uniform mat4 model;
uniform mat4 lightSpaceMatrix; 

void main()
{
    vec4 pos = vec4(position, 1.0);

     // --------------- Animation -------------
    if(animate == 1)
    {
          mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
            BoneTransform += gBones[BoneIDs[1]] * Weights[1];
            BoneTransform += gBones[BoneIDs[2]] * Weights[2];
            BoneTransform += gBones[BoneIDs[3]] * Weights[3];
            pos = BoneTransform * pos;
    }
    // --------------------------------------- 
    vec3 frag_pos = vec3(model * pos);
    gl_Position = lightSpaceMatrix * vec4(frag_pos, 1.0f);
};


#shader fragment
#version 330 core

out vec4 FragColor;
void main()
{
    // ----------------------- Object Color ---------------------
  FragColor = vec4(1.0f);
}