#pragma once

#include "j1Timer.h"
#include "C_Player.h"
#include <queue>
class My_Queue
{
public:  
	std::queue<float> mytime;
	
	std::queue<PlayerData*> myqueue;

	j1Timer inicial_time;

	My_Queue();

	~My_Queue();

	void add(PlayerData* player, float _time)
	{
		myqueue.push(player);
		mytime.push(_time);

	}

	bool is_Created()
	{
		float new_time = mytime.back();
		if (new_time == inicial_time.ReadSec())
		{
			myqueue.back();
			return true;
		}
		return false;
	}

	
	
private:

};
