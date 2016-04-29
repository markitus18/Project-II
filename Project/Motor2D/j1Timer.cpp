// ----------------------------------------------------
// j1Timer.cpp
// Fast timer with milisecons precision
// ----------------------------------------------------

#include "j1Timer.h"
#include "SDL\include\SDL_timer.h"

// ---------------------------------------------
j1Timer::j1Timer()
{
	Start();
}

// ---------------------------------------------
void j1Timer::Start()
{
	stopped = false;
	started_at = SDL_GetTicks();
}

// ---------------------------------------------
uint32 j1Timer::Read() const
{
	if (stopped)
		return 0;
	return SDL_GetTicks() - started_at;
}

// ---------------------------------------------
float j1Timer::ReadSec() const
{
	if (stopped)
		return 0;
	return float(SDL_GetTicks() - started_at) / 1000.0f;
}

void j1Timer::Stop()
{
	stopped = true;
}

bool j1Timer::IsStopped() const
{
	return stopped;
}