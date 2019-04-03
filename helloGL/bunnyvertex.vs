uniform mat4 u_Matrix;
attribute vec3 a_Position;
attribute vec2 a_UV;

varying vec2 v_UV;
varying vec3 v_Normal;

void main()
{
    v_UV = a_UV;
    gl_Position = u_Matrix * vec4(a_Position, 1.0);
    v_Normal = a_Position;
}