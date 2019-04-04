uniform mat4 u_Matrix;
uniform mat4 u_Model;
attribute vec3 a_Position;
attribute vec2 a_UV;
attribute vec3 a_Normal;

varying vec3 v_Position;
varying vec2 v_UV;
varying vec3 v_Normal;

void main()
{
	v_Position = a_Position;
	v_UV = a_UV;
	v_Normal = a_Normal;

	gl_Position = u_Matrix * vec4(a_Position, 1.0);
}