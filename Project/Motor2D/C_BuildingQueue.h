#ifndef __C_BUILDINGQUEUE_H__
#define __C_BUILDINGQUEUE_H__

#include "j1Timer.h"
#include "Unit.h"
#include "M_EntityManager.h"
#include <vector>

class C_BuildingQueue
{
public:  
	std::list<Unit_Type> units;
	std::list<int> timers;
	std::list<int> psiList;
	int count = 0;
	j1Timer timer;
	bool stopped = true;

	C_BuildingQueue()
	{}

	bool Update()
	{
		if (!stopped)
		{
			if (!units.empty())
			{
				if (timer.ReadSec() >= (*timers.begin()))
				{
					Stop();
					return true;
				}
			}
			return false;
		}
		else
		{
			if (count)
			{
				Start();
			}
		}
		return false;
	}
	
	void Add(Unit_Type unit, float time, int psi)
	{
		if (count < 5)
		{
			count++;

			units.push_back(unit);
			timers.push_back(time);
			psiList.push_back(psi);
			if (count == 1)
			{
				Start();
			}

		}
	}

	int Remove(int position)
	{
		int ret = 0;

		if (position < units.size())
		{
			int i = 0;
			std::list<Unit_Type>::iterator it = units.begin();
			std::list<int>::iterator timer_it = timers.begin();
			std::list<int>::iterator psi_it = psiList.begin();

			while (it != units.end())
			{
				if (i == position)
				{
					ret = (*psi_it);

					units.remove(*it);
					timers.remove(*timer_it);
					psiList.remove(*psi_it);
				}

				i++;
				it++;
				timer_it++;
			}
			count--;
		}
		return ret;
	}

	Unit_Type Pop()
	{
		count--;
		Unit_Type ret = (*units.begin());

		units.erase(units.begin());
		timers.erase(timers.begin());
		psiList.erase(psiList.begin());

		timer.Start();

		return ret;
	}

	void Stop()
	{
		stopped = true;
	}

	void Start()
	{
		if (count)
		{
			stopped = false;
			timer.Start();
		}
	}
};

#endif // __C_BUILDINGQUEUE_H__
