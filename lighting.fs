uniform sampler2D u_Texture;
uniform float u_Threshold;
uniform vec3 u_LightColor;
uniform vec3 u_LightPos;
uniform vec3 u_CameraPos;
uniform vec3 u_Ambient;
uniform vec3 u_Diffuse;
uniform vec3 u_Specular;
uniform float u_Shiny;

varying vec4 v_Position;
varying vec2 v_UV;
varying vec3 v_Normal;

void main()
{
    vec3 ambient = u_Ambient * u_LightColor;

    vec3 lightDir = vec3(u_LightPos.xyz - v_Position.xyz);
    vec3 diffuse = max(dot(normalize(lightDir), normalize(v_Normal)), 0.0) * (u_Diffuse * u_LightColor);

    vec3 viewDir = normalize(vec3(u_CameraPos.xyz - v_Position.xyz));
    vec3 reflection = normalize(reflect(-u_LightPos, v_Normal));
    vec3 specular = pow(max(dot(viewDir, reflection), 0.0), u_Shiny) * (u_Specular * u_LightColor);

    gl_FragColor = (vec4(ambient, 0.0) + vec4(diffuse.xyz, 0.0) + vec4(specular.xyz, 0.0)) * texture2D(u_Texture, v_UV);
}