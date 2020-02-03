#version 150
#extension GL_ARB_explicit_attrib_location : require

// enable vertex attributes of VAO
layout(location = 0) in vec2 in_Position;
layout(location = 1) in vec2 in_TexCoords;

// send texture coordination to fragment
out vec2 text_coords;

void main() {
    // 2D -> z = 0, no transformation
    gl_Position = vec4(in_Position, 0.0, 1.0);
    text_coords = in_TexCoords;
}