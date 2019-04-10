uniform sampler2D u_Texture;
uniform float u_Threshold;

uniform vec3 u_Camera;

uniform vec3 u_Ambient;
uniform vec3 u_Diffuse;
uniform vec3 u_Specular;

uniform vec3 u_LightColor;
uniform vec3 u_LightPosition;

varying vec3 v_Position;
varying vec2 v_UV;
varying vec3 v_Normal;

void main()
{
    vec4 sample = texture2D(u_Texture, v_UV);

    vec3 ambient = u_Ambient * u_LightColor;
    
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_LightPosition - v_Position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * (u_LightColor * u_Diffuse);

    vec3 viewDir = normalize(u_Camera - v_Position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2.0);
    vec3 specular = u_Specular * spec * u_LightColor;

    gl_FragColor = vec4(ambient + diffuse + specular, 1.0) * sample;
}