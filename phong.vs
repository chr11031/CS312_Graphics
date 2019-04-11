uniform mat4 u_MVPMatrix;
uniform mat4 u_MVMatrix;
uniform mat4 u_MMatrix;

attribute vec3 a_Position;
attribute vec2 a_UV;
attribute vec3 a_Normal;

varying vec2 v_UV;
varying vec3 v_Normal;
varying vec3 v_Position;

void main()
{
    v_UV = a_UV;
    gl_Position = u_MVPMatrix * vec4(a_Position, 1.0);
    v_Normal = vec3(u_MMatrix * vec4(a_Normal, 0.0));
    v_Position = vec3(u_MVMatrix * vec4(a_Position, 1.0));
}
