uniform vec3 lightPos = {0.0, 100.0, 100.0};
uniform vec3 lightColor = {1.0, 1.0, 1.0};
uniform vec3 u_Cam;
uniform vec3 u_Ambient;
uniform vec3 u_Diffuse;
uniform vec3 u_Specular;
uniform float u_Shiny;

uniform sampler2D u_Texture;

varying vec3 Normal;
varying vec3 FragPos;

varying vec2 v_UV;

void main()
{
    // ambient
    vec3 ambient = u_Ambient * lightColor;

    vec4 sample = texture2D(u_Texture, v_UV);
    // normalize vectors
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_Diffuse * lightColor;

    // spectral
    vec3 viewDir = normalize(u_Cam - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shiny);
    vec3 specular = spec * u_Specular * lightColor;

    // lighting calcs
    gl_FragColor = vec4(u_Ambient + diffuse + specular, 1.0) * sample;
} 