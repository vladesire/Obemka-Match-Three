#include <SFML/Graphics.hpp>
#include "WindowConstants.h"
#include "ResLoader.h"
#include "Gameplay.h"
#include "MainMenu.h"
#include "Splash.h"
#include <thread>
#include <locale>


int main()
{
	std::locale::global(std::locale("rus"));

	ResLoader resloader;
	
	sf::RenderWindow window {sf::VideoMode(WINDOW_W, WINDOW_H), "Обемка: три в ряд", sf::Style::Close};

	std::thread t {&ResLoader::load, &resloader};
	
	window.setMouseCursorVisible(false);
	splash(window); 
	
	t.join();
	
	window.setMouseCursorVisible(true);

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