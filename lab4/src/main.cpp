#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cmath>
#include <fstream>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <vector>

std::string loadShaderSource(const std::string &filePath) {
  std::ifstream file(filePath);
  if (!file) {
    std::cerr << "Не удалось открыть шейдерный файл: " << filePath << std::endl;
    exit(EXIT_FAILURE);
  }
  std::stringstream ss;
  ss << file.rdbuf();
  return ss.str();
}

GLuint compileShader(GLenum type, const std::string &source) {
  GLuint shader = glCreateShader(type);
  const char *src_cstr = source.c_str();
  glShaderSource(shader, 1, &src_cstr, NULL);
  glCompileShader(shader);
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLint logLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> infoLog(logLength);
    glGetShaderInfoLog(shader, logLength, NULL, infoLog.data());
    std::cerr << "Ошибка компиляции шейдера: " << infoLog.data() << std::endl;
    exit(EXIT_FAILURE);
  }
  return shader;
}

GLuint createProgram(const std::string &vertexPath,
                     const std::string &fragmentPath) {

  std::string vertexCode = loadShaderSource(vertexPath);
  std::string fragmentCode = loadShaderSource(fragmentPath);

  GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode);
  GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

  GLuint program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  GLint success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    GLint logLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> infoLog(logLength);
    glGetProgramInfoLog(program, logLength, NULL, infoLog.data());
    std::cerr << "Ошибка линковки программы: " << infoLog.data() << std::endl;
    exit(EXIT_FAILURE);
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return program;
}

void generateSphere(float radius, unsigned int sectorCount,
                    unsigned int stackCount, std::vector<float> &vertices,
                    std::vector<unsigned int> &indices) {
  float x, y, z, xy;
  float nx, ny, nz, lengthInv = 1.0f / radius;
  float sectorStep = 2 * M_PI / sectorCount;
  float stackStep = M_PI / stackCount;
  float sectorAngle, stackAngle;

  // Vertices
  for (unsigned int i = 0; i <= stackCount; ++i) {
    stackAngle = M_PI / 2 - i * stackStep;
    xy = radius * cosf(stackAngle);
    z = radius * sinf(stackAngle);

    for (unsigned int j = 0; j <= sectorCount; ++j) {
      sectorAngle = j * sectorStep;

      x = xy * cosf(sectorAngle);
      y = xy * sinf(sectorAngle);
      vertices.push_back(x);
      vertices.push_back(y);
      vertices.push_back(z);

      nx = x * lengthInv;
      ny = y * lengthInv;
      nz = z * lengthInv;
      vertices.push_back(nx);
      vertices.push_back(ny);
      vertices.push_back(nz);
    }
  }

  unsigned int k1, k2;
  for (unsigned int i = 0; i < stackCount; ++i) {
    k1 = i * (sectorCount + 1);
    k2 = k1 + sectorCount + 1;

    for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
      if (i != 0) {
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k1 + 1);
      }

      if (i != (stackCount - 1)) {
        indices.push_back(k1 + 1);
        indices.push_back(k2);
        indices.push_back(k2 + 1);
      }
    }
  }
}

int main() {
  // Создание окна SFML с контекстом OpenGL
  sf::ContextSettings settings;
  settings.depthBits = 24;
  settings.stencilBits = 8;
  settings.antialiasingLevel = 4;
  settings.majorVersion = 3;
  settings.minorVersion = 3;
  settings.attributeFlags = sf::ContextSettings::Core;

  sf::Window window(sf::VideoMode(800, 600), "Sphere Shading",
                    sf::Style::Default, settings);
  window.setVerticalSyncEnabled(true);

  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    std::cerr << "Ошибка инициализации GLEW: " << glewGetErrorString(err)
              << std::endl;
    return -1;
  }
  glEnable(GL_DEPTH_TEST);

  std::vector<float> vertices;
  std::vector<unsigned int> indices;
  generateSphere(1.0f, 50, 50, vertices, indices);

  GLuint VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  GLuint gouraudProgram =
      createProgram("shaders/gouraud.vert", "shaders/gouraud.frag");
  GLuint flatProgram = createProgram("shaders/flat.vert", "shaders/flat.frag");
  GLuint currentProgram = gouraudProgram;

  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 projection =
      glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

  glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
  bool useGouraud = true;

  bool running = true;
  sf::Clock clock;
  float angle = 0.0f;

  while (running) {
    // Обработка событий
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        running = false;
      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
          useGouraud = !useGouraud;
          currentProgram = useGouraud ? gouraudProgram : flatProgram;
          std::cout << "Current shader: " << (useGouraud ? "Gouraud" : "Flat")
                    << std::endl;
        }
        if (event.key.code == sf::Keyboard::Escape) {
          window.close();
          running = false;
        }
      }
    }

    float deltaTime = clock.restart().asSeconds();
    angle += deltaTime * glm::radians(50.0f);
    model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(currentProgram);

    GLint modelLoc = glGetUniformLocation(currentProgram, "model");
    GLint viewLoc = glGetUniformLocation(currentProgram, "view");
    GLint projLoc = glGetUniformLocation(currentProgram, "projection");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

    GLint lightPosLoc = glGetUniformLocation(currentProgram, "lightPos");
    glUniform3fv(lightPosLoc, 1, &lightPos[0]);

    glm::vec3 viewPos(0.0f, 0.0f, 3.0f);
    GLint viewPosLoc = glGetUniformLocation(currentProgram, "viewPos");
    glUniform3fv(viewPosLoc, 1, &viewPos[0]);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    window.display();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(gouraudProgram);
  glDeleteProgram(flatProgram);

  window.close();
  return 0;
}
