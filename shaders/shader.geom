#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (triangles) in;
layout (triangle_strip, max_vertices=6) out;

layout (location = 0) in vec3 fragIn[3];
layout (location = 0) out vec3 fragOut;

void main() {
    gl_Position = gl_in[0].gl_Position;
    fragOut = fragIn[1];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    fragOut = fragIn[2];
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    fragOut = fragIn[0];
    EmitVertex();
    EndPrimitive();

    gl_Position = gl_in[0].gl_Position + vec4(vec3(0.1), 1.0);
    fragOut = fragIn[2];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + vec4(vec3(0.1), 1.0);
    fragOut = fragIn[0];
    EmitVertex();

    gl_Position = gl_in[2].gl_Position + vec4(vec3(0.1), 1.0);
    fragOut = fragIn[1];
    EmitVertex();
    EndPrimitive();
}
