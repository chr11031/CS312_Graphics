uniform sampler2D u_Texture;
uniform float u_Threshold;
uniform vec3 u_Light;
uniform vec3 u_LightPosition;
uniform vec3 u_Ambient;
uniform vec3 u_Diffuse;
uniform vec3 u_Specular;
uniform vec3 u_Camera;
uniform float u_Strength;

varying vec2 v_UV;
varying vec3 v_Normal;
varying vec4 v_Position;

void main() 
{
    vec4 sample = texture2D(u_Texture, v_UV);    
    vec3 ambient = u_Ambient * u_Light;

    vec3 diffuse = max(dot(v_Normal, normalize(u_LightPosition)), 0.0) * (u_Diffuse * u_Light);

    vec3 viewDirection = normalize(u_Camera.xyz - v_Position.xyz);
    vec3 reflectDirection = normalize(reflect(-u_LightPosition, v_Normal));

    float specularFloat = pow(max(dot(viewDirection, reflectDirection), 0.0), u_Strength);
    vec3 specular = specularFloat * (u_Specular * u_Light);

    vec4 result = vec4((ambient + diffuse +specular), 1.0) * sample; 

    gl_FragColor = result;
}