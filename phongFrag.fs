uniform sampler2D u_Texture;
uniform float u_Threshold;
uniform vec3 u_Camera;

varying vec3 v_Position;
varying vec2 v_UV;
varying vec3 v_Normal;

void main()
{
	vec3 Material_Ambience = vec3(1,0.5,0.5);
	vec3 Material_Diffusivity = vec3(1,0.5,0.5);
	vec3 Material_Specularity = vec3(1,1,1);
	int Material_Shininess = 2;
	vec3 Light_Color = vec3(1,1,1);
	vec3 Light_Position = vec3(-10,10,10);

	vec4 sample = texture2D(u_Texture, v_UV);	
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * Light_Color * Material_Ambience;

	vec3 norm = normalize(v_Normal);
	vec3 lightDir = normalize(Light_Position - v_Position);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * Light_Color * Material_Diffusivity;
	
	vec3 viewDir = normalize(u_Camera - v_Position);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
	vec3 specular = Material_Shininess * spec * Light_Color * Material_Specularity;  

	vec3 result = (ambient + diffuse + specular) * vec3(sample);

    gl_FragColor = vec4(result, 1.0);

}
