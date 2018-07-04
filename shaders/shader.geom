#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

layout (location = 0) in vec3 frag_in[3];
layout (location = 0) out vec3 frag_out;

void main() {
    gl_Position = gl_in[0].gl_Position;
    frag_out = frag_in[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    frag_out = frag_in[1];
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    frag_out = frag_in[2];
    EmitVertex();
    EndPrimitive();
}
