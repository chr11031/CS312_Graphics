uniform mat4 u_Matrix;
uniform mat4 u_model;
uniform mat4 u_projection;
uniform mat4 u_view;

attribute vec3 a_Position;
attribute vec2 a_UV;
attribute vec3 a_normal;

//Part of the calculations take place in the Vertex Shader.

varying vec2 v_UV;
varying vec3 v_fragPos;
varying vec3 v_normal;  //This is going out to fragment shader

void main()
{
	v_UV = a_UV;
	gl_Position = u_projection * u_view * u_model * vec4(a_Position, 1.0);

	v_fragPos = vec3(u_model * vec4(a_Position, 1.0));
	v_normal =  vec3(u_model * vec4(a_normal, 1.0));
}
