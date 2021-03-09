#include "Animation.h"

AnimationDir opposite_dir(const AnimationDir &dir)
{
	switch (dir)
	{
		case AnimationDir::Down:
			return AnimationDir::Top;

		case AnimationDir::Top:
			return AnimationDir::Down;

		case AnimationDir::Left:
			return AnimationDir::Right;

		case AnimationDir::Right:
			return AnimationDir::Left;

	}
}

void Animation::add(sf::Sprite *sprite, AnimationType type, AnimationDir dir, int frames, float delta_per_frame, int delay)
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
bool Animation::apply()
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
void Animation::clear()
{
	frame_counter = 0;
	objects.clear();
}
void Animation::repeat()
{
	frame_counter = 0;
}