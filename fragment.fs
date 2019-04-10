uniform sampler2D u_Texture;
uniform float u_Threshold;
uniform vec3 u_light;
uniform vec3 u_lightPos;
uniform vec3 u_amb;
uniform vec3 u_diff;
uniform vec3 u_spec;
uniform vec3 u_camera;
uniform float u_shiny;

varying vec2 v_UV;
varying vec3 v_Normal;
varying vec4 v_Pos;


void main()
{
    vec4 sample = texture2D(u_Texture, v_UV);
    vec3 ambient = u_amb * u_light;
    // vec3 result = ambient * sample;

    // float diff = max(dot(v_Normal, v_Pos.xyz), 0.0);
    // vec3 diffuse = diff * u_light;
    vec3 diffuse = max(dot(v_Normal, normalize(u_lightPos)), 0.0) * (u_diff * u_light);

    vec3 viewDir = normalize(u_camera.xyz - v_Pos.xyz);
    vec3 reflectDir = normalize(reflect(-u_lightPos, v_Normal)); 

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_shiny);
    vec3 specular = spec * (u_spec * u_light); 

    vec4 result = vec4((ambient + diffuse + specular), 1.0) * sample;

    gl_FragColor = result;

}