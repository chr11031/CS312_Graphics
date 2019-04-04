uniform sampler2D u_Texture;
uniform float u_Threshold;
uniform vec3 u_LightPosition;
uniform vec3 u_LightColor;
uniform vec3 u_LightNorm;

// Ambience strength
uniform vec3 u_Ka;

// Diffuse strength
uniform vec3 u_Kd;

// Specular strength
uniform vec3 u_Ks;
uniform vec3 u_Camera;

varying vec2 v_UV;
varying vec3 Normal;
varying vec3 FragPos;

void main()
{
    vec4 sample = texture2D(u_Texture, v_UV);
    if(sample.r > u_Threshold)
    {
        discard;
    }

    // Diffuse light calculations
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(u_LightPosition - FragPos);
    float diff = max(dot(norm, lightDirection),0.0);
    vec3 diffuse = diff * u_LightColor * u_Kd;
    
    // Ambient light calculations
    vec3 ambient = u_Ka * u_LightColor;

    // Specular light calculations
    vec3 viewDirection = normalize(u_Camera - FragPos);
    vec3 reflectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reflectDirection),0.0),32.0);
    vec3 specular = u_Ks * spec * u_LightColor;

    gl_FragColor = vec4(ambient + diffuse + specular,1.0) * sample ;

    // Leftover TEA CUP code
    // gl_FragColor = sample;
}