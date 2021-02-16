#ifndef RES_LOADER_H_
#define RES_LOADER_H_

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>

#include <iostream>

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

public:
	void load()
	{
		if (!loaded)
		{
			fonts = new sf::Font[2];

			fonts[0].loadFromFile("Fonts/RobotoRegular.ttf");
			fonts[1].loadFromFile("Fonts/RobotoBold.ttf");


			music = new sf::Music[3];

			music[0].openFromFile("Sounds/soundtrack.wav");
			music[1].openFromFile("Sounds/main_menu.wav");
			music[2].openFromFile("Sounds/scorebeaten.wav");
			music[0].setLoop(true);
			music[1].setLoop(true);


			soundbuffers = new sf::SoundBuffer[16];

			soundbuffers[0].loadFromFile("Sounds/obeme_1.wav");
			soundbuffers[1].loadFromFile("Sounds/obeme_2.wav");
			soundbuffers[2].loadFromFile("Sounds/obeme_3.wav");
			soundbuffers[3].loadFromFile("Sounds/obeme_4.wav");
			soundbuffers[4].loadFromFile("Sounds/obeme_5.wav");
			soundbuffers[5].loadFromFile("Sounds/gorin_1.wav");
			soundbuffers[6].loadFromFile("Sounds/povar_1.wav");
			soundbuffers[7].loadFromFile("Sounds/pocik_1.wav");
			soundbuffers[8].loadFromFile("Sounds/sasxri_1.wav");
			soundbuffers[9].loadFromFile("Sounds/sasxri_2.wav");
			soundbuffers[10].loadFromFile("Sounds/sasxri_3.wav");
			soundbuffers[11].loadFromFile("Sounds/sasxri_4.wav");
			soundbuffers[12].loadFromFile("Sounds/sasxri_5.wav");
			soundbuffers[13].loadFromFile("Sounds/kizhak.wav");
			soundbuffers[14].loadFromFile("Sounds/chetko.wav");


			textures = new sf::Texture[3];

			textures[0].loadFromFile("Textures/tiles.png");
			textures[1].loadFromFile("Textures/gameover.jpg");
			textures[2].loadFromFile("Textures/chetko.jpg");

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
