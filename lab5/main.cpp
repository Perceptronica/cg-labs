// main.cpp
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;
};

struct Sphere {
  glm::vec3 center;
  float radius;
  glm::vec3 color;

  bool intersect(const Ray &ray, float &t) const {
    glm::vec3 oc = ray.origin - center;
    float a = glm::dot(ray.direction, ray.direction);
    float b = 2.0f * glm::dot(oc, ray.direction);
    float c = glm::dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
      return false;
    else {
      t = (-b - sqrt(discriminant)) / (2.0f * a);
      return t > 0;
    }
  }
};

struct Plane {
  glm::vec3 point;
  glm::vec3 normal;
  glm::vec3 color;
  bool intersect(const Ray &ray, float &t) const {
    float denom = glm::dot(normal, ray.direction);
    if (fabs(denom) > 1e-6) { // Проверка на перпендикулярность
      t = glm::dot(point - ray.origin, normal) / denom;
      return t >= 0;
    }
    return false;
  }
};

struct Light {
  glm::vec3 position;
  glm::vec3 color;
};

struct PhongParameters {
  glm::vec3 ambient;  // Ka
  glm::vec3 diffuse;  // Kd
  glm::vec3 specular; // Ks
  float shininess;
};

glm::vec3 phongShading(const glm::vec3 &point, const glm::vec3 &normal,
                       const glm::vec3 &viewDir, const PhongParameters &params,
                       const Light &light, const glm::vec3 &objectColor) {
  glm::vec3 lightDir = glm::normalize(light.position - point);
  glm::vec3 ambient = params.ambient * objectColor;
  float diff = std::max(glm::dot(normal, lightDir), 0.0f);
  glm::vec3 diffuse = params.diffuse * diff * objectColor;
  glm::vec3 reflectDir = glm::reflect(-lightDir, normal);
  float spec =
      pow(std::max(glm::dot(viewDir, reflectDir), 0.0f), params.shininess);
  glm::vec3 specular = params.specular * spec * light.color;
  return ambient + diffuse + specular;
}

int main() {
  const unsigned int width = 800;
  const unsigned int height = 600;
  sf::RenderWindow window(sf::VideoMode(width, height),
                          "Ray Tracing with Phong Shading");
  window.setFramerateLimit(30);

  window.setActive(true);
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    std::cerr << "Ошибка инициализации GLEW" << std::endl;
    return -1;
  }

  std::vector<sf::Uint8> pixels(width * height * 4);
  sf::Texture texture;
  texture.create(width, height);
  sf::Sprite sprite(texture);

  Sphere sphere = {glm::vec3(0.0f, 0.0f, -5.0f), 1.5f,
                   glm::vec3(0.1f, 0.6f, 0.8f)};
  Plane plane = {glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                 glm::vec3(0.8f, 0.8f, 0.8f)};
  Light light = {glm::vec3(-2.0f, 5.0f, 0.0f), glm::vec3(1.0f)};
  glm::vec3 cameraPos(0.0f, 0.0f, 0.0f);

  PhongParameters phongParams = {
      glm::vec3(0.1f), // ambient
      glm::vec3(1.0f), // diffuse
      glm::vec3(1.0f), // specular
      32.0f            // shininess
  };
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape)
          window.close();
        if (event.key.code == sf::Keyboard::Num1)
          phongParams.ambient += glm::vec3(0.1f);
        else if (event.key.code == sf::Keyboard::Num2)
          phongParams.ambient -= glm::vec3(0.1f);
        if (event.key.code == sf::Keyboard::Num3)
          phongParams.diffuse += glm::vec3(0.1f);
        else if (event.key.code == sf::Keyboard::Num4)
          phongParams.diffuse -= glm::vec3(0.1f);
        if (event.key.code == sf::Keyboard::Num5)
          phongParams.specular += glm::vec3(0.1f);
        else if (event.key.code == sf::Keyboard::Num6)
          phongParams.specular -= glm::vec3(0.1f);
        if (event.key.code == sf::Keyboard::Num7)
          phongParams.shininess += 8.0f;
        else if (event.key.code == sf::Keyboard::Num8)
          phongParams.shininess -= 8.0f;
        phongParams.ambient =
            glm::clamp(phongParams.ambient, glm::vec3(0.0f), glm::vec3(1.0f));
        phongParams.diffuse =
            glm::clamp(phongParams.diffuse, glm::vec3(0.0f), glm::vec3(1.0f));
        phongParams.specular =
            glm::clamp(phongParams.specular, glm::vec3(0.0f), glm::vec3(1.0f));
        phongParams.shininess = glm::max(phongParams.shininess, 1.0f);

        std::cout << "Ambient: " << phongParams.ambient.x << ", "
                  << "Diffuse: " << phongParams.diffuse.x << ", "
                  << "Specular: " << phongParams.specular.x << ", "
                  << "Shininess: " << phongParams.shininess << std::endl;
      }
    }

    for (unsigned int y = 0; y < height; ++y) {
      for (unsigned int x = 0; x < width; ++x) {
        // Нормализованные координаты устройства (NDC)
        float ndcX = (2.0f * x) / float(width) - 1.0f;
        float ndcY = 1.0f - (2.0f * y) / float(height);
        float aspectRatio = float(width) / float(height);
        float screenX = ndcX * aspectRatio;
        float screenY = ndcY;
        Ray ray;
        ray.origin = cameraPos;
        ray.direction = glm::normalize(glm::vec3(screenX, screenY, -1.0f));
        glm::vec3 pixelColor(0.0f);
        float t = 0.0f;

        if (sphere.intersect(ray, t)) {
          glm::vec3 point = ray.origin + t * ray.direction;
          glm::vec3 normal = glm::normalize(point - sphere.center);
          glm::vec3 viewDir = -ray.direction;

          pixelColor = phongShading(point, normal, viewDir, phongParams, light,
                                    sphere.color);
        } else if (plane.intersect(ray, t)) {
          glm::vec3 point = ray.origin + t * ray.direction;
          glm::vec3 normal = plane.normal;
          glm::vec3 viewDir = -ray.direction;

          pixelColor = phongShading(point, normal, viewDir, phongParams, light,
                                    plane.color);

        } else {
          pixelColor = glm::vec3(0.0f);
        }
        int index = (x + y * width) * 4;
        pixels[index + 0] = glm::clamp(pixelColor.r, 0.0f, 1.0f) * 255;
        pixels[index + 1] = glm::clamp(pixelColor.g, 0.0f, 1.0f) * 255;
        pixels[index + 2] = glm::clamp(pixelColor.b, 0.0f, 1.0f) * 255;
        pixels[index + 3] = 255;
      }
    }
    texture.update(&pixels[0]);
    window.clear();
    window.draw(sprite);
    window.display();
  }

  return 0;
}
