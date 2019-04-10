uniform sampler2D u_Texture;
uniform float u_Threshold;
uniform vec3 u_Camera;
uniform vec3 u_Ambient;
uniform vec3 u_Diffuse;
uniform vec3 u_Specular;
uniform float u_Shininess;

varying vec3 v_Position;
varying vec2 v_UV;
varying vec3 v_Normal;

void main()
{
	vec3 Light_Color = vec3(1,1,1);
	vec3 Light_Position = vec3(-10,10,10);

	vec4 sample = texture2D(u_Texture, v_UV);	
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * Light_Color * u_Ambient;

	vec3 norm = normalize(v_Normal);
	vec3 lightDir = normalize(Light_Position - v_Position);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * Light_Color * u_Diffuse;
	
	vec3 viewDir = normalize(u_Camera - v_Position);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
	vec3 specular = spec * Light_Color * u_Specular;  

	vec3 result = (ambient + diffuse + specular) * vec3(sample);

    gl_FragColor = vec4(result, 1.0);

}
