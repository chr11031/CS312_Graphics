uniform mat4 u_Matrix;
attribute vec3 a_Position;
attribute vec2 a_UV;
attribute vec3 a_Normal;

varying vec2 v_UV;
varying vec3 v_Normal;
varying vec3 v_FragPos;
varying vec3 v_viewPos;

void main()
{
    v_UV = a_UV;
    v_Normal = a_Normal;

    gl_Position = u_Matrix * vec4(a_Position, 1.0);
    v_FragPos = vec3(gl_Position);
}