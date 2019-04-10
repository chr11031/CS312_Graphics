uniform sampler2D u_Texture;
uniform float u_Threshold;
uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
uniform vec3 u_CameraPos;
uniform vec3 u_Ambience;
uniform vec3 u_Diffuse;
uniform vec3 u_Specular;
uniform float u_Shiny;

varying vec3 v_FragPos;
varying vec3 v_Normal;
varying vec2 v_UV;

void main()
{
    vec4 sample = texture2D(u_Texture, v_UV);

    if (sample.r > u_Threshold)
    {
        discard;
    }

    vec3 ambient = 0.1 * u_Ambience * u_LightColor;

    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_LightPos - v_FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor * u_Diffuse;

    vec3 viewDir = normalize(u_CameraPos - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shiny);
    vec3 specular = u_Specular * spec * u_LightColor;

    vec4 result = vec4(diffuse + ambient + specular, 1.0);

    gl_FragColor = sample * result;
}
