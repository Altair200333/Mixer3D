#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

out vec2 texcoord;
out float r;
uniform samplerBuffer VertexSampler0;
void main() 
{
    vec3 inVertex = texelFetch(VertexSampler0, 0).xyz;
    r = inVertex.x;
    gl_Position = vec4( 1.0, 1.0, 0.5, 1.0 );
    texcoord = vec2( 1.0, 1.0 );
    EmitVertex();

    gl_Position = vec4(-1.0, 1.0, 0.5, 1.0 );
    texcoord = vec2( 0.0, 1.0 ); 
    EmitVertex();

    gl_Position = vec4( 1.0,-1.0, 0.5, 1.0 );
    texcoord = vec2( 1.0, 0.0 ); 
    EmitVertex();

    gl_Position = vec4(-1.0,-1.0, 0.5, 1.0 );
    texcoord = vec2( 0.0, 0.0 ); 
    EmitVertex();

    EndPrimitive(); 
}