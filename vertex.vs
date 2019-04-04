uniform mat4 u_Matrix;
attribute vec3 a_Position;
attribute vec2 a_UV;
attribute vec3 a_Normal;

varying vec2 v_UV;
varying vec3 v_Normal;
varying vec4 v_Pos;

void main()
{
    v_UV = a_UV;
    vec4 nom = vec4(a_Normal, 0.0); 
    v_Pos =  u_Matrix * vec4(a_Position, 1.0);
    gl_Position = v_Pos; 
    v_Normal.xyz = normalize(u_Matrix * nom).xyz;
}