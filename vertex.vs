uniform mat4 u_Matrix;
attribute vec3 a_Position;
attribute vec2 a_UV;
attribute vec3 a_Normal;

varying vec4 v_Position;
varying vec2 v_UV;
varying vec3 v_Normal;

void main()
{
    vec4 norm = vec4(a_Normal, 0.0);
    v_UV = a_UV;
    v_Position = u_Matrix * vec4(a_Position, 1.0);
    gl_Position = v_Position;
    v_Normal.xyz = (normalize(u_Matrix * norm)).xyz;
}