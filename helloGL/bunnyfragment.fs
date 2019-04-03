uniform sampler2D u_Texture;
uniform vec3 u_LightPosition;
uniform vec3 u_LightColor;
uniform vec3 u_ViewPosition;
uniform float u_Kd;
uniform float u_Ka;
uniform float u_Ks;

varying vec2 v_UV;
varying vec3 v_Normal;

void main()
{
    vec4 sample = texture2D(u_Texture, v_UV);

    vec3 ambient = u_Ka * u_LightColor;

    vec3 diffuse = u_Kd * u_LightColor * dot((u_LightPosition - v_Normal), v_Normal);

    //vec3 reflection = dot(v_Normal, -u_LightPosition);
    //vec3 specular = u_Ks * u_LightColor * pow(max(dot((u_ViewPosition - v_Normal), reflection), 0), 32);

    vec3 sum = ambient + diffuse;// + specular;

    gl_FragColor = vec4(sum, 1.0);
}

vec3 dot(vec3 a, vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}