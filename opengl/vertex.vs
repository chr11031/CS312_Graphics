uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;
attribute vec3 a_Position;
attribute vec2 a_UV;
attribute vec3 a_Normal;

varying vec3 v_FragPos;
varying vec3 v_Normal;
varying vec2 v_UV;

void main()
{
    v_FragPos = vec3(u_Model * vec4(a_Position, 1.0));
    v_Normal = normalize(vec3(u_Model * vec4(a_Normal, 0)));

    v_UV = a_UV;
    gl_Position = u_Proj * u_View * vec4(v_FragPos, 1.0);
}
