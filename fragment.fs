uniform sampler2D u_Texture;
uniform float u_Threshold;

uniform vec3 light = {0, 100, -50};
uniform vec3 lightColor = {1.0, 1.0, 1.0};
uniform vec3 u_Cam;

uniform float u_Ns;
uniform vec3 u_Ka;
uniform vec3 u_Kd;
uniform vec3 u_Ks;

varying vec3 v_Normal;
varying vec2 v_UV;
varying vec3 frag_pos;

void main()
{
    vec4 sample = texture2D(u_Texture, v_UV);

    // ambient lighting
    vec3 ambient = u_Ka * lightColor;
    
    // diffuse lighting
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(light - frag_pos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * u_Kd;

    // specular lighting
    vec3 viewDir = normalize(u_Cam - frag_pos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Ns);
    vec3 specular = u_Ks * spec * lightColor;  

    // the total phong shading
    vec4 total = vec4((ambient + diffuse + specular), 1.0) * sample;
    gl_FragColor = total;
}