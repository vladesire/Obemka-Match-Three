#ifndef RES_LOADER_H_
#define RES_LOADER_H_

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>

class ResLoader
{
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

		}
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
	}

private:
	sf::Font *fonts;
	sf::Music *music;
	sf::SoundBuffer *soundbuffers;
	sf::Texture *textures;
	bool loaded = false;
};


#endif // !RES_LOADER_H_
