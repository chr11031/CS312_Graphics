uniform mat4 u_Model;
attribute vec3 a_Position;
attribute vec2 a_UV;
varying vec2 v_UV;
uniform mat4 u_View;
uniform mat4 u_Proj;
attribute vec3 a_Normal;
varying vec3 v_FragPos;
varying vec3 v_Normal;

uniform mat4 u_Matrix;

void main()
{
    v_UV = a_UV;
    gl_Position = u_Matrix * vec4(a_Position, 1.0);
    v_FragPos = vec3(u_Model * vec4(a_Position, 1.0));
	v_Normal = normalize(vec3(u_Model * vec4(a_Normal, 0)));
}