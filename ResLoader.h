#ifndef RES_LOADER_H_
#define RES_LOADER_H_

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>
#include <vector>

/*
	Resources order

	Music:
		Main menu (1)
		Game soundtrack (var)
		Victory music (var)
		Defeat music (var)

	Sounds:
		Tile_1 (var)
		Tile_2 (var)
		Tile_3 (var)
		Tile_4 (var)
		Tile_5 (var)

	Textures:
		tiles (1)
		victory (var)
		defeat (var)

*/



class ResLoader
{
public:
	class UserData
	{
	public:
		int get_high_score()
		{
			return high_score;
		}
		int get_sounds_volume()
		{
			return sounds_volume;
		}
		int get_music_volume()
		{
			return music_volume;
		}

		void set_high_score(int score)
		{
			high_score = (score > 0) ? score : 0;
		}
		void set_sounds_volume(int volume)
		{
			if (volume < 0)
				sounds_volume = 0;
			else if (volume > 100)
				sounds_volume = 100;
			else
				sounds_volume = volume / 10 * 10;
		}
		void set_music_volume(int volume)
		{
			if (volume < 0)
				music_volume = 0;
			else if (volume > 100)
				music_volume = 100;
			else
				music_volume = volume / 10 * 10;
		}

		void add_sounds()
		{
			if (sounds_volume < 100)
				sounds_volume += 10;
		}
		void sub_sounds()
		{
			if (sounds_volume > 0)
				sounds_volume -= 10;
		}
		void add_music()
		{
			if (music_volume < 100)
				music_volume += 10;
		}
		void sub_music()
		{
			if (music_volume > 0)
				music_volume -= 10;
		}

	private:
		int high_score;
		int sounds_volume;
		int music_volume;
	};

private:
	struct CfgParser
	{
		void parse()
		{
			std::ifstream fin;

			fin.open("resources.cfg");

			if (!fin.is_open())
			{
				// Well, just no music, sounds and texures
			}

			std::string templine;

			std::getline(fin, templine);
			get_paths(tiles_texture, templine);

			std::getline(fin, templine);
			get_paths(victory_textures, templine);

			std::getline(fin, templine);
			get_paths(defeat_textures, templine);

			std::getline(fin, templine);
			get_paths(music_menu, templine);

			std::getline(fin, templine);
			get_paths(music_game, templine);

			std::getline(fin, templine);
			get_paths(music_victory, templine);

			std::getline(fin, templine);
			get_paths(music_defeat, templine);

			for (size_t i = 0; i < 5; i++)
			{
				std::getline(fin, templine);
				get_paths(snd_tile[i], templine);
			}

			fin.close();
		}

		std::string tiles_texture;
		std::string music_menu;
		std::vector<std::string> victory_textures;
		std::vector<std::string> defeat_textures;
		std::vector<std::string> music_game;
		std::vector<std::string> music_victory;
		std::vector<std::string> music_defeat;
		std::vector<std::string> snd_tile[5];

	private:
		void get_paths(std::vector<std::string> &ready, std::string &from)
		{
			int beg = 0, end = 0;

			while (true)
			{
				beg = from.find_first_of('\"', end + 1);

				if (beg == std::string::npos)
				{
					return;
				}

				end = from.find_first_of('\"', beg + 1);

				ready.push_back(from.substr(beg + 1, end - beg - 1)); // Quotes should be omitted
			}
		}
		void get_paths(std::string &ready, std::string &from)
		{
			int beg = 0, end = 0;

			beg = from.find_first_of('\"', end);

			if (beg == std::string::npos)
			{
				return;
			}

			end = from.find_first_of('\"', beg + 1);

			ready = from.substr(beg + 1, end - beg - 1); // Quotes should be omitted
		}

	};

public:
	struct CfgParser *config;

	void load()
	{
		if (!loaded)
		{
			fonts = new sf::Font[2];

			fonts[0].loadFromFile("Fonts/RobotoRegular.ttf");
			fonts[1].loadFromFile("Fonts/RobotoBold.ttf");


			config = new CfgParser;
			config->parse();

			int size = config->music_defeat.size() + config->music_game.size() + config->music_victory.size() + 1;
			music = new sf::Music[size];
			
			music[0].openFromFile(config->music_menu);
			music[0].setLoop(true);

			int off = 1;
			for (size_t i = 0, j = i + off; i < config->music_game.size(); ++i, ++j)
			{
				music[j].openFromFile(config->music_game[i]);
				music[j].setLoop(true);
			}

			off += config->music_game.size();
			for (size_t i = 0, j = i + off; i < config->music_victory.size(); ++i, ++j)
			{
				music[j].openFromFile(config->music_victory[i]);
			}

			off += config->music_victory.size();
			for (size_t i = 0, j = i + off; i < config->music_defeat.size(); ++i, ++j)
			{
				music[j].openFromFile(config->music_defeat[i]);
			}

			size = 0;
			for (size_t i = 0; i < 5; i++)
			{
				size += config->snd_tile[i].size();
			}
			
			soundbuffers = new sf::SoundBuffer[size];

			off = 0;
			for (size_t i = 0; i < 5; i++)
			{
				for (size_t k = 0, x = k + off; k < config->snd_tile[i].size(); ++k, ++x)
				{
					soundbuffers[x].loadFromFile(config->snd_tile[i][k]);
				}
				
				off += config->snd_tile[i].size();
			}

			size = config->defeat_textures.size() + config->victory_textures.size() + 1;
			textures = new sf::Texture[size];

			textures[0].loadFromFile(config->tiles_texture);

			off = 1;
			for (size_t i = 0, j = i + off; i < config->victory_textures.size(); ++i, ++j)
			{
				textures[j].loadFromFile(config->victory_textures[i]);
			}

			off += config->victory_textures.size();
			for (size_t i = 0, j = i + off; i < config->defeat_textures.size(); ++i, ++j)
			{
				textures[j].loadFromFile(config->defeat_textures[i]);
			}

			userdata = new UserData;
			read_userdata();
		}
	}

	void write_userdata()
	{
		std::ofstream fout;

		fout.open("User/user.data", std::ios::trunc);

		if (!fout.is_open())
		{
			return;
		}

		fout << userdata->get_high_score() << "\n";
		fout << userdata->get_sounds_volume() << "\n";
		fout << userdata->get_music_volume() << "\n";

		fout.close();
	}
	void read_userdata()
	{
		std::ifstream fin;

		fin.open("User/user.data");

		if (!fin.is_open())
		{
			userdata->set_high_score(0);
			userdata->set_sounds_volume(50);
			userdata->set_music_volume(50);
			return;
		}

		int data;
		
		fin >> data;
		userdata->set_high_score(data);

		fin >> data;
		userdata->set_sounds_volume(data);

		fin >> data;
		userdata->set_music_volume(data);

		fin.close();
	}
	UserData *get_userdata()
	{
		return userdata;
	}

	const sf::Font *get_fonts()
	{
		return fonts;
	}

	sf::Music *get_music()
	{
		return music;
	}

	sf::SoundBuffer *get_soundbuffers()
	{
		return soundbuffers;
	}

	const sf::Texture *get_textures()
	{
		return textures;
	}
 
	~ResLoader()
	{
		delete[] fonts;
		delete[] music;
		delete[] soundbuffers;
		delete[] textures;
		delete userdata;
		delete config;
	}

private:
	sf::Font *fonts;
	sf::Music *music;
	sf::SoundBuffer *soundbuffers;
	sf::Texture *textures;
	bool loaded = false;

	UserData *userdata;
};


#endif // !RES_LOADER_H_
