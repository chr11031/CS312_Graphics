uniform mat4 u_Matrix;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

attribute vec3 a_Position;
attribute vec2 a_UV;
attribute vec3 a_Normal;

varying vec3 v_Position;
varying vec2 v_UV;
varying vec3 v_Normal;

void main()
{
	v_Position = vec3(u_Model * vec4(a_Position, 1.0));
    v_Normal   = normalize(vec3(u_Model * vec4(a_Normal, 0)));

    v_UV = a_UV;
    gl_Position = u_Projection * u_View * u_Model * vec4(v_Position, 1.0);
}