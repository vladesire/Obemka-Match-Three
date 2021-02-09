#include "Animation.h"

void Animation::set_animation(AnimationType antype, AnimationDir andir, int anframes, float andelta)
{
	type = antype;
	dir = andir;
	frames = init_frames = anframes;
	delta = andelta;

	if (type == AnimationType::Move)
	{
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

		off *= delta;

	}
	else if (type == AnimationType::Disappear)
	{
		opacity = 255.0 / anframes;
	}

}

void Animation::add(sf::Sprite *sprite)
{
	objects.push_back(sprite);
}

void Animation::clear()
{
	objects.clear();
}

bool Animation::apply()
{
	if (!frames)
	{
		return false;
	}
	
	if (type == AnimationType::Move)
	{
		for (auto i : objects)
		{
			i->move(off);
		}
	}
	else if (type == AnimationType::Disappear)
	{
		for (auto i : objects)
		{
			i->scale(delta, delta);
		}
	}
	
	--frames;
	return true;
}

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