#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec3 uvs;

uniform mat4 view; 
uniform mat4 model;
uniform mat4 projection;

uniform float time;
uniform float mult_time;

uniform float amplitude;

out vec3 position_l;
out float max_height;

void main()
{
    position_l = position;

    position_l.z += sin(time * mult_time + (position.x * 50.0 + position.y * 50.0)) * amplitude;

    max_height = sin(1) * amplitude;

    gl_Position = projection * view * model * vec4(position_l, 1.0f);
};

#shader fragment
#version 330 core

out vec4 color;
 
in vec3 position_l;
in float max_height;

float red_factor;
float green_factor;
float blue_factor;

void main()
{
	red_factor = position_l.z + position_l.z / max_height + max_height / 2.0;
	green_factor = position_l.z + position_l.z / max_height + max_height / 2.0;
	blue_factor = 1.0;

    color = vec4(red_factor, green_factor, blue_factor, 1.0);
}

