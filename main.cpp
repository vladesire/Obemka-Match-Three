#include <SFML/Graphics.hpp>

const int height = 640, width = 740;

#include "ResLoader.h"
#include "MainMenu.h"
#include "Gameplay.h"

#include <iostream>

int main()
{
	ResLoader resloader;
	sf::RenderWindow window {sf::VideoMode(width, height), "Obemka: match-three", sf::Style::Close};
	MainMenu main_menu {window, resloader.get_fonts()};

	main_menu.splash(); // TODO: in a different thread (while resloader is loading resources, 
	                    // then need to separate MainMenu and splash

	while (int state = main_menu.run())
	{
		if (state == 1)
		{
			std::cout << "Moves non-campaign game\n";

			MovesGame game {window, resloader.get_fonts(), resloader.get_textures(), resloader.get_soundbuffers(), resloader.get_music()};

			if (!game.play())
			{	
				std::cout << "Close window request\n";
				window.close();
				break; 
			}
		}

	}

	return 0;
}