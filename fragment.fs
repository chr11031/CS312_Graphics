uniform sampler2D u_Texture;
uniform float u_Threshold;

varying vec2 v_UV;
varying vec3 v_fragPos;
varying vec3 v_normal;

uniform vec3 u_lightPos;
uniform vec3 u_viewPos;
uniform vec3 u_lightColor;
uniform vec3 u_objectColor;

uniform vec3 u_ambient;
uniform vec3 u_diffuse;
uniform vec3 u_specular;

//Most lighting calculations take place in the Vertex Shader.

void main()
{
	//This is for texture mapped
	// vec4 sample = texture2D(u_Texture, v_UV);	
	// if(sample.r > u_Threshold)
	// {
	// 	discard;
	// }
	

	//ambient light calculations
	vec3 ambient = u_ambient * u_lightColor;
	vec4 calcAmbient = vec4(ambient, 1.0);

	// diffuse lighting calculations
	vec3 norm = normalize(v_normal);
	vec3 lightDir = normalize(u_lightPos - v_fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * (u_lightColor * u_diffuse);
	vec4 calcDiffuse = vec4(diffuse, 1.0);

	//Speculative lighting
	//float specularStrength = 0.5;
	vec3 viewDir = normalize(u_viewPos - v_fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = u_specular * spec * u_lightColor;
	vec4 calcSpecular = vec4(specular, 1.0);

	//vec4 sample = texture2D(u_Texture, v_UV);
	vec3 sample = u_objectColor;
    vec3 result = (ambient + diffuse + specular) * sample;

	//Add the lighting together diffuse and ambient
	//vec4 result = (calcAmbient + calcDiffuse + calcSpecular) * sample;

	gl_FragColor = vec4(result, 1.0);
	//gl_FragColor = sample; //Old code
	//gl_FragColor = result;

}
