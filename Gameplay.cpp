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

	soundtrack = music + 1 + rand() % 2;
	soundtrack->setLoop(true);
	soundtrack->setVolume(resloader.get_userdata()->get_music_volume());
	
	victory = music + 3 + rand() % 3;
	defeat = music + 6 + rand() % 3;

	victory->setVolume(resloader.get_userdata()->get_sounds_volume()); // It is loaded as music, but treated as sound
	defeat->setVolume(resloader.get_userdata()->get_sounds_volume());

	// TODO: fix when everyone will have 5 sounds!
	for (size_t k = 0; k < 5; k++)
	{
		sounds[0][k].setBuffer(soundbuffers[k]);
	}
	for (size_t k = 0; k < 5; k++)
	{
		sounds[1][k].setBuffer(soundbuffers[5]);
	}
	for (size_t k = 0; k < 5; k++)
	{
		sounds[2][k].setBuffer(soundbuffers[6]);
	}
	for (size_t k = 0; k < 5; k++)
	{
		sounds[3][k].setBuffer(soundbuffers[7]);
	}
	for (size_t k = 0; k < 5; k++)
	{
		sounds[4][k].setBuffer(soundbuffers[8 + k]);
	}

	for (size_t i = 0; i < 5; i++)
	{
		for (size_t k = 0; k < 5; k++)
		{
			sounds[i][k].setVolume(resloader.get_userdata()->get_sounds_volume());
		}
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
	sf::Vector2i selpos; // selected tile's position in tiles_sprites array
	sf::Vector2i newpos; // tile that's paired with selpos to swap

	bool exit_lock = true;
	int frames_left = FPS * 5.5; // wait for ending sound 

	int swap_animation = 0; // to allow double-time animation
	int disappear_animation = 0;
	int fall_animation = 0;

	std::chrono::system_clock::time_point start_time;
	std::chrono::system_clock::duration delta_time;


	// It is used to hide "falling" tiles that are initially rendered outside the game zone.
	sf::RectangleShape screen;
	screen.setFillColor(sf::Color(96, 73, 82));
	screen.setPosition(GAME_X, 0);
	screen.setSize(sf::Vector2f(WINDOW_W - WINDOW_WPAD, GAME_Y));


	soundtrack->play();

	while (true)
	{
		start_time = std::chrono::system_clock::now();

		window.clear(sf::Color(96, 73, 82));

		if (update_score)
		{
			texts[1].setString("Счет\n" + std::to_string(score));
			update_score = false;
		}
		if (seconds && frame_counter % 120 == 0)
		{
			texts[2].setString("Осталось\n" + std::to_string(--seconds));
		}

		// Layer 3
		for (size_t i = 0; i < 8; i++)
		{
			for (size_t k = 0; k < 8; k++)
			{
				if (!(swap_animation && selpos.x == k && selpos.y == i))
					window.draw(tiles_sprites[i][k]);
			}
		}

		if (swap_animation)
		{
			window.draw(tiles_sprites[selpos.y][selpos.x]);
		}

		// Layer: invisible screen
		window.draw(screen);

		// Layer 2 : Game over
		if (!seconds && !(swap_animation || disappear_animation || fall_animation))
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
				exit_lock = false;
				window.draw(texts[4]); // "Press any key"
			}

		}
		
		// Layer 1
		for (size_t i = 0; i < 4; i++)
		{
			window.draw(texts[i]);
		}
		window.draw(lines);
		
		// Layer 0
		window.display();


		sf::Event event;
		while (window.pollEvent(event))
		{
			if (!(swap_animation || disappear_animation || fall_animation) && seconds && event.type == sf::Event::MouseButtonPressed)
			{
				if (selected)
				{
					tile_id(newpos, sf::Mouse::getPosition(window));
					
					if (newpos.x < 0)
					{
						selected = false;
						continue;
					}

					auto diff = newpos - selpos;

					if (diff.x == 0 && abs(diff.y) == 1 || diff.y == 0 && abs(diff.x) == 1)
					{
						sounds[tiles[selpos.y][selpos.x]][rand() % 5].play();
						swap_animation = 2;
						selected = false;
						init_swap(newpos, selpos);
					}
					else
					{
						selpos = newpos;
					}

				}
				else
				{
					tile_id(selpos, sf::Mouse::getPosition(window));
					selected = (selpos.x != -1);
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
					return 0;
			}
			else if (!exit_lock && (!seconds && !(swap_animation || disappear_animation || fall_animation)) && (event.type == sf::Event::MouseButtonReleased || event.type == sf::Event::KeyReleased))
			{
				soundtrack->stop();
				victory->stop();
				defeat->stop();
				return 1;

				// TODO: Refactor condition
			}
		}

		if (selected && sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			tile_id(newpos, sf::Mouse::getPosition(window));

			if (newpos.x == -2)
			{
				selected = false;
			}

			auto diff = newpos - selpos;

			if (diff.x == 0 && abs(diff.y) == 1 || diff.y == 0 && abs(diff.x) == 1)
			{
				sounds[tiles[selpos.y][selpos.x]][rand() % 5].play();
				swap_animation = 2;
				selected = false;
				init_swap(newpos, selpos);
			}
		}

		if (swap_animation)
		{
			if (!animation.apply())
			{
				auto temp = tiles[selpos.y][selpos.x];
				tiles[selpos.y][selpos.x] = tiles[newpos.y][newpos.x];
				tiles[newpos.y][newpos.x] = temp;

				generate_sprites();

				update_score = true;

				if (analyze_tiles())
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

				if (analyze_tiles())
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
	while (analyze_tiles())
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

bool TimeGame::analyze_tiles()
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

void TimeGame::tile_id(sf::Vector2i &id, const sf::Vector2i &mouse_pos)
{
	if (
		mouse_pos.x < GAME_X ||
		mouse_pos.x > GAME_X + GAME_SIZE - 2*GAME_PAD ||
		mouse_pos.y < GAME_Y ||
		mouse_pos.y > GAME_Y + GAME_SIZE - 2*GAME_PAD
	   )
	{
		id.x = -2; // Mouse position is out of game zone
		id.y = -2;
	}
	else if (
	         (mouse_pos.x - GAME_X) % (TILE_SIZE + TILE_PAD) >= TILE_SIZE ||
		     (mouse_pos.y - GAME_Y) % (TILE_SIZE + TILE_PAD) >= TILE_SIZE
		    )
			
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

void TimeGame::init_swap(sf::Vector2i newpos, sf::Vector2i selpos)
{
	animation.clear();

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

	float scale_1 {1.01222893987}, scale_2 {0.987918800389};

	animation.add(&tiles_sprites[selpos.y][selpos.x], AnimationType::Move, dir, 30, 2.13);
	animation.add(&tiles_sprites[selpos.y][selpos.x], AnimationType::Scale, dir, 15, scale_1);
	animation.add(&tiles_sprites[selpos.y][selpos.x], AnimationType::Scale, dir, 15, scale_2, 15);

	animation.add(&tiles_sprites[newpos.y][newpos.x], AnimationType::Move, opposite_dir(dir), 30, 2.13);
	animation.add(&tiles_sprites[newpos.y][newpos.x], AnimationType::Scale, dir, 15, scale_2);
	animation.add(&tiles_sprites[newpos.y][newpos.x], AnimationType::Scale, dir, 15, scale_1, 15);
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

	int total; // number of -1 in column; used for iterations
	int orig_total; // the same; used as constant

	// TODO move tiles_temp declaration here
	// TODO: think, do we actually need the second tiles array now? 
	//copy_tiles(tiles_temp, tiles);

	char temp_row[8]; // to keep new tiles before writing to main tiles array

	for (int row = 0; row < 8; ++row) 
	{
		total = 0;
		orig_total = 0;

		for (int col = 0; col < 8; ++col)
		{
			if (tiles[col][row] == -1)
			{
				++orig_total;
			}
		}
	
		for (int col = 7; col >= 0; --col)
		{
			if (tiles[col][row] == -1)
			{
				++total;
			
				char new_tile = rand() % 5;

				tiles_sprites[col][row].setScale(1, 1);
				tiles_sprites[col][row].setTextureRect(sf::IntRect(new_tile * 60, 0, 60, 60));
				tiles_sprites[col][row].setPosition(GAME_X + 64 * row + 30, GAME_Y - 64 * total + 30);

				temp_row[total - 1] = new_tile;

				animation.add(&tiles_sprites[col][row], AnimationType::Move, AnimationDir::Down, orig_total * 20, 3.2);

			}
			else
			{
				if (total)
				{
					animation.add(&tiles_sprites[col][row], AnimationType::Move, AnimationDir::Down, total*20, 3.2);
					tiles[col + total][row] = tiles[col][row];
				}
			}
		}

		for (size_t i = 0; i < total; i++)
		{
			tiles[i][row] = temp_row[total - i - 1];
		}

	}
}