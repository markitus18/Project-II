#ifndef __j1TIMER_H__
#define __j1TIMER_H__

#include "Defs.h"

class j1Timer
{
public:

	// Constructor
	j1Timer();

	void Start();
	uint32 Read() const;
	float ReadSec() const;
	void Stop();
	bool IsStopped() const;

private:
	uint32	started_at;
	bool	stopped = true;
};

#endif //__j1TIMER_H__