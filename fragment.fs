uniform sampler2D u_Texture;

varying vec2 v_UV;


// Normals, fragment posiiton, view position
vec3 lightPos = vec3(1.0, 1.0, 1.0); 
vec3 lightColor = vec3(1.0, 1.0, 1.0);
vec3 objColor = vec3(1.0, 0.5, 0.5);


uniform vec3 viewPos; 
uniform vec3 strengths;

// Varying
varying vec3 normal;
varying vec3 fragPos;

void main()
{	
    // Ambient lighting
    float ambientStrength = 0.2;
    vec3 ambient = vec3(lightColor * ambientStrength);

    // Diffuse lighting
    float diffuseStrength = 0.64;
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    // specular
    float specularStrength = 0.9;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;  

    // Assign the lighting
    gl_FragColor = vec4(objColor * (ambient + diffuse + specular), 1.0);    
}