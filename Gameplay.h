#ifndef GAME_PLAY_H_
#define GAME_PLAY_H_

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "ResLoader.h"
#include "Animation.h"
#include <string>
#include <chrono>
#include <thread>

class TimeGame
{
public:
	TimeGame(sf::RenderWindow &window_, ResLoader &resloader_)
		: window{window_}, resloader{resloader_}
	{
		initialize(resloader);
	}

	int play();

private:
	/*
		Tiles codes: 0 - Obeme, 1 - Gorin, 2 - Povar, 3 - Pocik, 4 - Golomyanov
	*/
	void initialize(ResLoader &resloader);

	sf::RenderWindow &window;
	ResLoader &resloader;
	const sf::Font *fonts;
	sf::Text texts[5]; /* Header, Score, Time left, High Score, Gameover*/


	int max_score, score = 0;
	

	sf::VertexArray lines {sf::Lines, 12};

	sf::Sprite main_spr[8][8];
	sf::Sprite gameover, spr_victory;

	const sf::Texture *tile_texture, *gameover_texture;

	char tiles[8][8];

	sf::Music *soundtrack, *victory, *defeat;

	sf::Sound sounds[5][5];


	void init_lines(sf::VertexArray &lines);

	void generate_main(sf::Sprite(&main_spr)[8][8], const char(&tiles)[8][8], const sf::Texture &text);

	bool analyze_tiles(char(&tiles)[8][8]);

	void regenerate_tiles(char(&tiles)[8][8]);

	void generate_tiles(char(&tiles)[8][8]);

	void tilearrpos(sf::Vector2i &selpos, const sf::Vector2i &coords);

	void copy_tiles(char(&to)[8][8], const char(&from)[8][8]);

};

#endif // !GAME_PLAY_H_
