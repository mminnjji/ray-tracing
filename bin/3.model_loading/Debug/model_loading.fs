#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;  
in vec3 FragPos;  

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform vec3 ObjColor;
uniform bool hasTextures;
//radial
uniform float constant;
uniform float linear;
uniform float quadratic;
//angular
uniform vec3 lightDirection;
uniform float cutOff;
uniform float outerCutOff;
uniform bool angularAttenuationEnabled;

void main()
{    
    vec3 fColor = hasTextures ? texture(texture_diffuse1, TexCoords).rgb : ObjColor;

    // ambient
    const float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    const float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32); //32
    vec3 specular = specularStrength * spec * lightColor;  

    //angular attenuation

    vec3 lightDirec = normalize(-lightDirection); // 조명방향
    float theta = dot(lightDir, lightDirec); //각도
    float epsilon = cutOff - outerCutOff; // 경계값 차이
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0); //0 ~ 1로 클램핑

    if (angularAttenuationEnabled) 
    {
        diffuse *= intensity;
        specular *= intensity;
    }

    //radial attenuation
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
        
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = (ambient + diffuse + specular) * fColor;
    FragColor = vec4(result, 1.0);
}