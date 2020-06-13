#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 uvs;
layout(location = 2) in vec3 normals; 
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;
layout (location = 5) in vec3 tangents;
layout (location = 6) in vec3 biTangents;

uniform mat4 view; 
uniform mat4 model;
uniform mat4 projection;

out vec3 frag_pos;

void main()
{
    vec4 pos = vec4(position, 1.0);
	frag_pos = vec3(model * pos);

    gl_Position = projection * view * vec4(frag_pos, 1.0f);
};


#shader fragment
#version 330 core

uniform float speed;
uniform float movement;

uniform sampler2D t_channel;

in vec3 frag_pos;

out vec4 FragColor;

mat2 makem2(float theta)
{
	float c = cos(theta);
	float s = sin(theta);

	return mat2(c, -s, s, c);
}

float noise(vec2 x)
{
	return texture2D(t_channel, x * .01).x;
}

vec2 gradn(vec2 p)
{
	float ep = .09;
	float grad_x = noise(vec2(p.x + ep, p.y)) - noise(vec2(p.x - ep, p.y));
	float grad_y = noise(vec2(p.x, p.y + ep)) - noise(vec2(p.x, p.y - ep));

	return vec2(grad_x, grad_y);
}

float flow(vec2 p)
{
	float z = 2.0;
	float rz = 0.0;

	vec2 bp = p;

	for (float i = 1.0; i < 7.0; i++)
	{
		p += movement * 0.6;
		bp += movement * 1.9;

		vec2 gr = gradn(i * p * 0.34 + movement * 1.);
		gr *= makem2(movement * 6.0 - (0.05 * p.x + 0.03 * p.y) * 40.0);

		p += gr * .5;

		rz += (sin(noise(p) * 7.0) * 0.5 + 0.5) / z;

		p = mix(bp, p, 0.77);
		z *= 1.4;

		p *= 2.0;
		bp *= 2.0;
	}

	return rz;
}

void main()
{
	/*vec2 p = frag_pos.xy / iResolution.xy - 0.5;
	p.x *= iResolution.x / iResolution.y;
	p *= 4.0;*/

	vec2 p = frag_pos.xy / vec2(250.0, 250.0) - 0.5;
	p.x *= 250.0 / 250.0;
	p *= 4.0;
	
	float rz = flow(p);

	//vec3 col = vec3(0.5, 0.75, 0.01) / rz;
	vec3 col = vec3(.2,0.07,0.01) / rz;

	col = pow(col, vec3(1.4));
	FragColor = vec4(col, 1.0);
}
