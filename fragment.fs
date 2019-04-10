uniform sampler2D u_Texture;

uniform vec3 u_LightPosition;
uniform vec3 u_LightColor;
uniform vec3 u_LightNorm;
uniform vec3 u_Ka;
uniform vec3 u_Kd;
uniform vec3 u_Ks;
uniform vec3 u_Camera;

uniform float u_Threshold;

varying vec2 v_UV;
varying vec3 Normal;
varying vec3 FragPos;

void main()
{
    vec4 sample = texture2D(u_Texture, v_UV);
    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(u_LightPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor * u_Kd;

    // Ambient lighting
    vec3 ambient = u_Ka * u_LightColor;

    // Specular Lighting
    vec3 viewDirection = normalize(u_Camera - FragPos);
    vec3 reflectDirection = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32.0);
    vec3 specular = u_Ks * spec * u_LightColor;

    gl_FragColor = vec4(ambient + diffuse + specular, 1.0) * sample ;
}