#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 uvs;
layout(location = 2) in vec3 normals;

uniform mat4 view; 
uniform mat4 model;
uniform mat4 projection;

out vec3 frag_pos;
out vec3 norms;
out vec3 vertex;


void main()
{
    vec4 pos = vec4(position, 1.0);
    frag_pos = vec3(model * pos);
    vertex = position;
    //norms = (model * vec4(normals,0)).xyz;
    norms = normalize(mat3(transpose(inverse(model))) * normals);
    gl_Position = projection * view * vec4(frag_pos, 1.0f); 
    
};

#shader fragment
#version 330 core


uniform float shieldStrength;
uniform float shieldCooldown;
uniform float exponent;

const vec4 red = vec4(1.0, 0.5, 0.5, 0.5);
const vec4 blue = vec4(0.5, 0.5, 1.0, 1.0);
const vec4 hitColour = vec4(1.0, 0.5, 0.5, 1.0);

in vec3 frag_pos;
in vec3 norms;
in vec3 vertex;

uniform vec4 diffuse_color;

out vec4 FragColor;



void main()
{
	vec4 color = mix(red, diffuse_color, shieldStrength);
	FragColor = color;
	float fresnel = 1.0 - abs(normalize(norms).z);
	fresnel = pow(fresnel, exponent);
	fresnel += 0.05 * (1.0 - fresnel);

	FragColor.a = clamp(fresnel, 0.0, 1.0) * shieldCooldown;

	//FragColor.a *= 0.25f + 0.75f * (1.0f - abs(normalize(norms).z));






       // vec4 color = mix(red, blue, shieldStrength);
//	vec4 fillColour = color * 0.15;
	
//	vec3 eyenorm = normalize(-frag_pos);

//	float fresnel = 1.0 - abs(dot(eyenorm, norms)); 
//	fresnel = pow(fresnel, exponent);
//	fresnel += 0.05 * (1.0 - fresnel);
	
//	float sumIntensity = 0.0;
	
//	float clampedInt = clamp(sumIntensity, 0.0, 1.0);
	
	
//	color.a = (fillColour.a + clamp(fresnel * 0.5, 0.0, 1.0)) * shieldCooldown;
	
//	color = color + (hitColour * clampedInt);

//	FragColor = color;

        


}
