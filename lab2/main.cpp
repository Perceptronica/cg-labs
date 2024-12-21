#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
};

void createCube(std::vector<Vertex> &vertices, std::vector<GLuint> &indices) {
  glm::vec3 positions[] = {
      {-1.0f, -1.0f, 1.0f},  // 0
      {1.0f, -1.0f, 1.0f},   // 1
      {1.0f, 1.0f, 1.0f},    // 2
      {-1.0f, 1.0f, 1.0f},   // 3
      {-1.0f, -1.0f, -1.0f}, // 4
      {1.0f, -1.0f, -1.0f},  // 5
      {1.0f, 1.0f, -1.0f},   // 6
      {-1.0f, 1.0f, -1.0f},  // 7
  };

  glm::vec3 normals[] = {
      {0.0f, 0.0f, 1.0f},  // Передняя грань
      {0.0f, 0.0f, -1.0f}, // Задняя грань
      {-1.0f, 0.0f, 0.0f}, // Левая грань
      {1.0f, 0.0f, 0.0f},  // Правая грань
      {0.0f, 1.0f, 0.0f},  // Верхняя грань
      {0.0f, -1.0f, 0.0f}, // Нижняя грань
  };

  vertices.push_back({positions[0], normals[0]});
  vertices.push_back({positions[1], normals[0]});
  vertices.push_back({positions[2], normals[0]});
  vertices.push_back({positions[3], normals[0]});
  vertices.push_back({positions[5], normals[1]});
  vertices.push_back({positions[4], normals[1]});
  vertices.push_back({positions[7], normals[1]});
  vertices.push_back({positions[6], normals[1]});
  vertices.push_back({positions[4], normals[2]});
  vertices.push_back({positions[0], normals[2]});
  vertices.push_back({positions[3], normals[2]});
  vertices.push_back({positions[7], normals[2]});
  vertices.push_back({positions[1], normals[3]});
  vertices.push_back({positions[5], normals[3]});
  vertices.push_back({positions[6], normals[3]});
  vertices.push_back({positions[2], normals[3]});
  vertices.push_back({positions[3], normals[4]});
  vertices.push_back({positions[2], normals[4]});
  vertices.push_back({positions[6], normals[4]});
  vertices.push_back({positions[7], normals[4]});
  vertices.push_back({positions[4], normals[5]});
  vertices.push_back({positions[5], normals[5]});
  vertices.push_back({positions[1], normals[5]});
  vertices.push_back({positions[0], normals[5]});

  for (GLuint i = 0; i < 6; ++i) {
    GLuint offset = i * 4;
    indices.push_back(offset + 0);
    indices.push_back(offset + 1);
    indices.push_back(offset + 2);
    indices.push_back(offset + 2);
    indices.push_back(offset + 3);
    indices.push_back(offset + 0);
  }
}

int main() {
  sf::Window window(sf::VideoMode(800, 600), "Cube with Normals",
                    sf::Style::Default, sf::ContextSettings(24));
  window.setVerticalSyncEnabled(true);

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    std::cerr << "Не удалось инициализировать GLEW" << std::endl;
    return -1;
  }

  glEnable(GL_DEPTH_TEST);

  GLuint shaderProgram = glCreateProgram();
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

  const GLchar *vertexShaderSource = R"(
    #version 330 core

    layout(location = 0) in vec3 position;
    layout(location = 1) in vec3 normal;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    out vec3 FragPos;
    out vec3 Normal;

    void main()
    {

    FragPos = vec3(model * vec4(position, 1.0));
    Normal = mat3(transpose(inverse(model))) * normal;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "Ошибка компиляции вершинного шейдера:\n"
              << infoLog << std::endl;
  }

  const GLchar *fragmentShaderSource = R"(
#version 330 core

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 color;

void main()
{
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(1.0);

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0);

    vec3 result = (ambient + diffuse + specular);
    color = vec4(result, 1.0);
}
)";

  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "Ошибка компиляции фрагментного шейдера:\n"
              << infoLog << std::endl;
  }

  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "Ошибка линковки шейдерной программы:\n"
              << infoLog << std::endl;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  createCube(vertices, indices);

  GLuint cubeVAO, cubeVBO, cubeEBO;
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &cubeVBO);
  glGenBuffers(1, &cubeEBO);

  glBindVertexArray(cubeVAO);

  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0],
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
               &indices[0], GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid *)offsetof(Vertex, position));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid *)offsetof(Vertex, normal));
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);

  GLuint normalVAO, normalVBO;
  glGenVertexArrays(1, &normalVAO);
  glGenBuffers(1, &normalVBO);

  std::vector<glm::vec3> normalLines;
  for (const auto &vertex : vertices) {
    normalLines.push_back(vertex.position);
    normalLines.push_back(vertex.position + vertex.normal * 0.5f);
  }

  glBindVertexArray(normalVAO);

  glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
  glBufferData(GL_ARRAY_BUFFER, normalLines.size() * sizeof(glm::vec3),
               &normalLines[0], GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                        (GLvoid *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  sf::Clock clock;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape)
          window.close();
      }
    }

    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float time = clock.getElapsedTime().asSeconds();

    glm::mat4 model = glm::rotate(glm::mat4(1.0f), time * glm::radians(50.0f),
                                  glm::vec3(0.5f, 1.0f, 0.0f));

    glm::vec3 viewPos = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::mat4 view = glm::lookAt(viewPos, glm::vec3(0.0f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    glUseProgram(shaderProgram);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    glUniform3f(lightPosLoc, 1.2f, 1.0f, 2.0f);

    GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));

    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glUseProgram(0);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(projection));
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(view * model));

    glBindVertexArray(normalVAO);
    glColor3f(1.0f, 0.0f, 0.0f);
    glDrawArrays(GL_LINES, 0, normalLines.size());
    glBindVertexArray(0);

    window.display();
  }

  glDeleteProgram(shaderProgram);
  glDeleteBuffers(1, &cubeVBO);
  glDeleteBuffers(1, &cubeEBO);
  glDeleteVertexArrays(1, &cubeVAO);

  glDeleteBuffers(1, &normalVBO);
  glDeleteVertexArrays(1, &normalVAO);

  return 0;
}
