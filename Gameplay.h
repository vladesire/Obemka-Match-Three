#ifndef GAME_PLAY_H_
#define GAME_PLAY_H_

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <chrono>
#include <thread>
#include <string>

#include "ResLoader.h"
#include "Animation.h"


//FOR DEBUG ONLY!!!!!!!!!!!!!!!!!!!!!!!!!!!
#include <iostream>
inline void ___print_tiles(char(&tiles)[8][8])
{
	std::cout << "\nTiles dump\n";
	for (size_t i = 0; i < 8; i++)
	{
		for (size_t k = 0; k < 8; k++)
		{
			std::cout << (int)tiles[i][k] << " ";
		}

		std::cout << "\n";
	}
}
//FOR DEBUG ONLY!!!!!!!!!!!!!!!!!!!!!!!!!!!


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

	const int height = 640, width = 740, headw = 692, headh = 80, sidew = 160, sideh = 532, mainsz = 532, mainpad = 12, itempad = 4, tilesz = 60;
	const int whpad = 14, wwpad = 24; // window height/width padding

	int max_score, score = 0;
	

	sf::VertexArray lines {sf::Lines, 12};

	sf::Sprite main_spr[8][8];
	sf::Sprite gameover, spr_victory;

	const sf::Texture *tile_texture, *gameover_texture;

	char tiles[8][8];
	char tiles_temp[8][8]; // for drop animation


	sf::Music *soundtrack, *victory, *defeat;

	sf::Sound sounds[5][5];


	void init_lines(sf::VertexArray &lines);

	void generate_main(sf::Sprite(&main_spr)[8][8], const char(&tiles)[8][8], const sf::Texture &text);

	bool analyze_tiles(char(&tiles)[8][8]);

	void regenerate_tiles(char(&tiles)[8][8]);

	void generate_tiles(char(&tiles)[8][8]);

	void tilearrpos(sf::Vector2i &selpos, const sf::Vector2i &coords);

	void init_swap(CorrAnimation &anim, sf::Sprite(&main_spr)[8][8], sf::Vector2i newpos, sf::Vector2i selpos);
	void init_disappear(CorrAnimation &canim, sf::Sprite(&main_spr)[8][8], const char(&tiles)[8][8], int &score);
	void init_drop(CorrAnimation &canim, sf::Sprite(&main_spr)[8][8], char(&tiles_temp)[8][8]);

	void copy_tiles(char(&to)[8][8], const char(&from)[8][8]);

};

#endif // !GAME_PLAY_H_
