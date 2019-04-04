uniform sampler2D u_Texture;
uniform float u_Threshold;

uniform vec3 lightPosition = {0, 100, 0};
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
    // Get object color at position
    vec4 sample = texture2D(u_Texture, v_UV);

    // Ambient calculation
    vec3 ambient = u_Ka * lightColor;
    
    // Diffuse calculation
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(lightPosition - frag_pos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * u_Kd;

    // Specular calculation
    vec3 viewDir = normalize(u_Cam - frag_pos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Ns);
    vec3 specular = u_Ks * spec * lightColor;  

    // Add the lighting together for Phong effect
    vec4 result = vec4((ambient + diffuse + specular), 1.0) * sample;

    // Assign the calculated color to the fragment
    gl_FragColor = result;
}