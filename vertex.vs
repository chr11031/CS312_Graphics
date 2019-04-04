uniform mat4 u_Matrix;

attribute vec3 a_Position;
attribute vec2 a_UV;
attribute vec3 a_Normal;

uniform mat4 u_Model;

varying vec2 v_UV;
varying vec3 v_Normal;
varying vec3 fragPos;

void main()
{
    v_UV = a_UV;

    gl_Position = u_Matrix * vec4(a_Position, 1.0);
    fragPos = vec3(u_Model * vec4(a_Position, 1.0));

    v_Normal = vec3(u_Model * vec4(a_Normal, 0.0));
}