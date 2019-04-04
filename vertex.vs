#version 330 

uniform mat4 u_Matrix;
attribute vec3 a_Position;
attribute vec3 a_Normal;
attribute vec2 a_UV;
uniform mat4 u_Model;

varying vec2 v_UV;
varying vec4 v_objPosition;
varying vec3 v_Norm;
varying vec3 fragPos;

void main()
{
	v_UV = a_UV;
	gl_Position = u_Matrix * vec4(a_Position, 1.0);
	fragPos	= vec3(u_Model * vec4(a_Position, 1.0));
	v_Norm = vec3(u_Model * vec4(a_Normal, 0.0));
	
}
