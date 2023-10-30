#version 330 core
in vec3 normal;
in vec2 texCoord;
in vec3 position;
out vec4 fragColor;

uniform vec3 viewPos;
uniform bool m_texture;

struct Light { // point light
    vec3 position;
    vec3 direction;
    vec2 cutoff;
    vec3 attenuation;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

struct Material {
    sampler2D TexDiffuse;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

void main() {
  vec3 texColor = m_texture ? texture(material.TexDiffuse, texCoord).xyz : material.diffuse;
  vec3 ambient = m_texture ? texColor * light.ambient : light.ambient * material.ambient;

  vec3 lightDir = normalize(light.position - position);
  vec3 pixelNorm = normalize(normal);
  float diff = max(dot(pixelNorm, lightDir), 0.0);
  vec3 diffuse = diff * texColor * light.diffuse;

  // vec3 specColor = texture(material.specular, texCoord).xyz;
  vec3 viewDir = normalize(viewPos - position);
  vec3 reflectDir = reflect(-lightDir, pixelNorm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = spec * material.specular * light.specular;

  vec3 result = ambient + diffuse + specular;
  fragColor = vec4(result, 1.0);
}


// void main() {
//   vec3 ambient = material.ambient * light.ambient;

//   vec3 lightDir = normalize(light.position - position);
//   vec3 pixelNorm = normalize(normal);
//   float diff = max(dot(pixelNorm, lightDir), 0.0);
//   vec3 diffuse = diff * material.diffuse * light.diffuse;

//   vec3 viewDir = normalize(viewPos - position);
//   vec3 reflectDir = reflect(-lightDir, pixelNorm);
//   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//   vec3 specular = spec * material.specular * light.specular;

//   vec3 result = ambient + diffuse + specular;
//   fragColor = vec4(result, 1.0);
// }

// void main() {
//   //ambient
//   vec3 ambient = material.ambient * light.ambient;

//   float dist = length(light.position - position);
//   vec3 distPoly = vec3(1.0, dist, dist*dist);
//   float attenuation = 1.0 / dot(distPoly, light.attenuation);
//   vec3 lightDir = normalize(light.position - position);

//   //##spot light
//   vec3 result = ambient;
//   float theta = dot(lightDir, normalize(-light.direction));
//   float intensity = clamp((theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]), 0.0, 1.0);

//   if (intensity > 0.0) {
//     vec3 pixelNorm = normalize(normal);
//     float diff = max(dot(pixelNorm, lightDir), 0.0);
//     vec3 diffuse = diff * material.diffuse * light.diffuse;

//     vec3 viewDir = normalize(viewPos - position);
//     vec3 reflectDir = reflect(-lightDir, pixelNorm);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//     vec3 specular = spec * material.specular * light.specular;
    
//     result += (diffuse + specular) * intensity;
//   }

//   result *= attenuation;
//   fragColor = vec4(result, 1.0);
// }

// void main() {
//   //ambient
//   // vec3 ambient = ambientStrength * lightColor;
//   vec3 texColor = texture(material.diffuse, texCoord).xyz;
//   vec3 ambient = texColor * light.ambient;
  
//   //diffuse
//   // vec3 lightDir = normalize(lightPos - position);
//   // vec3 pixelNorm = normalize(normal);
//   // vec3 diffuse = max(dot(pixelNorm, lightDir), 0.0) * lightColor;
//   // vec3 lightDir = normalize(-light.direction); // directional light
//   // vec3 lightDir = normalize(light.position - position); // point light

//   float dist = length(light.position - position);
//   vec3 distPoly = vec3(1.0, dist, dist*dist);
//   float attenuation = 1.0 / dot(distPoly, light.attenuation);
//   vec3 lightDir = (light.position - position) / dist;

//   //##spot light
//   vec3 result = ambient;
//   float theta = dot(lightDir, normalize(-light.direction));
//   float intensity = clamp((theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]), 0.0, 1.0);

//   if (intensity > 0.0) {
//     vec3 pixelNorm = normalize(normal);
//     float diff = max(dot(pixelNorm, lightDir), 0.0);
//     vec3 diffuse = diff * texColor * light.diffuse;

//     vec3 specColor = texture(material.specular, texCoord).xyz;
//     vec3 viewDir = normalize(viewPos - position);
//     vec3 reflectDir = reflect(-lightDir, pixelNorm);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//     vec3 specular = spec * specColor * light.specular;

//     result += (diffuse + specular) * intensity;
//   }

//   result *= attenuation;
  
//   //## point light
//   // vec3 pixelNorm = normalize(normal);
//   // float diff = max(dot(pixelNorm, lightDir), 0.0);
//   // vec3 diffuse = diff * texColor * light.diffuse;
  
//   // //specular
//   // // vec3 viewDir = normalize(viewPos - position);
//   // // vec3 reflectDir = reflect(-lightDir, pixelNorm);
//   // // float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularShininess);
//   // // vec3 specular = specularStrength * spec * lightColor;
//   // vec3 specColor = texture(material.specular, texCoord).xyz;
//   // vec3 viewDir = normalize(viewPos - position);
//   // vec3 reflectDir = reflect(-lightDir, pixelNorm);
//   // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//   // vec3 specular = spec * specColor * light.specular;
//   // //result
//   // // vec3 result = (ambient + diffuse + specular) * objectColor;
//   // vec3 result = (ambient + diffuse + specular) * attenuation;

//   fragColor = vec4(result, 1.0);
// }