#include  <SFML/Graphics.hpp>
#include <iostream>

using namespace std;

enum class GameState { Playing, RoundOver, GameOver };

struct Game
{
    Game() : player1(sf::Vector2f(10.0f, 80.0f)), player2(sf::Vector2f(10.0f, 80.0f)), ball(8.0f), ballDirection(1.0f, 1.0f), score1(0), score2(0), ballSpeed(200.0f), ballSpeedOffset(20.0f), state(GameState::Playing) {
        player1.setPosition(sf::Vector2f(0.0f, 300.0f));
        player2.setPosition(sf::Vector2f(790.0f, 300.0f));
        ball.setPosition(sf::Vector2f(400.0f, 300.0f));
    }

    sf::RectangleShape player1;
    sf::RectangleShape player2;
    sf::CircleShape ball;
    sf::Vector2f ballDirection;
    int score1;
    int score2;
    float ballSpeed;
    float ballSpeedOffset;
    GameState state;
};

void boundPlayerPosition(sf::FloatRect windowBounds, sf::RectangleShape& player) {
    auto playerPosition = player.getPosition();
    playerPosition.y = std::max(playerPosition.y, windowBounds.top + 2.0f);
    playerPosition.y = std::min(playerPosition.y, windowBounds.top + windowBounds.height - player.getSize().y - 5.0f);
    player.setPosition(playerPosition);
}

void handleBallCollision(Game& game, sf::FloatRect windowBounds, sf::Clock& speedTimer) {
    if (game.player1.getGlobalBounds().intersects(game.ball.getGlobalBounds()) ||
        game.player2.getGlobalBounds().intersects(game.ball.getGlobalBounds())) {
        game.ballDirection.x *= -1;
        if (speedTimer.getElapsedTime().asSeconds() > 5.0f) {
            game.ballSpeed += game.ballSpeedOffset;
            speedTimer.restart();
        }
    }

    auto position = game.ball.getPosition();
    if (position.y - game.ball.getRadius() < windowBounds.top ||
        position.y + game.ball.getRadius() > windowBounds.top + windowBounds.height) {
        game.ballDirection.y *= -1;
        if (speedTimer.getElapsedTime().asSeconds() > 5.0f) {
            game.ballSpeed += game.ballSpeedOffset;
            speedTimer.restart();
        }
    }

    else if (position.x - game.ball.getRadius() < windowBounds.left) {
        game.ballDirection.x *= -1;
        game.score2++;
        game.state = GameState::RoundOver;
    }
    else if (position.x + game.ball.getRadius() > windowBounds.left + windowBounds.width) {
        game.ballDirection.x *= -1;
        game.score1++;
        game.state = GameState::RoundOver;
    }
}

void update(Game& game, sf::FloatRect windowBounds, sf::Time elapsedTime, sf::Clock& speedTimer) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        game.player1.move(0.0f, -10.0f * elapsedTime.asSeconds() * 100);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        game.player1.move(0.0f, 10.0f * elapsedTime.asSeconds() * 100);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        game.player2.move(0.0f, -10.0f * elapsedTime.asSeconds() * 100);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        game.player2.move(0.0f, 10.0f * elapsedTime.asSeconds() * 100);
    }

    game.ball.move(game.ballDirection * (elapsedTime.asSeconds() * game.ballSpeed));

    boundPlayerPosition(windowBounds, game.player1);
    boundPlayerPosition(windowBounds, game.player2);

    handleBallCollision(game, windowBounds, speedTimer);
}

void render(sf::RenderWindow& window, Game& game) {
    window.clear();
    sf::Vertex divider[] = {
        sf::Vertex(sf::Vector2f(400.0f,0.0f)),
        sf::Vertex(sf::Vector2f(400.0f, 600.0f))
    };

    sf::Font font;
    if (!font.loadFromFile("assets/fonts/BalooBhai-Regular.ttf"))
    {
        std::cout << "Error loading font" << std::endl;
    }
    sf::Text score1text;
    score1text.setString(std::to_string(game.score1));
    score1text.setFont(font);
    score1text.setCharacterSize(50);
    score1text.setFillColor(sf::Color::White);
    score1text.setStyle(sf::Text::Bold);
    score1text.setPosition(sf::Vector2f(200.0f, 20.0f));
    sf::Text score2text;
    score2text.setString(std::to_string(game.score2));
    score2text.setFont(font);
    score2text.setCharacterSize(50);
    score2text.setFillColor(sf::Color::White);
    score2text.setStyle(sf::Text::Bold);
    score2text.setPosition(sf::Vector2f(600.0f, 20.0f));

    window.draw(score1text);
    window.draw(score2text);
    window.draw(game.player1);
    window.draw(game.player2);
    window.draw(game.ball);
    window.draw(divider, 2, sf::Lines);

    window.display();
}

void resetRoundState(Game& game) {
    game.ballSpeed = 200.0f;
    game.player1.setPosition(sf::Vector2f(0.0f, 300.0f));
    game.player2.setPosition(sf::Vector2f(790.0f, 300.0f));
    game.ball.setPosition(sf::Vector2f(400.0f, 300.0f));
    game.state = GameState::Playing;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Classic Pong Game");

    Game game;
    sf::Clock clock, speedTimer;
    while (window.isOpen()) {
        if (game.state == GameState::RoundOver)resetRoundState(game);

        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            default:
                break;
            }
        }

        sf::Time elapsedTime = clock.restart();
        sf::FloatRect windowBounds({ 0.f, 0.f }, window.getDefaultView().getSize());
        update(game, windowBounds, elapsedTime, speedTimer);
        render(window, game);
    }
    return 0;
}
