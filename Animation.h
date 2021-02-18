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
	Disappear
};

AnimationDir opposite_dir(const AnimationDir &dir);


class CorrAnimation
{
public:
	void add(sf::Sprite *sprite, AnimationType type, AnimationDir dir, int frames, float delta_per_frame)
	{
		auto &obj = objects.emplace_back(sprite, type, frames);

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
		else if (type == AnimationType::Disappear)
		{
			obj.scale = 255.0 / frames;
		}
	}
	void clear()
	{
		objects.clear();
	}
	bool apply()
	{
		bool to_continue = false;

		for (auto &i : objects)
		{
			if (i.frames)
			{
				to_continue = true;

				if (i.type == AnimationType::Move)
				{
					i.sprite->move(i.off);
				}
				else if (i.type == AnimationType::Disappear)
				{
					i.sprite->scale(i.scale, i.scale);
				}

				--i.frames;
			}
		}

		return to_continue;
	}

	void repeat()
	{
		for (auto &i : objects)
		{
			i.frames = i.init_frames;
		}
	}

private:

	struct AnimObj
	{
		AnimObj(sf::Sprite *sprite_, AnimationType type_, int frames_) :
			sprite{sprite_}, type{type_}, init_frames{frames_}, frames{frames_}
		{}

		sf::Sprite *sprite;
		AnimationType type;
		const int init_frames;
		int frames;
	
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
