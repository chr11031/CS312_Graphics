uniform sampler2D u_Texture;
uniform float u_Threshold;

varying vec2 v_UV;

uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
uniform vec3 u_CameraPos;
uniform vec3 u_Ambience;
uniform vec3 u_Diffuse;
uniform vec3 u_Specular;
uniform float u_Shiny;
varying vec3 v_FragPos;
varying vec3 v_Normal;

void main()
{
    vec4 sample = texture2D(u_Texture, v_UV);	

    if (sample.r > u_Threshold)
    {
        discard;
    }
    // Ambient Lighting
    vec3 ambient = 0.1 * u_Ambience * u_LightColor;
	vec3 normal = normalize(v_Normal);
	vec3 lightDirection = normalize(u_LightPos - v_FragPos);
	
	float diff = max(dot(normal, lightDirection), 0.0);
	// Diffuse Lighting
    vec3 diffuse = diff * u_LightColor * u_Diffuse;
    // Specular Lighting
	vec3 viewDir = normalize(u_CameraPosition - v_FragPos);
	vec3 reflectDir = reflect(-lightDirection, normal);
	
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shiny);
	
    vec3 specular = u_Specular * spec * u_LightColor;
	vec4 final = vec4(diffuse + ambient + specular, 1.0);
    
    gl_FragColor = sample * finalsa;
}