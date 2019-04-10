uniform mat4 u_Matrix;

attribute vec3 a_Position;
attribute vec2 a_UV;
attribute vec3 a_Normal;

varying vec2 v_UV;

out vec3 Normal;
out vec3 FragPos;

void main()
{
    v_UV = a_UV;
    gl_Position = u_Matrix * vec4(a_Position, 1.0);
    Normal = a_Normal;
    FragPos = vec3(gl_Position);
}