#version 450

layout (location = 0) in vec4 in_billboard;
layout (location = 1) in vec4 in_posSize;
layout (location = 2) in vec4 in_color;

layout (location = 0) out vec2 out_uv;
layout (location = 1) out vec4 out_color;

uniform vec2 u_worldSize;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    gl_Position = vec4(in_posSize.xy + (1 / u_worldSize) * in_billboard.xy, 0, 1);
    out_uv = in_billboard.zw;
    out_color = in_color;
}
