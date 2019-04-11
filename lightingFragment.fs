#version 140

varying vec3 v_Normal;  
varying vec3 v_FragPos;  

uniform sampler2D u_Texture;
uniform float u_Threshold;

varying vec2 v_UV;

uniform vec3 u_lightPos;
uniform vec3 u_lightColor;
uniform vec3 u_viewPos;

uniform vec3 u_ambient;
uniform vec3 u_diffuse;
uniform vec3 u_specular;

void main()
{   
    vec3 cAmbient = u_ambient * u_lightColor;
    vec4 calcAmbient = vec4(cAmbient, 1.0);

    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_lightPos - v_FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 cDiffuse = diff * (u_diffuse * u_lightColor);
    vec4 calcDiffuse = vec4(cDiffuse, 1.0);

    vec3 viewDir = normalize(u_viewPos - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 cSpecular = u_specular * spec * u_lightColor; 
    vec4 calcSpecular = vec4(cSpecular, 1.0);

    vec4 sample = texture2D(u_Texture, v_UV);
    // vec3 sample = vec3(1.0, 0.0, 0.0);
    // vec3 result = (cAmbient + cDiffuse + cSpecular) * sample;
    vec4 result = (calcAmbient + calcDiffuse + calcSpecular) * sample;

    // gl_FragColor = vec4(result, 1.0);
    gl_FragColor = result;
} 