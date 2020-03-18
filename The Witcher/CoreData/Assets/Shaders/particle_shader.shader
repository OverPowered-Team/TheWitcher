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
    gl_Position = projection * view * model * vec4(position, 1.0f);
    texCoords = vec2(uvs.x, uvs.y);
};

#shader fragment
#version 330 core

uniform sampler2D tex;
uniform vec4 diffuse_color;
// Ins
in vec2 texCoords;
// Outs
out vec4 FragColor;

void main()
{
    vec4 textureColor = texture(tex, texCoords);
    

    if(textureColor.a < 0.001)
         discard;

     if(textureColor == vec4(0,0,0,1))
     {
        FragColor = diffuse_color;
     }
    else
     {
        FragColor = textureColor * diffuse_color;
     }
   
    
}
