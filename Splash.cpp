#include "Splash.h"
#include <SFML/Graphics.hpp>
#include <thread>
#include <chrono>


void splash(sf::RenderWindow &window)
{
	window.setMouseCursorVisible(false);


	sf::Font font;
	font.loadFromFile("Fonts/RobotoRegular.ttf");

	sf::Text text, powered;

	text.setFont(font);
	powered.setFont(font);

	text.setFillColor(sf::Color::White);
	powered.setFillColor(sf::Color(40,40,40));

	text.setString("Vladesire Games");
	powered.setString("Powered by SFML");

	text.setPosition(200, window.getSize().y / 2);
	powered.setPosition(500, window.getSize().y / 2 + 200);

	text.setCharacterSize(50);
	powered.setCharacterSize(25);

	window.clear();
	window.draw(text);
	window.draw(powered);
	window.display();

	std::this_thread::sleep_for(std::chrono::seconds(5)); //TODO
}