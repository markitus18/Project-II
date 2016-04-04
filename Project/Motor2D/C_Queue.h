#pragma once

#include "j1Timer.h"
#include "C_Player.h"
#include <queue>
class My_Queue
{
public:  
	j1Timer* time;
	
	std::queue<PlayerData*> myqueue;


	My_Queue();
	My_Queue(j1Timer _time){ time = _time; }

	~My_Queue();

	void add(PlayerData* player, j1Timer _time)
	{
		myqueue.push()
	}

private:

};
