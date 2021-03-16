#include <SFML/Graphics.hpp>
#include "WindowConstants.h"
#include "Splash.h"
#include <thread>
#include <chrono>

void splash(sf::RenderWindow &window)
{
	window.setMouseCursorVisible(false);

	sf::Font font;
	font.loadFromFile("Fonts/BRUSHSCI.TTF");

	sf::Text text, powered;

	text.setFont(font);
	powered.setFont(font);

	text.setFillColor(sf::Color::White);
	powered.setFillColor(sf::Color(40,40,40));

	text.setString("Vladesire Games");
	powered.setString("Powered by SFML");

	text.setCharacterSize(80);
	powered.setCharacterSize(30);
	
	text.setPosition(WINDOW_W/2 - text.getGlobalBounds().width/2, WINDOW_H/ 2 - text.getGlobalBounds().height / 2 - 50);
	powered.setPosition(500, WINDOW_H / 2 + 200);

	window.clear();
	window.draw(text);
	window.draw(powered);
	window.display();
	
	std::this_thread::sleep_for(std::chrono::seconds(5)); 
}