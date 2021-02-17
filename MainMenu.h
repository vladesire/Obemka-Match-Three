#ifndef MAIN_MENU_H_
#define MAIN_MENU_H_

#include <SFML/Graphics.hpp>
#include <thread>
#include <chrono>
#include "ExitAsk.h"
#include "ResLoader.h"

#include <iostream>


class MainMenu
{
public:
	MainMenu(sf::RenderWindow &window_, ResLoader &resloader_): 
		window{window_}, resloader{resloader_}, fonts{resloader_.get_fonts()}, soundtrack{resloader_.get_music()}, userdata{resloader_.get_userdata()}
	{}

	int run()
	{
		if (firsttime)
			firsttime = false;
		else
			soundtrack->setPlayingOffset(sf::milliseconds(16500));

		soundtrack->setVolume(userdata->get_music_volume());
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
						{
							settings(); break;
						}
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

	void settings()
	{
		sf::Sound sound; // To feel difference when changing settings
		sound.setBuffer(resloader.get_soundbuffers()[3]);

		sf::Text texts[8];

		for (size_t i = 0; i < 8; ++i)
		{
			texts[i].setFont(fonts[0]); // Regular
			texts[i].setFillColor(sf::Color(80,80,80));
		}

		texts[0].setString("Настройки");
		texts[0].setCharacterSize(40);
		texts[0].setFont(fonts[1]); // Bold
		texts[0].setPosition(20, 15);

		texts[1].setString("Сохранить");
		texts[1].setFont(fonts[1]);
		texts[1].setPosition(20, 170);

		texts[2].setString("Звуки");
		texts[2].setPosition(90, 70);

		texts[3].setString("Музыка");
		texts[3].setPosition(90, 110);

		texts[4].setString("-");
		texts[4].setPosition(220, 70);

		texts[5].setString("+");
		texts[5].setPosition(440, 70);

		texts[6].setString("-");
		texts[6].setPosition(220, 110);

		texts[7].setString("+");
		texts[7].setPosition(440, 110);

		for (size_t i = 0; i < 8; ++i)
		{
			texts[i].move(5, -10); // Fix library's bug
		}

		sf::RectangleShape rect1, rect2;

		rect1.setFillColor(sf::Color(40,40,40));
		rect2.setFillColor(sf::Color(40,40,40));

		rect1.setPosition(240, 70);
		rect2.setPosition(240, 110);

		rect1.setSize(sf::Vector2f(2*userdata->get_sounds_volume(), 20));
		rect2.setSize(sf::Vector2f(2*userdata->get_music_volume(), 20));

		std::chrono::system_clock::time_point start_time;
		std::chrono::system_clock::duration delta_time;

		while (true)
		{
			window.clear();

			for (size_t i = 0; i < 8; ++i)
			{
				window.draw(texts[i]);
			}

			window.draw(rect1);
			window.draw(rect2);

			window.display();

			sf::Event event;

			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
				{
					resloader.write_userdata();
					return;
				}
				else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased)
				{
					auto pos = sf::Mouse::getPosition(window);

					int id = settings_button(pos.x, pos.y);

					if (event.type == sf::Event::MouseButtonReleased)
					{
						if (id == 0)
						{
							resloader.write_userdata();
							return;
						}

					}
					else
					{
						if (id == 1 || id == 2)
						{
							if (id == 1)
							{
								userdata->sub_sounds();
							}
							else 
							{
								userdata->add_sounds();
							}

							sound.setVolume(userdata->get_sounds_volume());
							sound.play();
						}
						else if (id == 3)
						{
							userdata->sub_music();
						}
						else if (id == 4)
						{
							userdata->add_music();
						}				
					}

					if (id != -1)
					{
						soundtrack->setVolume(userdata->get_music_volume());
						rect1.setSize(sf::Vector2f(2 * userdata->get_sounds_volume(), 20));
						rect2.setSize(sf::Vector2f(2 * userdata->get_music_volume(), 20));
					}

				}
			}
		}

		delta_time = std::chrono::system_clock::now() - start_time;
		std::this_thread::sleep_for(std::chrono::microseconds(8333 - std::chrono::duration_cast<std::chrono::microseconds>(delta_time).count()));
	}

private:
	sf::RenderWindow &window;
	ResLoader &resloader;
	ResLoader::UserData *userdata;
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

	int settings_button(int x, int y)
	{
		if (x > 20 && x < 180 && y > 170 && y < 195)
		{
			return 0;
		}
		else if (x > 220 && x < 240 && y > 70 && y < 90)
		{
			return 1;
		}
		else if (x > 440 && x < 460 && y > 70 && y < 90)
		{
			return 2;
		}
		else if (x > 220 && x < 240 && y > 110 && y < 130)
		{
			return 3;
		}
		else if (x > 440 && x < 460 && y > 110 && y < 130)
		{
			return 4;
		}
		else
		{
			return -1;
		}
	}

};

#endif // !MAIN_MENU_H_
