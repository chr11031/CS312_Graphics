uniform sampler2D u_Texture;
uniform float u_Threshold;
uniform vec3 u_Camera;
uniform vec3 u_MaterialAmbient;
uniform vec3 u_MaterialDiffuse;
uniform vec3 u_MaterialSpecular;
uniform float u_MaterialShine;
uniform vec3 u_LightPos;
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f); //vec3(1.0f, 0.0f, 0.0f);

varying vec2 v_UV;
varying vec3 v_Normal;
varying vec3 v_FragPos;


void main()
{
    vec4 sample = texture2D(u_Texture, v_UV);
    if(sample.r > u_Threshold)
    {
        discard;
    }
    //ambient lighting
    vec3 ambient = u_MaterialAmbient * lightColor; // * vec3(sample);
  	
    // diffuse 
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_LightPos - v_FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff*u_MaterialDiffuse) * lightColor;
    
    // specular
    vec3 viewDir = normalize(u_Camera - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_MaterialShine);
    vec3 specular = u_MaterialSpecular * spec * lightColor;  
    
    gl_FragColor = vec4((ambient + diffuse + specular), 1.0) * sample;
}