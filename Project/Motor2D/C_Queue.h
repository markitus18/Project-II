#pragma once

#include "j1Timer.h"
#include "Unit.h"
#include <queue>
class My_Queue
{
public:  
	std::queue<float> my_time;
	
	std::queue<Unit_Type> my_queue;

	j1Timer inicial_time;

	My_Queue();

	~My_Queue();

	

	bool IsCreated()
	{
		float new_time = my_time.back();
		if (new_time == inicial_time.ReadSec())
		{
			my_queue.back();
			return true;
		}
		return false;
	}
	
	void Add(Unit_Type unit, float _time)
	{
		my_queue.push(unit);
		my_time.push(_time);

		if (IsCreated() == true)
		{
			inicial_time.Start();
		}
	}
	
	
private:

};
