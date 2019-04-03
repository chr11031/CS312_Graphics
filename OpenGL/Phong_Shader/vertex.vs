uniform mat4 u_Matrix;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

attribute vec3 a_Position;
attribute vec3 a_Normal;
attribute vec2 a_UV;

varying vec2 v_UV;
varying vec3 v_Normal;
varying vec3 v_Position;

void main()
{
    v_UV = a_UV;
    v_Normal = vec3(u_Model * vec4(a_Normal, 0.0));
    v_Position = vec3(u_Model * vec4(a_Position, 1.0));
    
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
}