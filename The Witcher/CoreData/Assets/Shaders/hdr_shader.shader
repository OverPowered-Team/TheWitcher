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

uniform bool hdr;
uniform float exposure;
uniform float gamma;

void main()
{             
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

    if(hdr)
    {
        // reinhard
        vec3 result = hdrColor / (hdrColor + vec3(1.0));

        // exposure
        result = vec3(1.0) - exp(-hdrColor * exposure);

        // also gamma correct while we're at it     
        if(gamma != 0)
            result = pow(result, vec3(1.0 / gamma));
         

        // Final
        FragColor = vec4(result, 1.0);
    }
    else
    {
        vec3 result = hdrColor; 
        if(gamma != 0)
             result = pow(result, vec3(1.0 / gamma));
             
        FragColor = vec4(hdrColor, 1.0);
    }
}