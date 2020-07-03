#version 330 core
out vec4 FragColor;

in vec2 texcoord;

struct Polygon
{
    vec3 rotation;
    vec3 scale;
    mat4 position;
};

void main()
{  	
    FragColor = vec4(1,texcoord.y,1, 1.0);
} 
