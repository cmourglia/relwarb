#version 450

layout (location = 0) in vec2 in_uv;
layout (location = 0) out vec4 out_color;

layout (binding = 0) uniform sampler2D u_tex;
uniform vec4 u_color;

void main()
{
    float r = texture(u_tex, in_uv).r;
    out_color = u_color;

    out_color.a *= r;
}
