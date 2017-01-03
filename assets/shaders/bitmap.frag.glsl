#version 450

layout (location = 0) in vec2 in_uv;

layout (location = 0) out vec4 out_color;

uniform vec4 u_color;
layout (binding = 0) uniform sampler2D u_tex;

void main()
{
    out_color = u_color * texture(u_tex, in_uv);
}
