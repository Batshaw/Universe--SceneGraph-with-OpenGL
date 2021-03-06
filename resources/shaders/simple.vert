#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TextCoords;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;

// pass the outputs to fragment
out vec3 pass_Normal;
out vec3 fragment_pos;
out vec3 camera_pos;
out vec2 texture_coord;


void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	pass_Normal = (NormalMatrix * vec4(in_Normal, 0.0f)).xyz;
	fragment_pos = (ModelMatrix * vec4(in_Position, 1.0f)).xyz;
	camera_pos = (ViewMatrix * vec4(fragment_pos, 1.0f)).xyz;
	texture_coord = in_TextCoords;
}
