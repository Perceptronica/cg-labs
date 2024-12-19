#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <sstream>
#include <sys/types.h>

const float moveSpeed = 2.0f;
const float scaleSpeed = 0.1f;
const float rotationSpeed = 2.0f;
const uint32_t horizontal_px = 1280;
const uint32_t vertical_px = 720;
const float X0 = static_cast<float>(horizontal_px / 2.0);
const float Y0 = static_cast<float>(vertical_px / 2.0);

int main() {
  sf::RenderWindow window(sf::VideoMode(horizontal_px, vertical_px), "SFML Circle (lab no.1)");
  sf::CircleShape circle(100.0f);
  circle.setFillColor(sf::Color::Blue);
  circle.setOrigin(circle.getRadius(), circle.getRadius());
  circle.setPosition(X0, Y0);

  sf::Font font;
  if (!font.loadFromFile("src/Arial.ttf")) {
    std::cerr << "Error! no font loaded!" << std::endl;
    return -1;
  }

  sf::Text positionText;
  positionText.setFont(font);
  positionText.setCharacterSize(20);
  positionText.setFillColor(sf::Color::White);
  positionText.setPosition(10.0f, 10.0f);

  sf::Text actionText;
  actionText.setFont(font);
  actionText.setCharacterSize(20);
  actionText.setFillColor(sf::Color::White);
  actionText.setPosition(10.0f, 30.0f);
  actionText.setString("-");

  sf::Vector2f position = circle.getPosition();
  std::vector<sf::Vector2f> path;
  bool displayPath{false};
  sf::CircleShape point(2.0f);
  point.setFillColor(sf::Color::White);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
        case sf::Keyboard::W:
          circle.move(0, -moveSpeed);
          actionText.setString("W");
          break;
        case sf::Keyboard::S:
          circle.move(0, moveSpeed);
          actionText.setString("S");
          break;
        case sf::Keyboard::A:
          circle.move(-moveSpeed, 0);
          actionText.setString("A");
          break;
        case sf::Keyboard::D:
          circle.move(moveSpeed, 0);
          actionText.setString("D");
          break;
        case sf::Keyboard::Q:
          circle.scale(1.0f + scaleSpeed, 1.0f + scaleSpeed);
          actionText.setString("Q");
          break;
        case sf::Keyboard::E:
          circle.scale(1.0f - scaleSpeed, 1.0f - scaleSpeed);
          actionText.setString("E");
          break;
        case sf::Keyboard::R:
          circle.rotate(rotationSpeed);
          actionText.setString("R");
          break;
        case sf::Keyboard::T:
          circle.rotate(-rotationSpeed);
          actionText.setString("T");
          break;
        case sf::Keyboard::Space:
          displayPath = (displayPath) ? false : true;
          if (!displayPath) {
            path.clear();
          }
          actionText.setString("Space");
          break;
        case sf::Keyboard::Escape:
          window.close();
          break;
        default:
          break;
        }
        position.x = static_cast<float>(circle.getPosition().x);
        position.y = static_cast<float>(circle.getPosition().y);
      } else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        position.x = static_cast<float>(mousePosition.x);
        position.y = static_cast<float>(mousePosition.y);
        actionText.setString("M");
        circle.setPosition(position);
      } else if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.delta > 0) {
          circle.scale(1.0f + scaleSpeed, 1.0f + scaleSpeed);
          actionText.setString("M+");
        } else if (event.mouseWheelScroll.delta < 0) {
          circle.scale(1.0f - scaleSpeed, 1.0f - scaleSpeed);
          actionText.setString("M-");
        }
      }
    }
    std::ostringstream posStream;
    posStream << "X: " << position.x << " Y: " << position.y;
    positionText.setString(posStream.str());
    if (displayPath) {
      path.push_back(circle.getPosition());
    }
    point.setPosition(position.x, position.y);

    window.clear(sf::Color::Black);
    window.draw(circle);
    for (std::size_t i = 0; i < path.size(); ++i) {
      point.setPosition(path[i].x, path[i].y);
      window.draw(point);
    }
    window.draw(positionText);
    window.draw(actionText);
    window.display();
  }
  return 0;
}
