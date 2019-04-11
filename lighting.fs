uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D u_Texture;

varying vec2 v_UV;
varying vec3 FragPos;
varying vec3 Normal;

void main()
{
    // ambient
    float ambientStrength = 0.2;
    vec4 sample = texture2D(u_Texture, v_UV);	

    vec3 ambient = lightColor * ambientStrength;

    // diffuse 
    float diffStrength = 0.64;
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffStrength * diff * lightColor;

    // specular 
    float specularStrength = 0.9;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;  


    gl_FragColor = sample * vec4(ambient + diffuse + specular,1.0);
}