#ifndef MAIN_MENU_H_
#define MAIN_MENU_H_

#include <SFML/Graphics.hpp>
#include "ResLoader.h"
#include "ExitAsk.h"
#include <thread>
#include <chrono>
#include <array>

class MainMenu
{
public:
	MainMenu(sf::RenderWindow &window_, ResLoader &resloader_): 
		window{window_}, resloader{resloader_}, fonts{resloader_.get_fonts()}, soundtrack{resloader_.get_music()}, userdata{resloader_.get_userdata()}
	{}

	int run();
	void settings();

private:
	sf::RenderWindow &window;
	ResLoader &resloader;
	ResLoader::UserData *userdata;
	const sf::Font *fonts;
	sf::Music *soundtrack;

	bool first_time = true; // to handle music offset
};

#endif // !MAIN_MENU_H_
