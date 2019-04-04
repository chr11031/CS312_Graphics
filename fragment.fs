// Texture and threshold
uniform sampler2D u_Texture;
uniform float u_Threshold;

// Light's information
uniform vec3 u_LightPos;
uniform vec3 u_LightColor;

// Phong shading information + Shininess
uniform vec3 u_Ambience;
uniform vec3 u_Diffuse;
uniform vec3 u_Specular;

// Cam information
uniform vec3 u_CameraPos;

// Vector UV, Position, and Normal
varying vec2 v_UV;
varying vec3 Normal;
varying vec3 FragPos;

// Does the fragment shader step
void main()
{
    vec4 sample = texture2D(u_Texture, v_UV);

    // The ambient calculations
    vec4 ambient = vec4(u_Ambience * u_LightColor, 1.0);

    // Dealing with the normals and the light directtion
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(u_LightPos - FragPos);

    // The diffuse calculations
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = vec4(diff * u_LightColor * u_Diffuse, 1.0);

    // View direction and the 'reflection' direction
    vec3 viewDir = normalize(u_CameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    // The Specular calculations
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec4 specular = vec4(u_Specular * spec * u_LightColor, 1.0);

    //Put it all together
    gl_FragColor = (ambient + diffuse + specular) * sample;
}
