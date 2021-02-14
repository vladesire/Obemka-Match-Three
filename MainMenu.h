#ifndef MAIN_MENU_H_
#define MAIN_MENU_H_

#include <SFML/Graphics.hpp>
#include <thread>
#include <chrono>
#include "ExitAsk.h"

#include <iostream>


class MainMenu
{
public:
	MainMenu(sf::RenderWindow &window_, const sf::Font *fonts_, sf::Music *music): window{window_}, fonts{fonts_}, soundtrack{music + 1}
	{}

	int run()
	{
		if (firsttime)
			firsttime = false;
		else
			soundtrack->setPlayingOffset(sf::milliseconds(16500));

		soundtrack->play();

		sf::Text texts[5];

		for (size_t i = 0; i < 5; i++)
		{
			texts[i].setFont(fonts[0]);
			texts[i].setCharacterSize(35);
			texts[i].setFillColor(sf::Color(80, 80, 80));
			texts[i].setPosition(txtposx, txtposy + txtygap * i);
		}

		texts[0].setString("Новая игра");
		texts[1].setString("Игра на время");
		texts[2].setString("Статистика");
		texts[3].setString("Настройки");
		texts[4].setString("Выход");


		std::chrono::system_clock::time_point start_time;
		std::chrono::system_clock::duration delta_time;

		int hovered_id = -1;

		while (true)
		{
			start_time = std::chrono::system_clock::now();

			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
				{
					if (exit_question(window, fonts))
						return 0;
					// TODO: Ask before closing
				}
				else if (event.type == sf::Event::MouseButtonReleased)
				{
					int id = button_id(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);

					switch (id)
					{
						case 0:
							std::cout << "Game!\n"; break;
						case 1:
						{
							soundtrack->stop();

							return 1;
						}
						case 2:
							std::cout << "Score\n"; break;
						case 3:
							std::cout << "Settings\n"; break;
						case 4:
						{
							if (exit_question(window, fonts))
								return 0;
						}
					}
				}

			}

			// Highlight effect
			
			int but_id = button_id(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
					
			if (but_id != -1 && hovered_id == -1)
			{
				hovered_id = but_id;
				texts[but_id].setFillColor(sf::Color(120,120,120));
			}
			else if (but_id == -1 && hovered_id != -1)
			{
				texts[hovered_id].setFillColor(sf::Color(80, 80, 80));
				hovered_id = -1;
			}
			

			window.clear();

			for (size_t i = 0; i < 5; i++)
			{
				window.draw(texts[i]);
			}

			window.display();

			delta_time = std::chrono::system_clock::now() - start_time;
			std::this_thread::sleep_for(std::chrono::microseconds(8333 - std::chrono::duration_cast<std::chrono::microseconds>(delta_time).count()));
		}

		return 1;
	}

private:
	sf::RenderWindow &window;
	const sf::Font *fonts;
	sf::Music *soundtrack;

	bool firsttime = true; // to handle music offset

	const int txtposx = 15, txtposy = 150, txtygap = 50;
	
	int button_id(int x, int y)
	{
		int oy = txtposy + 10; // for conveniece. I don't know why Text automatically adds 10 pixels to y.
		int ox = txtposx;
		float bs = 50.0; // button size (with padding)

		if (!(y < oy || y > oy+5*bs || x < ox))
		{
			float id = (y - oy) / bs;
			
			if (id - trunc(id) < 0.6f)
			{
				id = trunc(id);

				// check lengths of buttons
				if (id == 0 && x < 180 + ox || id == 1 && x < 235 + ox || id == 2 && x < 185 + ox || id == 3 && x < 175 + ox || id == 4 && x < 105 + ox)
				{
					return id;
				}
			}

		}
		 
		return -1;
	}

};

#endif // !MAIN_MENU_H_
