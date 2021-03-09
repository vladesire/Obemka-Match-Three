#ifndef ANIMATION_H
#define ANIMATION_H

#include <SFML/Graphics/Sprite.hpp>
#include <vector>

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

class Animation
{
public:
	void add(sf::Sprite *sprite, AnimationType type, AnimationDir dir, int frames, float delta_per_frame, int delay = 0);
	bool apply();
	void clear();
	void repeat();

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

#endif // !ANIMATION_H
