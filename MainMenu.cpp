#include "MainMenu.h"

template <int T>
int button_id(sf::Vector2i mouse_pos, const std::array<sf::IntRect, T> buttons)
{
	int counter = 0;

	for (auto &rect : buttons)
	{
		if (rect.contains(mouse_pos))
			return counter;

		++counter;
	}

	return -1;
}

int MainMenu::run()
{
	const int txtposx = 15, txtposy = 180, txtygap = 50;

	sf::Text texts[3];

	std::array<sf::IntRect, 3> buttons;

	texts[0].setString("Играть");
	texts[1].setString("Настройки");
	texts[2].setString("Выход");

	for (size_t i = 0; i < 3; i++)
	{
		texts[i].setFont(fonts[0]);
		texts[i].setCharacterSize(40);
		texts[i].setFillColor(sf::Color(80, 80, 80));
		texts[i].setPosition(txtposx, txtposy + txtygap * i);
	
		buttons[i] = sf::IntRect(texts[i].getGlobalBounds()); // Strings should be set already!
	}

	std::chrono::system_clock::time_point start_time;
	std::chrono::system_clock::duration delta_time;

	int hovered_id = -1;

	soundtrack->setVolume(userdata->get_music_volume());
	soundtrack->play();

	while (true)
	{
		start_time = std::chrono::system_clock::now();

		window.clear();

		for (size_t i = 0; i < 3; i++)
		{
			window.draw(texts[i]);
		}

		window.display();

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
			{
				if (exit_question(window, fonts))
				{
					return 0;
				}
			}
			else if (event.type == sf::Event::MouseButtonReleased)
			{
				int id = button_id(sf::Mouse::getPosition(window), buttons);

				switch (id)
				{
					case 0:
					{
						soundtrack->stop();
						return 1;
					}
					case 1:
					{
						settings(); 
						break;
					}
					case 2:
					{
						if (exit_question(window, fonts))
						{
							return 0;
						}
					}
				}
			}
		}

		// Highlight effect

		int but_id = button_id(sf::Mouse::getPosition(window), buttons);

		if (but_id != -1 && hovered_id == -1)
		{
			hovered_id = but_id;
			texts[but_id].setFillColor(sf::Color(120, 120, 120));
		}
		else if (but_id == -1 && hovered_id != -1)
		{
			texts[hovered_id].setFillColor(sf::Color(80, 80, 80));
			hovered_id = -1;
		}

		delta_time = std::chrono::system_clock::now() - start_time;
		std::this_thread::sleep_for(std::chrono::microseconds(8333 - std::chrono::duration_cast<std::chrono::microseconds>(delta_time).count())); // 120 FPS
	}

	return 1;
}

void MainMenu::settings()
{
	sf::Sound sound; // To feel difference when changing settings
	sound.setBuffer(resloader.get_soundbuffers()[0]);

	std::array<sf::IntRect, 7> buttons;

	sf::Text texts[8];

	texts[0].setString("Настройки");
	texts[0].setCharacterSize(40);
	texts[0].setFont(fonts[1]); // Bold
	texts[0].setPosition(20, 15);

	texts[1].setString("Звуки");
	texts[1].setPosition(90, 70);

	texts[2].setString("Музыка");
	texts[2].setPosition(90, 110);
	
	texts[3].setString("Сохранить");
	texts[3].setFont(fonts[1]);
	texts[3].setPosition(20, 170);

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
		texts[i].setFillColor(sf::Color(80, 80, 80));
		texts[i].setFont(fonts[0]); // Regular
		texts[i].move(5, -10); // Fix library's bug
	}

	buttons[0] = sf::IntRect(texts[3].getGlobalBounds());
	
	for (size_t i = 1; i < 5; ++i)
	{
		buttons[i] = sf::IntRect(texts[i+3].getGlobalBounds());
		buttons[i].width = 20;
		buttons[i].height = 20;
	}

	sf::RectangleShape rect1, rect2;

	rect1.setFillColor(sf::Color(40, 40, 40));
	rect2.setFillColor(sf::Color(40, 40, 40));

	rect1.setPosition(240, 70);
	rect2.setPosition(240, 110);

	rect1.setSize(sf::Vector2f(2 * userdata->get_sounds_volume(), 20));
	rect2.setSize(sf::Vector2f(2 * userdata->get_music_volume(), 20));

	buttons[5] = sf::IntRect(rect1.getGlobalBounds()); // For scrolling
	buttons[5].height = 20;
	buttons[5].width = 200;

	buttons[6] = sf::IntRect(rect2.getGlobalBounds());
	buttons[6].height = 20;
	buttons[6].width = 200;


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
				int id = button_id(sf::Mouse::getPosition(window), buttons);

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
			else if (event.type == sf::Event::MouseWheelScrolled)
			{
				int id = button_id(sf::Mouse::getPosition(window), buttons);

				if (id == 5 || (id == 6))
				{
					if (id == 5)
					{
						if (event.mouseWheelScroll.delta > 0)
						{
							userdata->add_sounds();
						}
						else
						{
							userdata->sub_sounds();
						}

						sound.setVolume(userdata->get_sounds_volume());
						sound.play();
					}
					else
					{
						if (event.mouseWheelScroll.delta > 0)
						{
							userdata->add_music();
						}
						else
						{
							userdata->sub_music();
						}
					}

					soundtrack->setVolume(userdata->get_music_volume());
					rect1.setSize(sf::Vector2f(2 * userdata->get_sounds_volume(), 20));
					rect2.setSize(sf::Vector2f(2 * userdata->get_music_volume(), 20));
				}

			}
		}
	}

	delta_time = std::chrono::system_clock::now() - start_time;
	std::this_thread::sleep_for(std::chrono::microseconds(8333 - std::chrono::duration_cast<std::chrono::microseconds>(delta_time).count())); // 120 FPS
}

