#version 150 core

struct Material{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
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

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 finalColor;

uniform vec3 viewPos;
uniform Material material;
uniform PointLight pointLights[2];

vec3 CalcPointLight(PointLight light, vec3 normalDir, vec3 fragPos, vec3 viewDir)
{
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
    
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normalDir, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    
    vec3 reflectDir = reflect(-lightDir, normalDir);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f/(light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}

void main()
{
    vec3 normalDir = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0.0, 0.0, 0.0);
    for(int i=0; i<2; i++)
        result += CalcPointLight(pointLights[i], normalDir, FragPos, viewDir);
    
    finalColor = vec4(result, 1.0);
}