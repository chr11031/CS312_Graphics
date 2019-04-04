#version 330 

uniform sampler2D u_Texture;
uniform float u_Threshold;

//color of the light
uniform vec3 lightColor = {1.0, 1.0, 1.0};

//light coordinates
uniform vec3 lightPos = {0, 100, 100};

uniform vec3 u_Ambient;
uniform vec3 u_Diffuse;
uniform vec3 u_Specular;

varying vec2 v_UV;
varying vec4 v_objPosition;
varying vec3 v_Norm;
varying vec3 fragPos;

void main()
{
    vec4 sample = texture2D(u_Texture, v_UV);	

    //AMBIENT
    vec3 ambientFinal = u_Ambient * lightColor;

    //DIFFUSE
    vec3 norm = normalize(v_Norm);
    //normalizes the position of the light direction between the light source and the fragment's position
    vec3 posToLightDirVec = normalize(lightPos - fragPos);
    //Calculating the diffuse impact that the light has on the fragment
    float diffuse = max(dot(norm, posToLightDirVec), 0.0);
    vec3 diffuseFinal = diffuse * lightColor;

    //SPECULAR
    vec3 viewDir = normalize(-fragPos);
    //reflection direction
    vec3 reflectDir = reflect(-posToLightDirVec, norm);
    //Calculating the specular component
    //dot product of the reflection direction and the view direction
    //the 32 value is how concentrated the shine is
    float specular = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specularFinal = 0.5 * specular * lightColor;

    //COLORING
    gl_FragColor = vec4(ambientFinal + diffuseFinal + specularFinal, 1.0) * sample;

}
//For the disappearing teapot
//vec4 sample = texture2D(u_Texture, v_UV);	
//if(sample.r > u_Threshold)
//{
//    discard;
//}
//
//gl_FragColor = sample;