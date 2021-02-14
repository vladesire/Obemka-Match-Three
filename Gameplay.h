#ifndef GAME_PLAY_H_
#define GAME_PLAY_H_

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <chrono>
#include <thread>
#include <string>

#include "Animation.h"

/*
//FOR DEBUG ONLY!!!!!!!!!!!!!!!!!!!!!!!!!!!
#include <iostream>
void ___print_tiles(char(&tiles)[8][8])
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
*/

class MovesGame
{
public:
	MovesGame(sf::RenderWindow &window_, const sf::Font *fonts_, const sf::Texture *textures, const sf::SoundBuffer *soundbuffers, sf::Music *music) 
		: window{window_}, fonts{fonts_}
	{
		initialize(textures, soundbuffers, music);
	}

	int play();

private:
	/*
		Tiles codes: 0 - Obeme, 1 - Gorin, 2 - Povar, 3 - Pocik, 4 - Golomyanov
	*/

	int max_score;

	void initialize(const sf::Texture *textures, const sf::SoundBuffer *soundbuffers, sf::Music *music);

	sf::RenderWindow &window;
	const sf::Font *fonts;

	const int height = 640, width = 740, headw = 692, headh = 80, sidew = 160, sideh = 532, mainsz = 532, mainpad = 12, itempad = 4, tilesz = 60;
	const int whpad = 14, wwpad = 24; // window height/width padding

	bool gameover_once = true;
	
	bool selected = false; // A tile is seleceted for movement
	sf::Vector2i selpos; // selected tile's position in main_spr array
	sf::Vector2i newpos; // tile that's paired with selpos to swap

	int swap_animation = 0; // to allow double-time animation
	int disappear_animation = 0;
	int drop_animation = 0;

	int iscore = 0, itarget = 14000, imoves = 15; // to use in strings
	bool state_changed = true; // to update strings


	sf::VertexArray lines{sf::Lines, 12};

	sf::Sprite main_spr[8][8];
	sf::Sprite gameover, spr_victory;

	const sf::Texture *tile_texture, *gameover_texture;

	char tiles[8][8];
	char tiles_temp[8][8]; // for drop animation

	sf::Text header, score, moves, target, gameovertext;

	sf::Music *soundtrack, *victory;

	sf::Sound kizhak, chetko;
	sf::Sound sounds[5][5];

	Animation anim, anim_2; // to handle two different collective animations; anim_2 for disappear, second swap



	void init_lines(sf::VertexArray &lines)
	{

		lines[0].position = sf::Vector2f(wwpad, whpad);
		lines[1].position = sf::Vector2f(width - wwpad, whpad);

		lines[2].position = sf::Vector2f(wwpad, whpad + headh);
		lines[3].position = sf::Vector2f(width - wwpad, whpad + headh);

		lines[4].position = sf::Vector2f(wwpad, height - whpad);
		lines[5].position = sf::Vector2f(width - wwpad, height - whpad);

		lines[6].position = sf::Vector2f(wwpad, whpad);
		lines[7].position = sf::Vector2f(wwpad, height - whpad);

		lines[8].position = sf::Vector2f(width - wwpad, whpad);
		lines[9].position = sf::Vector2f(width - wwpad, height - whpad);

		lines[10].position = sf::Vector2f(wwpad + sidew, whpad + headh);
		lines[11].position = sf::Vector2f(wwpad + sidew, height - whpad);

		for (size_t i = 0; i < 12; ++i)
		{
			lines[i].color = sf::Color(194, 137, 137);
		}

	}

	void generate_main(sf::Sprite(&main_spr)[8][8], const char(&tiles)[8][8], const sf::Texture &text)
	{
		int orh = whpad + headh + mainpad; // origin height
		int orw = wwpad + sidew + mainpad; // origin width

		for (size_t i = 0; i < 8; ++i)
		{
			int origh = orh + i * (tilesz + itempad);

			for (size_t k = 0; k < 8; ++k)
			{
				int origw = orw + k * (tilesz + itempad);

				main_spr[i][k].setPosition(origw, origh);
				main_spr[i][k].setTexture(text);
				main_spr[i][k].setTextureRect(sf::IntRect(tiles[i][k] * 60, 0, 60, 60));
			}
		}
	}

	bool analyze_tiles(char(&tiles)[8][8])
	{
		bool changed = false;
		// horizontal analysis

		//        |        |        midpoints
		// [0][1][2][3][4][5][6][7]
		// [0][1][2][3][4][5][6][7]

		for (size_t i = 0; i < 8; i++)
		{
			char col = tiles[i][2];
			int left = 0, right = 0;

			for (int k = 1; k >= 0; --k)
			{
				if (tiles[i][k] == col)
					++left;
				else
					break;
			}

			for (int k = 3; k <= 6; ++k)
			{
				if (tiles[i][k] == col)
					++right;
				else
					break;
			}

			if (left + right + 1 >= 3)
			{
				changed = true;

				for (int k = 2 - left; k <= 2 + right; k++)
				{
					tiles[i][k] = -1;
				}
			}

			col = tiles[i][5];

			if (col != -1)
			{
				left = right = 0;

				for (int k = 4; k >= 1; --k)
				{
					if (tiles[i][k] == col)
						++left;
					else
						break;
				}

				for (int k = 6; k <= 7; ++k)
				{
					if (tiles[i][k] == col)
						++right;
					else
						break;
				}

				if (left + right + 1 >= 3)
				{
					changed = true;

					for (int k = 5 - left; k <= 5 + right; k++)
					{
						tiles[i][k] = -1;
					}
				}
			}

		}

		for (size_t i = 0; i < 8; i++)
		{
			char col = tiles[2][i];
			int left = 0, right = 0;

			for (int k = 1; k >= 0; --k)
			{
				if (tiles[k][i] == col)
					++left;
				else
					break;
			}

			for (int k = 3; k <= 6; ++k)
			{
				if (tiles[k][i] == col)
					++right;
				else
					break;
			}

			if (left + right + 1 >= 3)
			{
				changed = true;

				for (int k = 2 - left; k <= 2 + right; k++)
				{
					tiles[k][i] = -1;
				}
			}

			col = tiles[5][i];

			if (col != -1)
			{
				left = right = 0;

				for (int k = 4; k >= 1; --k)
				{
					if (tiles[k][i] == col)
						++left;
					else
						break;
				}

				for (int k = 6; k <= 7; ++k)
				{
					if (tiles[k][i] == col)
						++right;
					else
						break;
				}

				if (left + right + 1 >= 3)
				{
					changed = true;

					for (int k = 5 - left; k <= 5 + right; k++)
					{
						tiles[k][i] = -1;
					}
				}
			}

		}
		return changed;
	}

	void regenerate_tiles(char(&tiles)[8][8])
	{
		for (size_t i = 0; i < 8; i++)
		{
			for (size_t k = 0; k < 8; k++)
			{
				if (tiles[i][k] == -2)
					tiles[i][k] = rand() % 5;
			}
		}
	}

	void generate_tiles(char(&tiles)[8][8])
	{
		srand(time(NULL));

		for (size_t i = 0; i < 8; i++)
		{
			for (size_t k = 0; k < 8; k++)
			{
				tiles[i][k] = rand() % 5;
			}
		}

		while (analyze_tiles(tiles))
		{
			for (size_t i = 0; i < 8; i++)
			{
				for (size_t k = 0; k < 8; k++)
				{
					if (tiles[i][k] == -1)
						tiles[i][k] = rand() % 5;
				}
			}
		}

	}

	void tilearrpos(sf::Vector2i &selpos, const sf::Vector2i &coords)
	{
		int orh = whpad + headh + mainpad; // origin height
		int orw = wwpad + sidew + mainpad; // origin width

		if (coords.x < orw ||
			coords.x >(width - wwpad) ||
			coords.y < orh ||
			coords.y >(height - whpad) ||
			((coords.x - orw) / 64.0 - (int)((coords.x - orw) / 64.0) > 0.9375) ||
			((coords.y - orh) / 64.0 - (int)((coords.y - orh) / 64.0) > 0.9375)
			)
		{
			selpos.x = -1;
			selpos.y = -1;
		}
		else
		{
			selpos.x = (coords.x - orw) / 64.0;
			selpos.y = (coords.y - orh) / 64.0;
		}

	}

	void init_swap(Animation &a1, Animation &a2, sf::Sprite(&main_spr)[8][8], sf::Vector2i newpos, sf::Vector2i selpos)
	{
		a1.clear();
		a2.clear();

		auto diff = newpos - selpos;
		AnimationDir dir;

		if (diff.x == -1)
		{
			dir = AnimationDir::Left;
		}
		else if (diff.x == 1)
		{
			dir = AnimationDir::Right;
		}
		else if (diff.y == -1)
		{
			dir = AnimationDir::Top;
		}
		else
		{
			dir = AnimationDir::Down;
		}

		a1.set_animation(AnimationType::Move, dir, 30, 2);
		a1.add(&main_spr[selpos.y][selpos.x]);
		a2.set_animation(AnimationType::Move, opposite_dir(dir), 30, 2);
		a2.add(&main_spr[newpos.y][newpos.x]);

	}
	void init_disappear(Animation &a, sf::Sprite(&main_spr)[8][8], const char(&tiles)[8][8], int &score)
	{
		a.clear();

		for (size_t i = 0; i < 8; i++)
		{
			for (size_t k = 0; k < 8; k++)
			{
				if (tiles[i][k] == -1)
				{
					score += 100;
					main_spr[i][k].setOrigin(30, 30);
					main_spr[i][k].move(30, 30); // to compensate origin move
					a.add(&main_spr[i][k]);
				}
			}
		}

		a.set_animation(AnimationType::Disappear, AnimationDir::Down, 60, 0.85);

	}

	void init_drop(Animation &a, sf::Sprite(&main_spr)[8][8], char(&tiles_temp)[8][8])
	{
		a.clear();

		int in_col; // number of -1 in column

		for (int i = 0; i < 8; ++i) // row
		{
			in_col = 0;
			for (int k = 0, top = 0; k < 8; ++k) // column
			{
				if (tiles_temp[k][i] == -1)
				{
					++in_col;

					for (int r = k; r >= in_col; --r)
					{
						tiles_temp[r][i] = tiles_temp[r - 1][i];
						//a.add(&main_spr[r-1][i]);
					}
					tiles_temp[in_col - 1][i] = -2;
				}
			}
		}

		//a.set_animation(AnimationType::Move, AnimationDir::Down, 30, 2);

	}

	void reverse_swap(Animation &a1, Animation &a2)
	{
		a1.set_animation(AnimationType::Move, a1.get_dir(), 30, 2); // No need for reversion of dir; object are swapped in main_spr
		a2.set_animation(AnimationType::Move, a2.get_dir(), 30, 2);
	}
	void copy_tiles(char(&to)[8][8], const char(&from)[8][8])
	{
		for (size_t i = 0; i < 8; i++)
		{
			for (size_t k = 0; k < 8; k++)
			{
				to[i][k] = from[i][k];
			}
		}
	}


};

#endif // !GAME_PLAY_H_
