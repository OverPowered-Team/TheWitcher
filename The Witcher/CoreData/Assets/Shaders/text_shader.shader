#shader vertex
#version 330 core
layout (location = 0) in vec3 vertex; // <vec2 pos, vec2 tex>
layout (location = 1) in vec2 uvs;
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform bool isGame;

void main()
{
    if(isGame)
        gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    else
        gl_Position = projection * view * vec4(vertex.xyz, 1.0);
    TexCoords = uvs;
}  

#shader fragment
#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec4 textColor;

void main()
{   
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = textColor * sampled;
} 