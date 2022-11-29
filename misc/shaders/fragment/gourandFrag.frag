#version 330 core
out vec4 fragColor;

in vec3 lightingColor; 
in vec3 diffuseColor;

void main()
{
   fragColor = vec4(lightingColor * diffuseColor, 1.0);
}