#include <SFML/Graphics.hpp>
#include <thread>
#include <locale>
#include "Splash.h"
#include "ResLoader.h"
#include "MainMenu.h"
#include "Gameplay.h"

const int height = 640, width = 740;

/*
	Game ideas:

		1. One of the last levels: win with disabled cursor (obama took it from you...)

		2. Create in-game cursor and click sounds

		3. Settings
			Sounds | Create variable to set them, pass to function,
			Music  | change in settings (main_menu class)


*/

/*
	Todo list:

		1. sf::Event::MouseButtonPressed -> sf::Event::MouseButtonReleased
		2. In every render cycle: first draw, then update! 
*/


int main()
{
	std::locale::global(std::locale("rus"));

	ResLoader resloader;
	
	sf::RenderWindow window {sf::VideoMode(width, height), "Обемка: три в ряд", sf::Style::Close};

	std::thread t {&ResLoader::load, &resloader};
	
	window.setMouseCursorVisible(false);
	splash(window); // FIX SPLASH!
	
	t.join();
	

	/*sf::Image img_cursor; // TODO cursor
	img_cursor.loadFromFile("cursor.png");
	sf::Cursor cursor;
	cursor.loadFromPixels(img_cursor.getPixelsPtr(), sf::Vector2u(53, 87), sf::Vector2u(4, 3));
	window.setMouseCursor(cursor);*/

	window.setMouseCursorVisible(true); // Set in game curosor!

	MainMenu main_menu {window, resloader};

	while (int state = main_menu.run())
	{
		if (state == 1)
		{
			TimeGame game{window, resloader};

			if (!game.play())
			{	
				window.close();
				break; 
			}
		}

	}

	return 0;
}