#include "Gameplay.h"
#include "ExitAsk.h"

void TimeGame::initialize(ResLoader &resloader)
{
	auto textures = resloader.get_textures(); 
	auto soundbuffers = resloader.get_soundbuffers();
	auto music = resloader.get_music(); 
	fonts = resloader.get_fonts();


	tile_texture = textures;
	gameover_texture = textures + 1;

	init_lines(lines);

	gameover.setScale(.498f, .525f);
	gameover.setPosition(wwpad + sidew + mainpad, whpad + headh + mainpad);
	gameover.setTexture(*gameover_texture);

	spr_victory.setScale(.498f, .525f);
	spr_victory.setPosition(wwpad + sidew + mainpad, whpad + headh + mainpad);
	spr_victory.setTexture(textures[2]);


	generate_tiles(tiles);

	generate_main(main_spr, tiles, *tile_texture);


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

	texts[0].setPosition(wwpad + 5, whpad + 20);
	texts[1].setPosition(wwpad + 5, whpad + headh);
	texts[2].setPosition(wwpad + 5, whpad + headh + mainsz / 3);
	texts[3].setPosition(wwpad + 5, whpad + headh + mainsz * 2 / 3);
	texts[4].setPosition(wwpad + sidew + mainpad + 5, whpad + headh + mainpad + 5);

	max_score = resloader.get_userdata()->get_high_score();

	texts[0].setString("���� �� �����");
	texts[3].setString("������\n" + std::to_string(max_score));
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
	unsigned int frame_counter = 0; // FPS = 120
	unsigned int seconds = 5 + 1;

	bool gameover_once = true;
	bool update_score = true;

	bool selected = false; // A tile is seleceted for movement
	sf::Vector2i selpos; // selected tile's position in main_spr array
	sf::Vector2i newpos; // tile that's paired with selpos to swap

	bool exit_lock = true;
	int frames_left = 120 * 5 + 60; // wait for ending sound 

	int swap_animation = 0; // to allow double-time animation
	int disappear_animation = 0;
	int drop_animation = 0;

	std::chrono::system_clock::time_point start_time;
	std::chrono::system_clock::duration delta_time;

	Animation anim, anim_2; // to handle two different collective animations; anim_2 for disappear, second swap

	soundtrack->play();

	while (true)
	{
		start_time = std::chrono::system_clock::now();

		sf::Event event;
		while (window.pollEvent(event))
		{

			// TODO: change order, check animation first
			if (event.type == sf::Event::Closed)
			{
				if (exit_question(window, fonts))
					return 0;
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
			else if (!(swap_animation || disappear_animation || drop_animation) && seconds && event.type == sf::Event::MouseButtonPressed)
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
			else if (!exit_lock && (!seconds && !(swap_animation || disappear_animation || drop_animation)) && (event.type == sf::Event::MouseButtonReleased || event.type == sf::Event::KeyReleased))
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

			tilearrpos(newpos, sf::Mouse::getPosition(window));

			auto diff = newpos - selpos;

			if (diff.x == 0 && abs(diff.y) == 1 || diff.y == 0 && abs(diff.x) == 1)
			{
				sounds[tiles[selpos.y][selpos.x]][rand() % 5].play();
				swap_animation = 2;
				selected = false;
				init_swap(anim, anim_2, main_spr, newpos, selpos);
			}

		}

		if (swap_animation)
		{
			if (!(anim.apply() && anim_2.apply()))
			{
				auto temp = tiles[selpos.y][selpos.x];
				tiles[selpos.y][selpos.x] = tiles[newpos.y][newpos.x];
				tiles[newpos.y][newpos.x] = temp;

				generate_main(main_spr, tiles, *tile_texture);

				update_score = true;

				if (analyze_tiles(tiles))
				{
					swap_animation = 0;
					disappear_animation = 1;

					init_disappear(anim_2, main_spr, tiles, score);
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
				update_score = true;

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
							main_spr[i][k].setScale(1, 1);
							main_spr[i][k].setOrigin(0, 0);
						}
					}
				}

				copy_tiles(tiles, tiles_temp);

				regenerate_tiles(tiles);

				generate_main(main_spr, tiles, *tile_texture);

				if (analyze_tiles(tiles))
				{
					disappear_animation = 1;

					init_disappear(anim_2, main_spr, tiles, score);

				}

			}
		}

		if (update_score)
		{
			texts[1].setString("����\n" + std::to_string(score));
			update_score = false;
		}

		if (frame_counter % 120 == 0 && seconds)
		{
			texts[2].setString("��������\n" + std::to_string(--seconds));
		}


		window.clear(sf::Color(96, 73, 82));

		window.draw(lines);

		for (size_t i = 0; i < 4; i++)
		{
			window.draw(texts[i]);
		}


		for (size_t i = 0; i < 8; i++)
		{
			for (size_t k = 0; k < 8; k++)
			{
				window.draw(main_spr[i][k]);
			}
		}

		if (!seconds && !(swap_animation || disappear_animation || drop_animation))
		{
			if (gameover_once)
			{
				gameover_once = false;
				soundtrack->setVolume(5);

				if (max_score < score)
				{
					resloader.get_userdata()->set_high_score(score);
					resloader.write_userdata();

					victory->play();
				}
				else
				{
					defeat->play();
				}
			}
			if (max_score < score)
			{
				window.draw(spr_victory);
			}
			else
			{
				window.draw(gameover);
			}

			if (!frames_left)
			{
				exit_lock = false;
				window.draw(texts[4]);
			}
			else
			{
				--frames_left;
			}

		}

		window.display();


		delta_time = std::chrono::system_clock::now() - start_time;

		std::this_thread::sleep_for(std::chrono::microseconds(8333 - std::chrono::duration_cast<std::chrono::microseconds>(delta_time).count()));

		++frame_counter;

		//std::cout << std::chrono::duration_cast<std::chrono::microseconds>(delta_time).count() << "\n";
	}

	return 1;
}

void TimeGame::init_lines(sf::VertexArray &lines)
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

void TimeGame::generate_main(sf::Sprite(&main_spr)[8][8], const char(&tiles)[8][8], const sf::Texture &text)
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

bool TimeGame::analyze_tiles(char(&tiles)[8][8])
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

void TimeGame::regenerate_tiles(char(&tiles)[8][8])
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

void TimeGame::generate_tiles(char(&tiles)[8][8])
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

void TimeGame::tilearrpos(sf::Vector2i &selpos, const sf::Vector2i &coords)
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

void TimeGame::init_swap(Animation &a1, Animation &a2, sf::Sprite(&main_spr)[8][8], sf::Vector2i newpos, sf::Vector2i selpos)
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

void TimeGame::init_disappear(Animation &a, sf::Sprite(&main_spr)[8][8], const char(&tiles)[8][8], int &score)
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

void TimeGame::init_drop(Animation &a, sf::Sprite(&main_spr)[8][8], char(&tiles_temp)[8][8])
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

void TimeGame::reverse_swap(Animation &a1, Animation &a2)
{
	a1.set_animation(AnimationType::Move, a1.get_dir(), 30, 2); // No need for reversion of dir; object are swapped in main_spr
	a2.set_animation(AnimationType::Move, a2.get_dir(), 30, 2);
}

void TimeGame::copy_tiles(char(&to)[8][8], const char(&from)[8][8])
{
	for (size_t i = 0; i < 8; i++)
	{
		for (size_t k = 0; k < 8; k++)
		{
			to[i][k] = from[i][k];
		}
	}
}
