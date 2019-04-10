uniform sampler2D u_Texture;
uniform float u_Threshold;
uniform vec3 u_Camera;

//Material For Ambient, Diffuse, Specular, and Shine
uniform vec3 u_MaterialAmbient;
uniform vec3 u_MaterialDiffuse;
uniform vec3 u_MaterialSpecular;
uniform vec3 u_MaterialShine;

varying vec3 v_Position;
varying vec2 v_UV;
varying vec3 v_Normal;

void main()
{

	//Lighting
	vec3 lightColor = vec3(1, 1, 1);
	vec3 lightPos = vec3(10, 10, 10);

	vec4 sample = texture2D(u_Texture, v_UV);

	//ambient lighting
    vec3 ambient = u_MaterialAmbient * lightColor; // * vec3(sample);
    
    // diffuse 
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(lightPos - v_Position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * u_MaterialDiffuse) * lightColor;
    
    // specular
    vec3 viewDir = normalize(u_Camera - v_Position);
    vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = u_MaterialSpecular * spec * lightColor;  
    
	//Fragment Shader
    gl_FragColor = vec4((ambient + diffuse + specular), 1.0) * sample;

}