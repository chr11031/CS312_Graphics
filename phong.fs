uniform sampler2D u_Texture;

uniform vec3 u_Light_Color;
uniform vec3 u_Light_Position;
uniform vec3 u_Camera_Position;

uniform vec3 u_Material_Ambient;
uniform vec3 u_Material_Diffuse;
uniform vec3 u_Material_Specular;
uniform float u_Material_Shine;

varying vec2 v_UV;
varying vec3 v_Normal;
varying vec3 v_Position;

void main()
{
	vec4 sample = texture2D(u_Texture, v_UV);

    // Ambient Lighting
    vec3 ambient = u_Material_Ambient * u_Light_Color;

    // Diffuse Lighting
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_Light_Position - v_Position); 
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * (u_Light_Color * u_Material_Diffuse);

    // Specular Lighting
    vec3 viewDir = normalize(u_Camera_Position - v_Position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material_Shine);
    vec3 specular = spec * (u_Light_Color * u_Material_Specular);

    vec4 result = vec4(ambient + diffuse + specular, 1.0) * sample;
    gl_FragColor = result;
}
