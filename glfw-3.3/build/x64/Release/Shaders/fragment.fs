#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	float strength;
};

in vec3 FragPos;  
in vec3 Normal;  
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform DirLight dirLight;

vec3 getDifuse(vec3 fragPos)
{
	return material.diffuse;
}
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = light.direction;
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.01);
    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
	//vec3 halfwayDir = normalize(lightDir + viewDir);  
	vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //float spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
    // attenuation
    
    vec3 ambient = light.ambient * material.ambient*light.strength;
    vec3 diffuse = light.diffuse * diff * getDifuse(fragPos)*light.strength;

    vec3 result = (ambient+diffuse);
	return result;
} 
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.1);
    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
	//vec3 halfwayDir = normalize(lightDir + viewDir);  
	vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //float spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = max(1.0 / (light.constant + light.linear * distance + light.quadratic * (distance)), 0.3f);    
    // combine results
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * diff * getDifuse(fragPos);
    vec3 specular = light.specular * spec * material.specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation*0.6;
    vec3 result = (diffuse + specular);
	return result;
}
void main()
{  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
        
    //vec3 result = max(CalcDirLight(dirLight, norm, FragPos, viewDir), 0) + 
    //max(CalcPointLight(light, norm, FragPos,viewDir),0);
    float cc = max(abs(dot(viewDir,norm)), 0.3f);
    vec3 res = vec3(cc);
    FragColor = vec4(res, 1.0);
} 
