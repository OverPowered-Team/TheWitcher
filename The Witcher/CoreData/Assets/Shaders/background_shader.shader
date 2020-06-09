
#shader vertex
#version 330 core
layout(location = 2) in vec3 vertex; 

uniform mat4 projection;
uniform mat4 view;
uniform bool isGame;

void main()
{
    if (isGame)
        gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    else
        gl_Position = projection * view * vec4(vertex.xyz, 1.0);
}

#shader fragment
#version 330 core
out vec4 color;

uniform vec3 textColor;

void main()
{
    color = vec4(textColor, 1.0);
}