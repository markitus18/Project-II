#pragma once

#include "M_Render.h"

class Animation
{
public:
	float speed;
	bool loop;
	C_DynArray<SDL_Rect> frames;

private:
	float current_frame;
	int loops;

public:
	Animation() : frames(5), speed(1.0f), current_frame(0), loop(true), loops(0)
	{}

	Animation(const Animation& a) : frames(a.frames), speed(a.speed), current_frame(0), loop(a.loop), loops(0)
	{}

	SDL_Rect& GetCurrentFrame(float dt)
	{
		current_frame += speed * dt;
		if (current_frame >= frames.Count())
		{
			current_frame = (loop) ? 0.0f : frames.Count() - 1;
			loops++;
		}

		return frames[(int)current_frame];
	}

	const SDL_Rect& PeekCurrentFrame() const
	{
		return frames[(int)current_frame];
	}

	bool Finished()
	{
		return loops > 0;
	}

	void Reset()
	{
		current_frame = 0;
	}

	void ResetLoops()
	{
		loops = 0;
	}
};