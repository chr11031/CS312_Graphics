uniform mat4 u_Matrix;
uniform mat4 u_Model;

attribute vec3 a_Position;
attribute vec2 a_UV;
attribute vec3 a_Normal;

varying vec2 v_UV;
varying vec3 v_Normal;
varying vec3 frag_pos;

void main()
{
    v_UV = a_UV;

    gl_Position = u_Matrix * vec4(a_Position, 1.0);
    frag_pos = vec3(u_Model * vec4(a_Position, 1.0));
    v_Normal = vec3(u_Model * vec4(a_Normal, 0.0));
}