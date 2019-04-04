// MVP Matrix stuff
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

// Attribute UV, Position, and Normal
attribute vec3 a_Position;
attribute vec2 a_UV;
attribute vec3 a_Normal;

// Vector UV, Position, and Normal
varying vec2 v_UV;
varying vec3 Normal;
varying vec3 FragPos;

// Does the vertex shader step
void main()
{
    // UV Coor stuff
    v_UV = a_UV;

    // Goes though MVP with the position attribute
    gl_Position = u_Proj * u_View * u_Model * vec4(a_Position, 1.0);
    FragPos     = vec3(u_View * u_Model * vec4(a_Position, 1.0));
    // Does the Normal math during the M Phase of MVP
    Normal      = vec3(u_Model * vec4(a_Normal, 0.0));
}