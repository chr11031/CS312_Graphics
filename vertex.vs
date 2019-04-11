uniform mat4 u_Matrix;
attribute vec3 a_Position;
attribute vec2 a_UV;
attribute vec3 a_Normal;
varying vec2 v_UV;
varying vec3 FragPos;
varying vec3 Normal;


void main()
{
    v_UV = a_UV;

    // Get rid of translation or it will mess up our normals. Trim to mat3
    Normal = a_Normal;
    // Normal = mat3(u_Matrix[0].rgb, u_Matrix[1].rgb, u_Matrix[2].rgb) *  a_Normal;
    // Normal = (u_Matrix * vec4(a_Normal, 0.0)).xyz;


    gl_Position = u_Matrix * vec4(a_Position, 1.0);
    FragPos = gl_Position.rgb;
}