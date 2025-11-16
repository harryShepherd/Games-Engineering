#include <SFML/Graphics.hpp>
#include <format>
#include <iostream>
#include <fstream>

const sf::Keyboard::Key controls[4] = {
  sf::Keyboard::A,    // Player1 UP
  sf::Keyboard::Z,    // Player1 DOWN
  sf::Keyboard::K,   // Player2 UP
  sf::Keyboard::M  // Player2 DOWN
};

// Parameters
const sf::Vector2f paddleSize(25.f, 100.f);
const float ballRadius = 10.f;
const int gameWidth = 800;
const int gameHeight = 600;
const int frameRate = 60;
const float paddleSpeed = 4.f;
const float paddleOffsetWall = 10.f;
const float timeStep = 1 / frameRate;

sf::Vector2f ballVelocity;
bool isPlayerServing = true;
const float initialVelocityX = -2.0f;
const float initialVelocityY = -1.0f;
const float velocityMultiplier = 1.1f;

// Objects
sf::CircleShape ball;
sf::RectangleShape paddles[2];

sf::Font myFont;
sf::Text scoreText;

void init() {
  std::ofstream file;
  file.open("Test.txt");
  file.close();

  // Load font
  myFont.loadFromFile("resources\\vcr_mono.ttf");

  scoreText.setFont(myFont);
  scoreText.setCharacterSize(12);
  scoreText.setFillColor(sf::Color::White);
  scoreText.setString("Wow this is epic");

  // Set size and origin of paddles
  for (sf::RectangleShape &p : paddles) {
    p.setSize(paddleSize);
    p.setOrigin(paddleSize / 2.f);
  }

  // Set size and origin of ball
  ball.setRadius(ballRadius);
  ball.setOrigin({ballRadius, ballRadius});

  // Reset paddle position
  paddles[0].setPosition(paddleOffsetWall + paddleSize.x / 2.f, gameHeight / 2.f);
  paddles[1].setPosition(gameWidth - paddleOffsetWall - paddleSize.x / 2.f, gameHeight / 2.f);

  // Reset ball position
  ball.setPosition({gameWidth / 2.f, gameHeight / 2.f}); 

  ballVelocity = { initialVelocityX, initialVelocityY };
}

void update(float dt, float fps)
{
  // Handle paddle movement
  float directionLeft = 0.0f;
  float directionRight = 0.0f;

  if (sf::Keyboard::isKeyPressed(controls[0]))
  {
    directionLeft = -1;
  }

  if (sf::Keyboard::isKeyPressed(controls[1]))
  {
    directionLeft = 1;
  }

    if (sf::Keyboard::isKeyPressed(controls[2]))
  {
    directionRight = -1;
  }

  if (sf::Keyboard::isKeyPressed(controls[3]))
  {
    directionRight = 1;
  }

  paddles[0].move(sf::Vector2f(0.f, directionLeft * paddleSpeed));
  paddles[1].move(sf::Vector2f(0.f, directionRight * paddleSpeed));

  ball.move(ballVelocity);
  
  const float bx = ball.getPosition().x;
  const float by = ball.getPosition().y;

  if(by > gameHeight) {
    ballVelocity.y *= -1;
  } else if (by < 0) {
    ballVelocity.y *= -1;
  } else if (bx > gameWidth) {
    init();
  } else if (bx < 0) {
    init();
  } else if (
    bx < paddleSize.x + paddleOffsetWall &&
    by > paddles[0].getPosition().y - (paddleSize.y * 0.5f) &&
    by < paddles[0].getPosition().y + (paddleSize.y * 0.5f)) {
      ballVelocity.x *= -1;
    } else if (
      bx > gameWidth - paddleSize.x - paddleOffsetWall &&
      by > paddles[1].getPosition().y - (paddleSize.y * 0.5f) &&
      by < paddles[1].getPosition().y + (paddleSize.y * 0.5f)) {
        ballVelocity.x *= -1;
      }
}

void render(sf::RenderWindow &window)
{
  // Draw Everything
  window.draw(paddles[0]);
  window.draw(paddles[1]);
  window.draw(ball);
  window.draw(scoreText);
}

int main()
{
  init();

  sf::RenderWindow window(sf::VideoMode({gameWidth, gameHeight}), "PONG");

  float t = 0.0f;
  float dt = 1.0f / 60.0f;

  sf::Clock clock;
  float lastTime = 0;

  while (window.isOpen()){
      sf::Event event;
      while (window.pollEvent(event)){
      if (event.type == sf::Event::Closed){
        window.close();
      }
    }
      
    float currentTime = clock.restart().asSeconds();
    float fps = 1.0f / (currentTime - lastTime);
    lastTime = currentTime;

    window.clear();
    update(t, fps);
    render(window);
    window.display();

    t += dt;

  }
  return 0;
}