#version 330 core

flat in vec3 FragNormal;
flat in vec3 FragPos;
out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec3 norm = normalize(FragNormal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);

    FragColor = vec4(diffuse, 1.0);
}
