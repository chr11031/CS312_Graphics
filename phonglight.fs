#version 330

uniform sampler2D u_Texture;
uniform float u_Threshold;
uniform vec3 u_LightColor = {1, 1, 1};
uniform vec3 u_LightPos = {0, 200, 100};
uniform vec3 u_CameraPos;
uniform vec3 u_Ambient;
uniform vec3 u_Diffuse;
uniform vec3 u_Specular;
uniform float u_Shiny;

varying vec3 FragPos;

varying vec4 v_Position;
varying vec2 v_UV;
varying vec3 Normal;

void main()
{
    //take sample
    vec4 sample = texture2D(u_Texture, v_UV);	

    //ambient lighting
    vec3 ambient = u_Ambient * u_LightColor;

    
    //diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(u_LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor;


    //specular lighting
    vec3 specularStrength = u_Specular;
    vec3 viewDir = normalize(u_CameraPos-FragPos); 
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    vec3 specular = specularStrength * vec3(spec) * u_LightColor; 
    
    gl_FragColor = vec4(ambient + diffuse + specular, 1.0) * sample;
} 