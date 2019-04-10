uniform sampler2D u_Texture;
uniform float u_Threshold;

// hard coding light
uniform vec3 light = {0, 100, -50};
uniform vec3 lightColor = {1.0, 1.0, 1.0};

// inserted uniforms
uniform float u_Ns;
uniform vec3 u_Ka;
uniform vec3 u_Kd;
uniform vec3 u_Ks;
uniform vec3 u_Camera;

varying vec3 v_Normal;
varying vec2 v_UV;
varying vec3 fragPos;

void main()
{
    vec4 sample = texture2D(u_Texture, v_UV);

    // if (sample.r > u_Threshold)
    // {
    //     discard;
    // }
    
    // ambient lighting
    vec3 ambient = u_Ka * lightColor;

    // diffuse lighting
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(light - fragPos);
    vec3 diffuse = max(dot(norm, lightDir), 0.0) * u_Kd * lightColor;

    // specular lighting
    vec3 viewDir = normalize(u_Camera - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Ns);
    vec3 specular = u_Ks * spec * lightColor;

    // adding them all together
    vec4 total = vec4((diffuse + ambient + specular), 1.0) * sample;

    gl_FragColor = total;
} 