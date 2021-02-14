#include "Gameplay.h"
#include "ExitAsk.h"

#include <fstream>

void MovesGame::initialize(const sf::Texture *textures, const sf::SoundBuffer *soundbuffers, sf::Music *music)
{
	std::ifstream fin;
	fin.open("user.data");

	if (!fin.is_open() || !(fin >> max_score))
	{
		fin.clear();
		max_score = 0;
	}

	fin.close();


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


	header.setFont(fonts[1]);
	score.setFont(fonts[0]);
	moves.setFont(fonts[0]);
	target.setFont(fonts[0]);
	gameovertext.setFont(fonts[1]);

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

	header.setPosition(wwpad + 5, whpad + 20);
	score.setPosition(wwpad + 5, whpad + headh);
	moves.setPosition(wwpad + 5, whpad + headh + mainsz / 3);
	target.setPosition(wwpad + 5, whpad + headh + mainsz * 2 / 3);
	gameovertext.setPosition(wwpad + sidew + mainpad + 5, whpad + headh + mainpad + 5);

	header.setString("Игра на время");
	target.setString("Рекорд\n" + std::to_string(max_score));
	gameovertext.setString("Press any key");

	soundtrack = music;
	soundtrack->setLoop(true);
	soundtrack->setVolume(50);

	victory = music + 2;

	kizhak.setBuffer(soundbuffers[13]);
	chetko.setBuffer(soundbuffers[14]);

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

}

int MovesGame::play()
{
	unsigned int frame_counter = 0; // FPS = 120
	unsigned int seconds = 90+1;
	
	bool exit_lock = true;
	int frames_left = 120*5+60; // wait for ending sound 

	std::chrono::system_clock::time_point start_time;
	std::chrono::system_clock::duration delta_time;

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
			else if (!exit_lock && (!seconds && !(swap_animation || disappear_animation || drop_animation)) && (event.type == sf::Event::MouseButtonReleased || event.type == sf::Event::KeyReleased))
			{
				soundtrack->stop();
				victory->stop();
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

				generate_main(main_spr, tiles, *tile_texture);

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

					init_disappear(anim_2, main_spr, tiles, iscore);

				}

			}
		}

		if (state_changed)
		{
			score.setString("Счет\n" + std::to_string(iscore));
			state_changed = false;
		}

		if (frame_counter % 120 == 0 && seconds)
		{
			moves.setString("Осталось\n" + std::to_string(--seconds));
		}


		window.clear(sf::Color(96, 73, 82));

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

		if (!seconds && !(swap_animation || disappear_animation || drop_animation))
		{
			if (gameover_once)
			{
				gameover_once = false;
				soundtrack->setVolume(10);
				
				if (max_score < iscore)
				{
					std::ofstream fout;
					fout.open("user.data");

					/*soundtrack->stop();
					victory->play();*/

					chetko.play();

					fout << iscore;
					fout.close();
				}
				else
				{
					kizhak.play();
				}
			}
			if (max_score < iscore)
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
				window.draw(gameovertext);
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