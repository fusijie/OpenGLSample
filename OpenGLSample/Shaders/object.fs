#version 150 core

struct Material{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
struct DirLight{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct PointLight{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};
struct SpotLight{
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 finalColor;

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[4];
uniform SpotLight spotLight;

vec3 CalcDirLight(DirLight light, vec3 normalDir, vec3 viewDir)
{
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normalDir, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    
    vec3 reflectDir = reflect(-lightDir, normalDir);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normalDir, vec3 fragPos, vec3 viewDir)
{
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normalDir, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    
    vec3 reflectDir = reflect(-lightDir, normalDir);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f/(light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normalDir, vec3 fragPos, vec3 viewDir)
{
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normalDir, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    
    vec3 reflectDir = reflect(-lightDir, normalDir);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
   
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f/(light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = (light.cutOff -light.outerCutOff);
    float intensity = clamp((theta-light.outerCutOff)/epsilon, 0.0, 1.0);
    diffuse *= intensity;
    specular *= intensity;
    
    return (ambient + diffuse + specular);
}

void main() {
    vec3 normalDir = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = CalcDirLight(dirLight, normalDir, viewDir);
    for(int i=0; i<4; i++)
        result += CalcPointLight(pointLights[i], normalDir, FragPos, viewDir);
    result += CalcSpotLight(spotLight, normalDir, FragPos, viewDir);
    
    finalColor = vec4(result, 1.0);
}