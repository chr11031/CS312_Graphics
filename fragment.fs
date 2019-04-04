uniform sampler2D u_Texture;
uniform float u_Threshold;
//uniform 
uniform vec3 viewPos;
uniform vec3 u_MaterialAmbient;
uniform vec3 u_MaterialDiffuse;
uniform vec3 u_MaterialSpecular;
uniform float u_MaterialShine;

varying vec2 v_UV;
in vec3 Normal;
in vec3 FragPos;

void main()
{
    //Initializations
    vec4 sample = texture2D(u_Texture, v_UV);
    vec3 lightColor = {1, 1, 1};
    vec3 lightPos = {1, 1, 9.5};
    float ambientStr = 0.2;
    float specularStr = 1;

    //Ambient
    vec3 ambient = ambientStr * lightColor * u_MaterialAmbient;

    //Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * u_MaterialDiffuse;

    //Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_MaterialShine);
    vec3 specular = specularStr * spec * lightColor * u_MaterialSpecular;

    vec3 result = (ambient + diffuse + specular);

    gl_FragColor = vec4(result, 1.0) * sample;
}