uniform mat4 u_Matrix;
attribute vec3 a_Position;
attribute vec2 a_UV;
attribute vec3 a_Normal;
varying vec2 v_UV;
varying vec3 normal;
varying vec3 fragPos;

void main()
{
    v_UV = a_UV;
    normal = a_Normal;

    gl_Position = u_Matrix * vec4(a_Position, 1.0);
    fragPos = vec3(gl_Position.r, gl_Position.g, gl_Position.b);
}