#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 vertexColor;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);

    // Calculate lighting (Gouraud Shading - per vertex)
    vec3 norm = normalize(mat3(transpose(inverse(model))) * normal);
    vec3 lightDir = normalize(lightPos - vec3(model * vec4(position, 1.0)));
    float diff = max(dot(norm, lightDir), 0.0);

    // Simple diffuse lighting
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);

    vertexColor = diffuse;
}
