#include "ExitAsk.h"

bool exit_question(sf::RenderWindow &window, const sf::Font *fonts)
{
	sf::Texture txt;
	txt.create(window.getSize().x, window.getSize().y);
	txt.update(window);

	sf::Sprite background;
	background.setTexture(txt);

	sf::RectangleShape rect;
	rect.setFillColor(sf::Color::White);
	rect.setSize(sf::Vector2f(300, 200));
	rect.setPosition(static_cast<sf::Vector2f>(window.getSize() / 2u) - rect.getSize() / 2.0f);


	sf::Text texts[3];

	for (size_t i = 0; i < 3; i++)
	{
		texts[i].setFillColor(sf::Color::Black);
		texts[i].setFont(fonts[0]);
	}

	texts[0].setString("Выйти?");
	texts[1].setString("Да");
	texts[2].setString("Нет");

	texts[0].setPosition(rect.getPosition() + sf::Vector2f(rect.getSize().x / 2 - 50, rect.getSize().y / 3));
	texts[1].setPosition(rect.getPosition() + sf::Vector2f(rect.getSize().x / 4 - 20, rect.getSize().y / 3 * 2));
	texts[2].setPosition(rect.getPosition() + sf::Vector2f(rect.getSize().x / 4 * 3 - 25, rect.getSize().y / 3 * 2));


	while (true)
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::MouseButtonReleased)
			{
				auto pos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)) - rect.getPosition();
				return (pos.x > 50 && pos.x < 95 && pos.y > 140 && pos.y < 170);
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
					return false;
				else if (event.key.code == sf::Keyboard::Enter)
					return true;
			}
		}

		window.clear();
		window.draw(background);
		window.draw(rect);

		for (size_t i = 0; i < 3; i++)
		{
			window.draw(texts[i]);
		}

		window.display();
	}

}
