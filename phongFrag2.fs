uniform sampler2D u_Texture;
uniform vec3 ambientStrength;
uniform vec3 diffuseStrength;
uniform vec4 specularStrength;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec4 lightColor;

varying vec2 v_UV;
varying vec3 v_Normal;
varying vec3 v_Position;

void main()
{
    float ambientStrength = ambientStrength[0];
    float diffuseStrength = diffuseStrength[0];
    float specularStrength = specularStrength[0];



    // Diffuse
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(lightPos - v_Position); 
    float diff = max(dot(norm, lightDir), 0.0); 
    vec4 diffuse = diff * lightColor * diffuseStrength;

    // Ambient
    vec4 ambient = ambientStrength * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - v_Position);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec4 specular = specularStrength * spec * lightColor;

    // Combine diffuse ambient and specular lighting
    vec4 sample = (diffuse + ambient + specular) * texture2D(u_Texture, v_UV);

    gl_FragColor = sample;
}