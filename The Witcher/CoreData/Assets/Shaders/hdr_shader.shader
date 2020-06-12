#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor; 

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform sampler2D bloomBlur; 

uniform bool bloom;
uniform bool hdr;
uniform float exposure;
uniform float gamma;

void main()
{             
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    vec3 result = hdrColor;

    if(hdr)
    {
        if(bloom)
            result = hdrColor + bloomColor;

        // Tone mapping with exposure
        result = vec3(1.0) - exp(-result * exposure);

        // Gamma correct while we're at it     
        if(gamma != 0)
            result = pow(result, vec3(1.0 / gamma));
         
        // Final
        FragColor = vec4(result, 1.0);
    }
    else
    {
        if(gamma != 0)
             result = pow(result, vec3(1.0 / gamma));
             
        FragColor = vec4(result, 1.0);
    }
}