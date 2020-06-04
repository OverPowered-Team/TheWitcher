#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uvs;

uniform mat4 view; 
uniform mat4 model;
uniform mat4 projection;

uniform float currUV;
uniform float nextUV;

uniform vec3 vertex1;
uniform vec3 vertex2;
uniform vec3 vertex3;
uniform vec3 vertex4;


out vec3 frag_pos;
out vec2 texCoords;


void main()
{
		
vec3 realPos = vec3(0.0, 0.0, 0.0);
vec2 realCoord = uvs;

if (uvs.x == 0)
	{
		realCoord.x = currUV;
		
		if (uvs.y == 1)
			realPos = vertex1;
		else
			realPos = vertex2;
	}
	
	else
	{
		realCoord.x = nextUV;
		
		if (uvs.y == 1)
			realPos = vertex3;
		else
			realPos = vertex4;
	}

frag_pos = vec3(model * vec4(realPos, 1.0));
texCoords = realCoord;
gl_Position =  projection * view * vec4(frag_pos, 1.0);
    
}

#shader fragment
#version 330 core
struct Material {
    vec4 diffuse_color;

    sampler2D diffuseTexture;
    bool hasDiffuseTexture;

    
};
uniform Material objectMaterial;

uniform bool activeFog;
uniform vec3 backgroundColor;

// Ins
in vec2 texCoords;


// Outs
out vec4 FragColor;


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
     
   
    

}