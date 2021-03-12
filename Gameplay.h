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
		initialize();
	}

	int play();

private:
	sf::RenderWindow &window;
	ResLoader &resloader;
	Animation animation; 

	const sf::Texture *tile_texture, *defeat_texture, *victory_texture;
	const sf::Font *fonts;
	
	sf::Text texts[5]; /* Header, Score, Time left, High Score, Gameover */
	sf::Music *soundtrack, *victory, *defeat;
	sf::VertexArray lines {sf::Lines, 12};
	sf::Sound sounds[5][5];
	sf::Sprite gameover;

	// Tiles codes: 
	// -2:  Fall animation
	// -1:  Disappear animation
	// 0-4: Texture id (no animation) 
	sf::Sprite tiles_sprites[8][8];
	char tiles[8][8];

	int max_score, score = 0;


	void initialize();
	void generate_tiles();
	void regenerate_tiles();
	void generate_sprites();

	bool analyze_tiles();

	// ID is position of physical tile in (logical) tile array
	void tile_id(sf::Vector2i &id, const sf::Vector2i &mouse_pos);

	// Set animation up
	void init_swap(sf::Vector2i newpos, sf::Vector2i selpos);
	void init_disappear();
	void init_fall();
};

#endif // !GAME_PLAY_H_
