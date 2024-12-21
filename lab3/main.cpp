#include <GL/glew.h>
#include <GL/glu.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <iostream>

bool initOpenGL() {
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    std::cerr << "Ошибка инициализации GLEW: " << glewGetErrorString(err)
              << std::endl;
    return false;
  }
  if (!GLEW_VERSION_2_0) {
    return false;
  }
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glShadeModel(GL_SMOOTH);
  return true;
}

void setupPerspective(int width, int height) {
  if (height == 0)
    height = 1;
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  float aspect = static_cast<float>(width) / static_cast<float>(height);
  gluPerspective(45.0f, aspect, 1.0f, 500.0f);
  glMatrixMode(GL_MODELVIEW);
}

void drawCube() {
  glBegin(GL_QUADS);
  // Передняя граница (z = 1.0f)
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex3f(-1.0f, -1.0f, 1.0f);
  glVertex3f(1.0f, -1.0f, 1.0f);
  glVertex3f(1.0f, 1.0f, 1.0f);
  glVertex3f(-1.0f, 1.0f, 1.0f);
  // Задняя граница (z = -1.0f)
  glColor3f(0.0f, 1.0f, 0.0f);
  glVertex3f(-1.0f, -1.0f, -1.0f);
  glVertex3f(-1.0f, 1.0f, -1.0f);
  glVertex3f(1.0f, 1.0f, -1.0f);
  glVertex3f(1.0f, -1.0f, -1.0f);
  // Верхняя граница (y = 1.0f)
  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex3f(-1.0f, 1.0f, -1.0f);
  glVertex3f(-1.0f, 1.0f, 1.0f);
  glVertex3f(1.0f, 1.0f, 1.0f);
  glVertex3f(1.0f, 1.0f, -1.0f);
  // Нижняя граница (y = -1.0f)
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex3f(-1.0f, -1.0f, -1.0f);
  glVertex3f(1.0f, -1.0f, -1.0f);
  glVertex3f(1.0f, -1.0f, 1.0f);
  glVertex3f(-1.0f, -1.0f, 1.0f);
  // Правая граница (x = 1.0f)
  glColor3f(0.0f, 1.0f, 0.0f);
  glVertex3f(1.0f, -1.0f, -1.0f);
  glVertex3f(1.0f, 1.0f, -1.0f);
  glVertex3f(1.0f, 1.0f, 1.0f);
  glVertex3f(1.0f, -1.0f, 1.0f);
  // Левая граница (x = -1.0f)
  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex3f(-1.0f, -1.0f, -1.0f);
  glVertex3f(-1.0f, -1.0f, 1.0f);
  glVertex3f(-1.0f, 1.0f, 1.0f);
  glVertex3f(-1.0f, 1.0f, -1.0f);
  glEnd();
}

void handleResize(int width, int height) { setupPerspective(width, height); }

int main() {
  sf::ContextSettings settings;
  settings.depthBits = 24;
  settings.stencilBits = 8;
  settings.antialiasingLevel = 4;
  settings.majorVersion = 2;
  settings.minorVersion = 1;
  sf::Window window(sf::VideoMode(800, 600), "Вращающийся куб с камерой",
                    sf::Style::Default, settings);
  window.setActive(true);
  if (!initOpenGL()) {
    return -1;
  }

  setupPerspective(800, 600);
  float rotationX = 0.0f;
  float rotationY = 0.0f;
  float rotationZ = 0.0f;
  float rotationSpeedX = 50.0f;
  float rotationSpeedY = 50.0f;
  float rotationSpeedZ = 50.0f;
  float cameraX = 0.0f;
  float cameraY = 0.0f;
  float cameraZ = 5.0f;

  sf::Clock clock;

  bool running = true;
  while (running) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        running = false;
      if (event.type == sf::Event::KeyPressed &&
          event.key.code == sf::Keyboard::Escape)
        running = false;
      if (event.type == sf::Event::Resized) {
        handleResize(event.size.width, event.size.height);
      }
    }
    float deltaTime = clock.restart().asSeconds();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
      rotationY -= rotationSpeedY * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
      rotationY += rotationSpeedY * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
      rotationX -= rotationSpeedX * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
      rotationX += rotationSpeedX * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
      rotationZ -= rotationSpeedZ * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
      rotationZ += rotationSpeedZ * deltaTime;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
      rotationSpeedX += 10.0f * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
      rotationSpeedX -= 10.0f * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
      rotationSpeedY -= 10.0f * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
      rotationSpeedY += 10.0f * deltaTime;

    float cameraSpeed = 5.0f * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
      cameraZ -= cameraSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
      cameraZ += cameraSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::J))
      cameraX -= cameraSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
      cameraX += cameraSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::U))
      cameraY += cameraSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
      cameraY -= cameraSpeed;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glRotatef(rotationX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotationY, 0.0f, 1.0f, 0.0f);
    glRotatef(rotationZ, 0.0f, 0.0f, 1.0f);
    drawCube();
    window.display();
  }

  window.close();
  return 0;
}
