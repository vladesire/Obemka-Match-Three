#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <thread>
#include <chrono>
#include <string>

#include "Animation.h"

/*Projects\Obemka Match-Three
	Tiles codes: 0 - Obeme, 1 - Gorin, 2 - Povar, 3 - Pocik, 4 - Golomyanov
*/

const int height = 640, width = 740, headw = 692, headh = 80, sidew = 160, sideh = 532, mainsz = 532, mainpad = 12, itempad = 4, tilesz = 60;
const int whpad = 14, wwpad = 24; // window height/width padding

void lines_init(sf::VertexArray &lines)
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

void tilearrpos(sf::Vector2i &selpos, const sf::Vector2i &coords)
{
	int orh = whpad + headh + mainpad; // origin height
	int orw = wwpad + sidew + mainpad; // origin width

	if (coords.x < orw ||
		coords.x > (width - wwpad) ||
		coords.y < orh ||
		coords.y > (height - whpad) ||
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
					tiles_temp[r][i] = tiles_temp[r-1][i];
					//a.add(&main_spr[r-1][i]);
				}
				tiles_temp[in_col-1][i] = -2;
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

int main()
{
	//dir: 0 down, 1 up, 2 left, 3 right

	bool selected = false; // A tile is seleceted for movement
	sf::Vector2i selpos; // selected tile's position in main_spr array
	sf::Vector2i newpos; // tile that's paired with selpos to swap

	int swap_animation = 0; // to allow double-time animation
	int disappear_animation = 0;
	int drop_animation = 0;

	int iscore = 0, itarget = 14000, imoves = 15; // to use in strings
	bool state_changed = true; // to update strings

	sf::RenderWindow window {sf::VideoMode(width, height), "Obemka: match-three", sf::Style::Close};

	sf::VertexArray lines {sf::Lines, 12};
	lines_init(lines);

	sf::Sprite main_spr[8][8];
	sf::Sprite gameover;

	sf::Texture tile_text, gameover_text;
	tile_text.loadFromFile("tiles.png");
	
	gameover_text.loadFromFile("gameover.jpg");
	gameover.setScale(.498f, .525f);
	gameover.setPosition(wwpad + sidew + mainpad, whpad + headh + mainpad);
	gameover.setTexture(gameover_text);
	
	char tiles[8][8];
	char tiles_temp[8][8]; // for drop animation
	generate_tiles(tiles);


	generate_main(main_spr, tiles, tile_text);
	
	sf::Font font_bold, font_normal;
	font_bold.loadFromFile("Fonts/RobotoBold.ttf");
	font_normal.loadFromFile("Fonts/RobotoRegular.ttf");

	sf::Text header, score, moves, target, gameovertext; // TODO: update score, moves in the logic functions.

	header.setFont(font_bold);
	score.setFont(font_normal);
	moves.setFont(font_normal);
	target.setFont(font_normal);
	gameovertext.setFont(font_bold);

	header.setFillColor(sf::Color(169, 156, 173));
	score.setFillColor(sf::Color(169, 156, 173));
	moves.setFillColor(sf::Color(169, 156, 173));
	target.setFillColor(sf::Color(169, 156, 173));
	gameovertext.setFillColor(sf::Color::Red);

	header.setCharacterSize(40);
	score.setCharacterSize(30);
	moves.setCharacterSize(30);
	target.setCharacterSize(30);
	gameovertext.setCharacterSize(50);
	
	
	header.setPosition(wwpad+5, whpad+20);
	score.setPosition(wwpad+5, whpad+headh);
	moves.setPosition(wwpad+5, whpad+headh+mainsz/3);
	target.setPosition(wwpad+5, whpad+headh+mainsz*2/3);
	gameovertext.setPosition(wwpad + sidew + mainpad + 5, whpad + headh + mainpad + 5);
	
	header.setString("Level 1. The Theme.");
	target.setString("Target\n" + std::to_string(itarget));
	gameovertext.setString("Game Over");

	sf::Music soundtrack;
	soundtrack.openFromFile("soundtrack.wav");
	soundtrack.setLoop(true);
	soundtrack.setVolume(50);
	soundtrack.play();

	sf::SoundBuffer obeme[5], gorin[5], povar[5], pocik[5];
	sf::SoundBuffer *sndarrays[5] {obeme, gorin, povar, pocik, obeme}; // todo
	sf::SoundBuffer kizhakbuf;
	obeme[0].loadFromFile("Sounds/obeme_1.wav");
	obeme[1].loadFromFile("Sounds/obeme_2.wav");
	obeme[2].loadFromFile("Sounds/obeme_3.wav");
	obeme[3].loadFromFile("Sounds/obeme_4.wav");
	obeme[4].loadFromFile("Sounds/obeme_5.wav");

	povar[0].loadFromFile("Sounds/povar_1.wav");
	povar[1].loadFromFile("Sounds/povar_1.wav");
	povar[2].loadFromFile("Sounds/povar_1.wav");
	povar[3].loadFromFile("Sounds/povar_1.wav");
	povar[4].loadFromFile("Sounds/povar_1.wav");

	gorin[0].loadFromFile("Sounds/gorin_1.wav");
	gorin[1].loadFromFile("Sounds/gorin_1.wav");
	gorin[2].loadFromFile("Sounds/gorin_1.wav");
	gorin[3].loadFromFile("Sounds/gorin_1.wav");
	gorin[4].loadFromFile("Sounds/gorin_1.wav");

	pocik[0].loadFromFile("Sounds/pocik_1.wav");
	pocik[1].loadFromFile("Sounds/pocik_1.wav");
	pocik[2].loadFromFile("Sounds/pocik_1.wav");
	pocik[3].loadFromFile("Sounds/pocik_1.wav");
	pocik[4].loadFromFile("Sounds/pocik_1.wav");

	kizhakbuf.loadFromFile("kizhak.wav");

	sf::Sound kizhak;
	sf::Sound sounds[5][5];

	for (size_t i = 0; i < 5; i++)
	{
		for (size_t k = 0; k < 5; k++)
		{
			sounds[i][k].setBuffer(sndarrays[i][k]);
		}
	}
	
	kizhak.setBuffer(kizhakbuf);

	bool gameover_once = true;

	std::chrono::system_clock::time_point start_time;
	std::chrono::system_clock::duration delta_time;
	
	Animation anim, anim_2; // to handle two different collective animations; anim_2 for disappear, second swap

	while (window.isOpen())
	{
		start_time = std::chrono::system_clock::now();
		
		window.clear(sf::Color(96, 73, 82));

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
			{
				// TODO: Ask before closing
				window.close();
			}
			else if (!(swap_animation || disappear_animation || drop_animation) && imoves && event.type == sf::Event::MouseButtonPressed)
			{
				if (selected)
				{
					tilearrpos(newpos, sf::Mouse::getPosition(window));
					auto diff = newpos - selpos;

					if (newpos.x == -1)
					{
						selected = false;
					}
					else if (diff.x == 0 && abs(diff.y) == 1 || diff.y == 0 && abs(diff.x) == 1)
					{
						sounds[tiles[selpos.y][selpos.x]][rand() % 5].play();
						swap_animation = 2;
						selected = false;
						init_swap(anim, anim_2, main_spr, newpos, selpos);
						--imoves;
					}
					else
					{
						selpos = newpos;
					}
					
				}
				else
				{
					tilearrpos(selpos, sf::Mouse::getPosition(window));
					selected = (selpos.x != -1);
				}

			}
		
		}

		if (selected && sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{

			tilearrpos(newpos, sf::Mouse::getPosition(window));

			auto diff = newpos - selpos;

			if (diff.x == 0 && abs(diff.y) == 1 || diff.y == 0 && abs(diff.x) == 1)
			{
				sounds[tiles[selpos.y][selpos.x]][rand() % 5].play();
				swap_animation = 2;
				selected = false;
				init_swap(anim, anim_2, main_spr, newpos, selpos);
				--imoves;
			}
			
		}

		if (swap_animation)
		{
			if (!(anim.apply() && anim_2.apply()))
			{
				auto temp = tiles[selpos.y][selpos.x];
				tiles[selpos.y][selpos.x] = tiles[newpos.y][newpos.x];
				tiles[newpos.y][newpos.x] = temp;

				generate_main(main_spr, tiles, tile_text);

				state_changed = true;

				if (analyze_tiles(tiles))
				{
					swap_animation = 0;
					disappear_animation = 1;

					init_disappear(anim_2, main_spr, tiles, iscore);


					// TODO: REGENERATE -1 FIRST. IT WON'T WORK OTHERWISE.

					// Play disappear animation: scale(0.9, 0.9) + setOrignin(30, 30)
					// then regenerate
					// only then play fall animation pop up from the middle
					// then try analyze again. Repeat till no more 3-in-line
				}
				else
				{
					if (swap_animation == 2)
					{
						
						reverse_swap(anim, anim_2);
					}
					--swap_animation;
				}

			}
		}
		else if (disappear_animation)
		{
			if (!(anim_2.apply())) // anim_2 because I will need it to restore tiles after drop
			{
				disappear_animation = 0;
				drop_animation = 1;
				state_changed = true;

				copy_tiles(tiles_temp, tiles);

				init_drop(anim, main_spr, tiles_temp);

			}
		}
		else if (drop_animation)
		{
			if (true/*!(anim.apply())*/) // For now, without animation
			{
				drop_animation = 0;

				for (size_t i = 0; i < 8; i++) // Restore disappeared tiles
				{
					for (size_t k = 0; k < 8; k++)
					{
						if (tiles[i][k] == -1) //using old values
						{
							main_spr[i][k].setScale(1,1);
							main_spr[i][k].setOrigin(0, 0);
						}
					}
				}

				copy_tiles(tiles, tiles_temp);

				regenerate_tiles(tiles);

				generate_main(main_spr, tiles, tile_text);

				if (analyze_tiles(tiles))
				{
					disappear_animation = 1;

					init_disappear(anim_2, main_spr, tiles, iscore);

				}

			}
		}

		if (state_changed)
		{
			moves.setString("Moves\n" + std::to_string(imoves));
			score.setString("Score\n" + std::to_string(iscore));
			state_changed = false;
		}

		

		window.draw(lines);
		window.draw(header);
		window.draw(score);
		window.draw(moves);
		window.draw(target);

		for (size_t i = 0; i < 8; i++)
		{
			for (size_t k = 0; k < 8; k++)
			{
				window.draw(main_spr[i][k]);
			}
		}

		if (!imoves && !(swap_animation || disappear_animation || drop_animation))
		{

			if (gameover_once)
			{
				soundtrack.setVolume(10);
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				kizhak.play();
				gameover_once = false;
			}

			window.draw(gameover);
			//window.draw(gameovertext);
		}

		window.display();


		delta_time = std::chrono::system_clock::now() - start_time;

		std::this_thread::sleep_for(std::chrono::microseconds(8333 - std::chrono::duration_cast<std::chrono::microseconds>(delta_time).count()));

		//std::cout << std::chrono::duration_cast<std::chrono::microseconds>(delta_time).count() << "\n";
	}

	return 0;
}