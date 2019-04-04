uniform sampler2D u_Texture;

uniform vec3 lightPosition = {0.0, 10.0, 0.0};
uniform vec3 lightColor = {1.0,1.0,1.0};
uniform vec3 u_Cam;
uniform vec3 ambientAmount;
uniform vec3 diffuseAmount;
uniform vec3 specularAmount;
uniform float u_Shiny;

varying vec3 v_Normal;
varying vec2 v_UV;
varying vec3 FragPos;

void main()
{
    vec4 sample = texture2D(u_Texture, v_UV);
    vec3 ambient = ambientAmount * lightColor;

    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(lightPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * diffuseAmount * lightColor;

    vec3 viewDir = normalize(u_Cam - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shiny * 2);
    vec3 specular = spec * specularAmount * lightColor;

    gl_FragColor = vec4(ambient + diffuse + specular, 1.0) * sample;
}