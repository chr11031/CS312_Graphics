uniform mat4 u_Matrix;
uniform mat4 u_Model;
attribute vec3 a_Position;
attribute vec2 a_UV;
attribute vec3 a_Normal;

varying vec2 v_UV;
varying vec3 v_Normal;
varying vec3 FragPos;

void main()
{
	v_UV = a_UV;
	gl_Position = u_Matrix * vec4(a_Position, 1.0);
	FragPos = vec3(u_Model * vec4(a_Position, 0.0));
	v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
}