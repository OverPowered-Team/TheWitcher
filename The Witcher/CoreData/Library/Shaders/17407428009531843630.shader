#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 uvs;
layout(location = 2) in vec3 vNormal; 

uniform mat4 view; 
uniform mat4 model;
uniform mat4 projection;
uniform float time;

out vec2 texCoords;
out vec3 normal; 
out vec3 fragPos; 

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); 

    fragPos = vec3(model * vec4(position, 1.0f));
    texCoords = vec2(uvs.x, uvs.y);
    normal = vNormal;
};

#shader fragment
#version 330 core

uniform sampler2D tex;

uniform vec3 lightPos; 
uniform vec3 lightColor;

uniform float ambientStrength;
uniform float specularStrength;

uniform vec3 objectColor;
uniform vec3 viewPos; 
// uniform vec3 ambient_color;
// uniform vec3 diffuse_color;

in vec2 texCoords;
in vec3 normal;
in vec3 fragPos; 

out vec4 FragColor;


void main()
{
    // FragColor = vec4(texture(tex, texCoords).rgb,1.0) * ambient_color;
    // Ambient
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor; 
    
    // Specular 
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Final Color
    vec3 result = (ambient + diffuse + specular) * vec4(texture(tex, texCoords).rgb,1.0);
    FragColor = vec4(result, 1.0f);
}
