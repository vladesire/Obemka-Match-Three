#ifndef MAIN_MENU_H_
#define MAIN_MENU_H_

#include <SFML/Graphics.hpp>
#include <thread>
#include <chrono>

class MainMenu
{
public:
	MainMenu(sf::RenderWindow &window_, const sf::Font *fonts_): window{window_}, fonts{fonts_}
	{}

	int run()
	{
		return 1;
	}

	void splash()
	{
		sf::Text text;
		text.setFont(*fonts);
		text.setFillColor(sf::Color::White);
		text.setString("Vladesire Games\n\n\n\t\tPowered by SFML");
		text.setPosition(200, 300);
		text.setCharacterSize(50);

		window.clear();
		window.draw(text);
		window.display();

		std::this_thread::sleep_for(std::chrono::seconds(5));

	}

private:
	sf::RenderWindow &window;
	const sf::Font *fonts;
};

#endif // !MAIN_MENU_H_
