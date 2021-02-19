#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <SFML/Graphics/Sprite.hpp>

enum class AnimationDir
{
	Down, Top, Left, Right

};
enum class AnimationType
{
	Move, 
	Scale
};

AnimationDir opposite_dir(const AnimationDir &dir);

class CorrAnimation
{
public:
	void add(sf::Sprite *sprite, AnimationType type, AnimationDir dir, int frames, float delta_per_frame, int delay = 0)
	{
		auto &obj = objects.emplace_back(sprite, type, frames, delay);

		if (type == AnimationType::Move)
		{
			sf::Vector2f off;

			switch (dir)
			{
				case AnimationDir::Down:
				{
					off.x = 0; off.y = 1;
				} break;

				case AnimationDir::Top:
				{
					off.x = 0; off.y = -1;
				} break;

				case AnimationDir::Left:
				{
					off.x = -1; off.y = 0;
				} break;

				case AnimationDir::Right:
				{
					off.x = 1; off.y = 0;
				} break;
			}

			off *= delta_per_frame;

			obj.off = off;

		}
		else if (type == AnimationType::Scale)
		{
			obj.scale = delta_per_frame;
		}

	}
	void clear()
	{
		frame_counter = 0;
		objects.clear();
	}
	bool apply()
	{
		bool to_continue = false;

		for (auto &i : objects)
		{
			if (frame_counter < (i.frames + i.delay) && frame_counter >= i.delay)
			{
				to_continue = true;

				if (i.type == AnimationType::Move)
				{
					i.sprite->move(i.off);
				}
				else if (i.type == AnimationType::Scale)
				{
					i.sprite->scale(i.scale, i.scale);
				}

			}
		}

		++frame_counter;

		return to_continue;
	}

	void repeat()
	{
		frame_counter = 0;
	}

private:

	int frame_counter = 0;

	struct AnimObj
	{
		AnimObj(sf::Sprite *sprite_, AnimationType type_, int frames_, int delay_) :
			sprite{sprite_}, type{type_}, frames{frames_}, delay{delay_}
		{}

		sf::Sprite *sprite;
		AnimationType type;
		const int frames;
		const int delay;
	
		union
		{
			sf::Vector2f off; // for Move
			float scale; // for Disappear
		};

	};

	std::vector<AnimObj> objects;

};

class Animation
{
public:
	bool apply();
	void add(sf::Sprite *sprite);
	void clear();
	void set_animation(AnimationType type, AnimationDir dir, int frames, float delta_per_frame);

	const AnimationDir &get_dir()
	{
		return dir;
	}

private:
	int frames = 0;
	float delta = 0;

	sf::Vector2f off; // for Move
	float opacity; // for Disappear
	int init_frames; // frames from the beginning

	AnimationType type;
	AnimationDir dir;
	std::vector<sf::Sprite*> objects;
};

#endif // !ANIMATION_H
