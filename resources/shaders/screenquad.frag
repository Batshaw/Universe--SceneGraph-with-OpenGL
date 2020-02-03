#version 150

// input form vert
in vec2 text_coords;

uniform sampler2D colorTexture;
uniform bool ShaderMode_grey;
uniform bool ShaderMode_verticalMirror;
uniform bool ShaderMode_horizontalMirror;
uniform bool ShaderMode_blur;