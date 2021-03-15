#include "WindowConstants.h"
#include "Gameplay.h"
#include "ExitAsk.h"

void init_lines(sf::VertexArray &lines)
{
	lines[0].position = sf::Vector2f(WINDOW_WPAD, WINDOW_HPAD);
	lines[1].position = sf::Vector2f(WINDOW_W - WINDOW_WPAD, WINDOW_HPAD);

	lines[2].position = sf::Vector2f(WINDOW_WPAD, WINDOW_HPAD + HEADER_H);
	lines[3].position = sf::Vector2f(WINDOW_W - WINDOW_WPAD, WINDOW_HPAD + HEADER_H);

	lines[4].position = sf::Vector2f(WINDOW_WPAD, WINDOW_H - WINDOW_HPAD);
	lines[5].position = sf::Vector2f(WINDOW_W - WINDOW_WPAD, WINDOW_H - WINDOW_HPAD);

	lines[6].position = sf::Vector2f(WINDOW_WPAD, WINDOW_HPAD);
	lines[7].position = sf::Vector2f(WINDOW_WPAD, WINDOW_H - WINDOW_HPAD);

	lines[8].position = sf::Vector2f(WINDOW_W - WINDOW_WPAD, WINDOW_HPAD);
	lines[9].position = sf::Vector2f(WINDOW_W - WINDOW_WPAD, WINDOW_H - WINDOW_HPAD);

	lines[10].position = sf::Vector2f(WINDOW_WPAD + SIDE_W, WINDOW_HPAD + HEADER_H);
	lines[11].position = sf::Vector2f(WINDOW_WPAD + SIDE_W, WINDOW_H - WINDOW_HPAD);

	for (size_t i = 0; i < 12; ++i)
	{
		lines[i].color = sf::Color(194, 137, 137);
	}
}

void TimeGame::initialize()
{
	auto textures = resloader.get_textures(); 
	auto soundbuffers = resloader.get_soundbuffers();
	auto music = resloader.get_music(); 
	fonts = resloader.get_fonts();

	tile_texture = textures;
	defeat_texture = textures + 1;
	victory_texture = textures + 2;

	init_lines(lines);
	generate_tiles();
	generate_sprites();

	gameover.setScale(.498f, .525f);
	gameover.setPosition(WINDOW_WPAD + SIDE_W + GAME_PAD, WINDOW_HPAD + HEADER_H + GAME_PAD);


	/*Respectively: Header, Score, Time left, High Score, Gameover*/
	texts[0].setFont(fonts[1]);
	texts[1].setFont(fonts[0]);
	texts[2].setFont(fonts[0]);
	texts[3].setFont(fonts[0]);
	texts[4].setFont(fonts[1]);

	texts[0].setFillColor(sf::Color(169, 156, 173));
	texts[1].setFillColor(sf::Color(169, 156, 173));
	texts[2].setFillColor(sf::Color(169, 156, 173));
	texts[3].setFillColor(sf::Color(169, 156, 173));
	texts[4].setFillColor(sf::Color::Red);

	texts[0].setCharacterSize(40);
	texts[1].setCharacterSize(30);
	texts[2].setCharacterSize(30);
	texts[3].setCharacterSize(30);
	texts[4].setCharacterSize(50);

	texts[0].setPosition(WINDOW_WPAD + 5, WINDOW_HPAD + 20);
	texts[1].setPosition(WINDOW_WPAD + 5, WINDOW_HPAD + HEADER_H);
	texts[2].setPosition(WINDOW_WPAD + 5, WINDOW_HPAD + HEADER_H + GAME_SIZE / 3);
	texts[3].setPosition(WINDOW_WPAD + 5, WINDOW_HPAD + HEADER_H + GAME_SIZE * 2 / 3);
	texts[4].setPosition(WINDOW_WPAD + SIDE_W + GAME_PAD + 5, WINDOW_HPAD + HEADER_H + GAME_PAD + 5);

	max_score = resloader.get_userdata()->get_high_score();

	texts[0].setString("Игра на время");
	texts[3].setString("Рекорд\n" + std::to_string(max_score));
	texts[4].setString("Press any key");

	srand(time(NULL));

	// TODO: SOUNDS

	int musoff = 1;

	soundtrack = music + musoff + (rand() % resloader.config->music_game.size());
	soundtrack->setVolume(resloader.get_userdata()->get_music_volume());
	
	musoff += resloader.config->music_game.size();
	victory = music + musoff + (rand() % resloader.config->music_victory.size());

	musoff += resloader.config->music_victory.size();
	defeat = music + musoff + (rand() % resloader.config->music_defeat.size());

	victory->setVolume(resloader.get_userdata()->get_sounds_volume()); // It is loaded as music, but treated as sound
	defeat->setVolume(resloader.get_userdata()->get_sounds_volume());

	// TODO: SOUNDS
	// TODO: fix when everyone will have 5 sounds!

	int size = 0;
	for (size_t i = 0; i < 5; ++i)
	{
		if (i == 0)
		{
			sound_offs[i] = 0;
		}
		else
		{
			sound_offs[i] = sound_offs[i-1] + resloader.config->snd_tile[i-1].size();
		}

		size += resloader.config->snd_tile[i].size();
	}
	sounds = new sf::Sound[size];

	for (size_t i = 0; i < size; ++i)
	{
		sounds[i].setBuffer(soundbuffers[i]);
		sounds[i].setVolume(resloader.get_userdata()->get_sounds_volume());
	}
}

int TimeGame::play()
{
	constexpr int FPS = 120;
	constexpr int frame_delay = 1.0 / FPS * 1000000; // In microseconds

	unsigned int frame_counter = 0;
	unsigned int seconds = 90 + 1;

	bool gameover_once = true;
	bool update_score = true;

	bool selected = false; // A tile is seleceted for movement
	sf::Vector2i sel_pos, // Position of selected tile in tiles_sprites array
	             new_pos; // Position of tile that will be swapped with sel_pos

	bool exit_locked = true;
	int frames_left = FPS * 5.5; // wait for ending sound 

	int swap_animation = 0; // to allow double-time animation
	int disappear_animation = 0;
	int fall_animation = 0;

	std::chrono::system_clock::time_point start_time;
	std::chrono::system_clock::duration delta_time;


	// It is used to hide "falling" tiles that are initially rendered outside the game zone.
	sf::RectangleShape invisible_screen;
	invisible_screen.setFillColor(sf::Color(96, 73, 82));
	invisible_screen.setPosition(GAME_X, 0);
	invisible_screen.setSize(sf::Vector2f(WINDOW_W - WINDOW_WPAD, GAME_Y));


	soundtrack->play();

	while (true)
	{
		start_time = std::chrono::system_clock::now();
		bool has_animation = swap_animation || disappear_animation || fall_animation;

		window.clear(sf::Color(96, 73, 82));

		if (update_score)
		{
			texts[1].setString("Счет\n" + std::to_string(score));
			update_score = false;
		}
		if (seconds && frame_counter % FPS == 0)
		{
			texts[2].setString("Осталось\n" + std::to_string(--seconds));
		}

		for (size_t i = 0; i < 8; i++)
		{
			for (size_t k = 0; k < 8; k++)
			{
				window.draw(tiles_sprites[i][k]);
			}
		}
		// Swapping tile should be drawn at the top (yes, it's drawn twice; overhead is minimal).
		if (swap_animation)
		{
			window.draw(tiles_sprites[sel_pos.y][sel_pos.x]);
		}

		if (fall_animation)
		{
			window.draw(invisible_screen);
		}

		// Game over
		if (!seconds && !has_animation)
		{
			if (gameover_once)
			{
				gameover_once = false;
				soundtrack->setVolume(5);

				if (max_score < score)
				{
					resloader.get_userdata()->set_high_score(score);
					resloader.write_userdata();

					gameover.setTexture(*victory_texture);
					victory->play();
				}
				else
				{
					gameover.setTexture(*defeat_texture);
					defeat->play();
				}
			}

			window.draw(gameover);

			if (!--frames_left)
			{
				exit_locked = false;
				window.draw(texts[4]);
			}
		}
		
		// text[4] = "Press any key"
		for (size_t i = 0; i < 4 + !exit_locked; i++)
		{
			window.draw(texts[i]);
		}
		window.draw(lines);
		

		window.display();


		sf::Event event;
		while (window.pollEvent(event))
		{
			if (!has_animation && seconds && event.type == sf::Event::MouseButtonPressed)
			{
				if (selected)
				{
					tile_id(new_pos, sf::Mouse::getPosition(window));
					
					if (new_pos.x < 0)
					{
						selected = false;
						continue; // Event loop
					}

					auto diff = new_pos - sel_pos;

					if (diff.x == 0 && abs(diff.y) == 1 || diff.y == 0 && abs(diff.x) == 1)
					{
						sounds[sound_offs[tiles[sel_pos.y][sel_pos.x]] + (rand() % resloader.config->snd_tile[tiles[sel_pos.y][sel_pos.x]].size())].play();
						swap_animation = 2;
						selected = false;
						init_swap(new_pos, sel_pos);
					}
					else
					{
						sel_pos = new_pos;
					}
				}
				else
				{
					tile_id(sel_pos, sf::Mouse::getPosition(window));
					selected = (sel_pos.x != -1);
				}
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
			{
				if (exit_question(window, fonts))
				{
					soundtrack->stop();
					victory->stop();
					defeat->stop();
					return 1;
				}
			}
			else if (event.type == sf::Event::Closed)
			{
				if (exit_question(window, fonts))
				{	
					return 0;
				}
			}
			else if (!exit_locked && !seconds && (event.type == sf::Event::MouseButtonReleased || event.type == sf::Event::KeyReleased))
			{
				soundtrack->stop();
				victory->stop();
				defeat->stop();
				return 1;
			}
		}

		// Handle swipes
		if (selected && sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			tile_id(new_pos, sf::Mouse::getPosition(window));

			if (new_pos.x == -2)
			{
				selected = false;
			}

			auto diff = new_pos - sel_pos;

			if (diff.x == 0 && abs(diff.y) == 1 || diff.y == 0 && abs(diff.x) == 1)
			{
				sounds[sound_offs[tiles[sel_pos.y][sel_pos.x]] + (rand() % resloader.config->snd_tile[tiles[sel_pos.y][sel_pos.x]].size())].play();
				swap_animation = 2;
				selected = false;
				init_swap(new_pos, sel_pos);
			}
		}

		// Animation handling
		if (swap_animation)
		{
			if (!animation.apply())
			{
				auto temp = tiles[sel_pos.y][sel_pos.x];
				tiles[sel_pos.y][sel_pos.x] = tiles[new_pos.y][new_pos.x];
				tiles[new_pos.y][new_pos.x] = temp;

				update_score = true;

				generate_sprites();

				if (check_combinations())
				{
					swap_animation = 0;
					disappear_animation = 1;
					init_disappear();
				}
				else
				{
					if (swap_animation == 2)
					{
						animation.repeat();
					}
					--swap_animation;
				}
			}
		}
		else if (disappear_animation)
		{
			if (!animation.apply())
			{
				disappear_animation = 0;
				fall_animation = 1;
				update_score = true;
				init_fall();
			}
		}
		else if (fall_animation)
		{
			if (!animation.apply())
			{
				fall_animation = 0;

				generate_sprites();

				if (check_combinations())
				{
					disappear_animation = 1;
					init_disappear();
				}

			}
		}

		delta_time = std::chrono::system_clock::now() - start_time;

		std::this_thread::sleep_for(std::chrono::microseconds(frame_delay - std::chrono::duration_cast<std::chrono::microseconds>(delta_time).count()));

		++frame_counter;
	}

	return 1;
}

void TimeGame::generate_tiles()
{
	srand(time(NULL));

	for (size_t i = 0; i < 8; i++)
	{
		for (size_t k = 0; k < 8; k++)
		{
			tiles[i][k] = rand() % 5;
		}
	}

	// Spawn no combinations in the beginning
	while (check_combinations())
	{
		regenerate_tiles();
	}

}
void TimeGame::regenerate_tiles()
{
	for (size_t i = 0; i < 8; i++)
	{
		for (size_t k = 0; k < 8; k++)
		{
			if (tiles[i][k] < 0)
				tiles[i][k] = rand() % 5;
		}
	}
}
void TimeGame::generate_sprites()
{
	for (size_t i = 0; i < 8; ++i)
	{
		int offy = GAME_Y + i * (TILE_SIZE + TILE_PAD);

		for (size_t k = 0; k < 8; ++k)
		{
			int offx = GAME_X + k * (TILE_SIZE + TILE_PAD);

			tiles_sprites[i][k].setPosition(offx+30, offy+30); // To compensate origin
			tiles_sprites[i][k].setTexture(*tile_texture);
			tiles_sprites[i][k].setTextureRect(sf::IntRect(tiles[i][k] * 60, 0, 60, 60));
			tiles_sprites[i][k].setOrigin(30,30);
		}
	}
}

bool TimeGame::check_combinations()
{
	bool result = false;

	// Horizontal analysis

	for (size_t row = 0; row < 8; ++row)
	{
		char tile = tiles[row][0];
		int len = 1; // one tile is always of its own color

		// col max is 7, but situation when combination is situated at the end of the row should be handled as well
		for (size_t col = 1; col < 9; ++col)
		{
			if (col < 8 && tile == tiles[row][col])
			{
				++len;
			}
			else
			{
				// Check T (and reversed T)
				bool is_T = false;
				if (len >= 3)
				{
					// Go through midpoints of all 3-tile lines 
					for (size_t i = col - len + 1; i < col - 1; ++i)
					{
						// T down
						if (row < 6 && tiles[row + 1][i] == tile && tiles[row + 2][i] == tile)
						{
							tiles[row + 1][i] = -1;
							tiles[row + 2][i] = -1;

							result = is_T = true;
						}

						// T up
						if (row > 1 && tiles[row - 1][i] == tile && tiles[row - 2][i] == tile)
						{
							tiles[row - 1][i] = -1;
							tiles[row - 2][i] = -1;

							result = is_T = true;
						}

						if (is_T)
						{
							tiles[row][i-1] = -1;
							tiles[row][i]   = -1;
							tiles[row][i+1] = -1;

							break;
						}

					}

					// T right
					for (size_t i = col - len; i < col - 2; ++i)
					{
						if (row > 0 && row < 7 && tiles[row - 1][i] == tile && tiles[row + 1][i] == tile)
						{
							tiles[row][i+2] = -1;
							tiles[row][i+1] = -1;
							tiles[row][i]   = -1;
							tiles[row+1][i] = -1;
							tiles[row-1][i] = -1;
							
							result = is_T = true;
							break;
						}
					}

					// T left
					for (size_t i = col - len + 2; i < col; ++i)
					{
						if (row > 0 && row < 7 && tiles[row - 1][i] == tile && tiles[row + 1][i] == tile)
						{
							tiles[row][i-2] = -1;
							tiles[row][i-1] = -1;
							tiles[row][i]   = -1;
							tiles[row+1][i] = -1;
							tiles[row-1][i] = -1;

							result = is_T = true;
							break;
						}
					}

				}

				// Check cube (up and down)
				bool is_square = false;
				if (!is_T && len >= 2)
				{
					// Go through biginnings of all 2-tile lines
					for (size_t i = col - len; i < col - 1; ++i)
					{
						// Square
						if (row < 7 && tiles[row + 1][i] == tile && tiles[row + 1][i + 1] == tile)
						{
							tiles[row + 1][i + 1] = -1;
							tiles[row + 1][i] = -1;
							tiles[row][i + 1] = -1;
							tiles[row][i] = -1;

							result = is_square = true;
							break;
						}
					}
				}

				// Plain line
				if (!is_T && !is_square && len >= 3)
				{
					result = true;
					for (size_t i = col - len; i < col; ++i)
					{
						tiles[row][i] = -1;
					}
				}

				tile = tiles[row][col];
				len = 1;
			}
		}

	}
	
	// Vertical analysis (no need to check T and square).
	for (size_t col = 0; col < 8; ++col)
	{
		char tile = tiles[0][col];
		int len = 1;

		for (size_t row = 1; row < 9; ++row)
		{
			if (col < 8 && tile == tiles[row][col])
			{
				++len;
			}
			else
			{
				// Plain line
				if (len >= 3)
				{
					result = true;
					for (size_t i = row - len; i < row; ++i)
					{
						tiles[i][col] = -1;
					}
				}

				tile = tiles[row][col];
				len = 1;
			}
		}
	}

	return result;
}

void TimeGame::tile_id(sf::Vector2i &id, const sf::Vector2i &mouse_pos)
{
	if ( mouse_pos.x < GAME_X || mouse_pos.y < GAME_Y  ||
		 mouse_pos.x > GAME_X + GAME_SIZE - 2*GAME_PAD ||
		 mouse_pos.y > GAME_Y + GAME_SIZE - 2*GAME_PAD    )
	{
		id.x = -2; // Mouse position is out of game zone
		id.y = -2;
	}
	else if ( (mouse_pos.x - GAME_X) % (TILE_SIZE + TILE_PAD) >= TILE_SIZE ||
		      (mouse_pos.y - GAME_Y) % (TILE_SIZE + TILE_PAD) >= TILE_SIZE    )
	{
		id.x = -1; // Mouse position is between tiles
		id.y = -1;
	}
	else
	{
		id.x = (mouse_pos.x - GAME_X) / (TILE_SIZE + TILE_PAD);
		id.y = (mouse_pos.y - GAME_Y) / (TILE_SIZE + TILE_PAD);
	}
}

void TimeGame::init_swap(sf::Vector2i new_pos, sf::Vector2i sel_pos)
{
	animation.clear();

	AnimationDir dir;
	auto diff = new_pos - sel_pos;

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

	float scale_1 {1.01222893987}, scale_2 {0.987918800389};

	animation.add(&tiles_sprites[sel_pos.y][sel_pos.x], AnimationType::Move, dir, 30, 2.13);
	animation.add(&tiles_sprites[sel_pos.y][sel_pos.x], AnimationType::Scale, dir, 15, scale_1);
	animation.add(&tiles_sprites[sel_pos.y][sel_pos.x], AnimationType::Scale, dir, 15, scale_2, 15);

	animation.add(&tiles_sprites[new_pos.y][new_pos.x], AnimationType::Move, opposite_dir(dir), 30, 2.13);
	animation.add(&tiles_sprites[new_pos.y][new_pos.x], AnimationType::Scale, dir, 15, scale_2);
	animation.add(&tiles_sprites[new_pos.y][new_pos.x], AnimationType::Scale, dir, 15, scale_1, 15);
}
void TimeGame::init_disappear()
{
	animation.clear();

	for (size_t i = 0; i < 8; i++)
	{
		for (size_t k = 0; k < 8; k++)
		{
			if (tiles[i][k] == -1)
			{
				score += 100;
				animation.add(&tiles_sprites[i][k], AnimationType::Scale, AnimationDir::Down, 60, 0.926118728129);
			}
		}
	}
}
void TimeGame::init_fall()
{
	animation.clear();

	char temp_row[8]; // to keep new tiles before writing to main tiles array

	for (int row = 0; row < 8; ++row) 
	{
		int counter = 0; // counter of -1 in column
		int total = 0;   // total number of -1 in column; used as constant

		for (int col = 0; col < 8; ++col)
		{
			if (tiles[col][row] == -1)
			{
				++total;
			}
		}
	
		for (int col = 7; col >= 0; --col)
		{
			if (tiles[col][row] == -1)
			{
				++counter;
			
				char new_tile = rand() % 5;

				// Restore "disappeared" tile
				tiles_sprites[col][row].setScale(1, 1);
				tiles_sprites[col][row].setTextureRect(sf::IntRect(new_tile * 60, 0, 60, 60));
				tiles_sprites[col][row].setPosition(GAME_X + 64 * row + 30, GAME_Y - 64 * counter + 30);

				temp_row[counter - 1] = new_tile;

				animation.add(&tiles_sprites[col][row], AnimationType::Move, AnimationDir::Down, total * 20, 3.2);
			}
			else if (counter)
			{
				// Need to move every tile down if it's over "disappeared" tile
				animation.add(&tiles_sprites[col][row], AnimationType::Move, AnimationDir::Down, counter * 20, 3.2);
				tiles[col + counter][row] = tiles[col][row];
			}
		}

		for (size_t i = 0; i < counter; i++)
		{
			// Copy restored tiles at the top (in reversed order)
			tiles[i][row] = temp_row[counter - i - 1];
		}
	}
}