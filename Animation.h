#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <SFML/Graphics/Sprite.hpp>

enum class AnimationDir
{
	Down, Top, Left, Right

};

AnimationDir opposite_dir(const AnimationDir &dir);


enum class AnimationType
{
	Move, 
	Disappear
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
