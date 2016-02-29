// ----------------------------------------------------
// Array that resizes dynamically   -------------------
// ----------------------------------------------------

#include<array>

#ifndef __P2DYNARRAY_H__
#define __P2DYNARRAY_H__

#include "Defs.h"
#include <vector>

template<class VALUE>
class C_DynArray
{
private:

	std::vector<VALUE> data;

public:

	// Constructors
	C_DynArray()
	{
	}

	C_DynArray(unsigned int capacity) : data(capacity)
	{
	}

	// Destructor
	~C_DynArray()
	{
	}

	// Operators
	VALUE& operator[](unsigned int index)
	{
		if (index >= 0 && index < data.size())
		{
			return data[index];
		}
	}

	const VALUE& operator[](unsigned int index) const
	{
		if (index >= 0 && index < data.size())
		{
			return data[index];
		}
	}

	const C_DynArray<VALUE>& operator+= (const C_DynArray<VALUE>& array)
	{
		for (uint n = 0; n < array.Count(); n++)
		{
			data.push_back(array[n]);
		}

		return(*this);
	}

	// Data Management
	void PushBack(const VALUE& element)
	{
		data.push_back(element);
	}

	bool Pop(VALUE& result)
	{
		result = data.pop_back();
		return false;
	}

	void Clear()
	{
		data.clear();
	}

	bool Insert(const VALUE& element, unsigned int position)
	{

		data.insert(position, element);

		return true;
	}

	bool Insert(const C_DynArray<VALUE>& array, unsigned int position)
	{
		data.insert(position, array.GetData());

		return true;
	}

	VALUE* At(unsigned int index)
	{
		return &data[index];
	}

	const VALUE* At(unsigned int index) const
	{
		return &data[index];
	}

	// Utils
	unsigned int GetCapacity() const
	{
		return data.capacity();
	}

	unsigned int Count() const
	{
		return data.size();
	}

	const std::vector<VALUE>* GetData() const
	{
		return *data;
	}

	// Sort
	/*int BubbleSort()
	{
		int ret = 0;
		bool swapped = true;

		while(swapped)
		{
			swapped = false;
			for(unsigned int i = 0; i < num_elements - 2; ++i)
			{
				++ret;
				if(data[i] > data[i + 1])
				{
					SWAP(data[i], data[i + 1]);
					swapped = true;
				}
			}
		}

		return ret;
	}


	int BubbleSortOptimized()
	{
		int ret = 0;
		unsigned int count;
		unsigned int last = num_elements - 2;

		while(last > 0)
		{
			count = last;
			last = 0;
			for(unsigned int i = 0; i < count; ++i)
			{
				++ret;
				if(data[i] > data[i + 1])
				{
					SWAP(data[i], data[i + 1]);
					last = i;
				}
			}
		}
		return ret;
	}


	int CombSort()
	{
		int ret = 0;
		bool swapped = true;
		int gap = num_elements - 1;
		float shrink = 1.3f;

		while(swapped || gap > 1)
		{
			gap = MAX(1, gap / shrink);

			swapped = false;
			for(unsigned int i = 0; i + gap < num_elements - 1; ++i)
			{
				++ret;
				if(data[i] > data[i + gap])
				{
					SWAP(data[i], data[i + gap]);
					swapped = true;
				}
			}
		}

		return ret;
	}*/

	void Flip()
	{
		VALUE* start = &data[0];
		VALUE* end = &data[num_elements-1];

		while(start < end)
			SWAP(*start++, *end--);
	}
};

#endif // __P2DYNARRAY_H__