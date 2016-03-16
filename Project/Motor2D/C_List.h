#ifndef __C_List_H__
#define __C_List_H__

#include "Defs.h"
#include <list>


/**
* Contains items from double linked list
*/
template<class tdata>
struct C_List_item
{
	tdata                 data;
	C_List_item<tdata>*   next;
	C_List_item<tdata>*   prev;

	inline C_List_item(const tdata& _data)
	{
		data = _data;
		next = prev = NULL;
	}

	~C_List_item()
	{}
};

/**
* Manages a double linked list
*/
template<class tdata>
class C_List
{

public:

	C_List_item<tdata>*   start;
	C_List_item<tdata>*   end;

private:

	unsigned int  size;

public:

	/**
	* Constructor
	*/
	inline C_List()
	{
		start = end = NULL;
		size = 0;
	}

	/**
	* Destructor
	*/
	~C_List()
	{
		clear();
	}

	/**
	* Get Size
	*/
	unsigned int count() const
	{
		return size;
	}

	/**
	* Add new item
	*/
	C_List_item<tdata>* add(const tdata& item)
	{
		C_List_item<tdata>*   p_data_item;
		p_data_item = new C_List_item < tdata >(item);

		if(start == NULL)
		{
			start = end = p_data_item;
		}
		else
		{
			p_data_item->prev = end;
			end->next = p_data_item;
			end = p_data_item;
		}

		++size;
		return(p_data_item);
	}

	/**
	* Deletes an item from the list
	*/
	bool del(C_List_item<tdata>* item)
	{
		if(item == NULL)
		{
			return (false);
		}

		// Now reconstruct the list
		if(item->prev != NULL)
		{
			item->prev->next = item->next;

			if(item->next != NULL)
			{
				item->next->prev = item->prev;
			}
			else
			{
				end = item->prev;
			}
		}
		else
		{
			if(item->next)
			{
				item->next->prev = NULL;
				start = item->next;
			}
			else
			{
				start = end = NULL;
			}
		}

		RELEASE(item);
		--size;
		return(true);
	}

	bool transfer(C_List_item<tdata>* item, C_List& dst)
	{
		if (item == NULL)
		{
			return NULL;
		}
		if (item->prev != NULL)
		{
			item->prev->next = item->next;

			if (item->next != NULL)
			{
				item->next->prev = item->prev;
			}
			else
			{
				end = item->prev;
			}
		}
		else
		{
			if (item->next)
			{
				item->next->prev = NULL;
				start = item->next;
			}
			else
			{
				start = end = NULL;
			}
		}
		--size;
		dst.add(item->data);
		return true;
	}
	/**
	* Destroy and free all mem
	*/
	void clear()
	{
		C_List_item<tdata>*   p_data;
		C_List_item<tdata>*   p_next;
		p_data = start;

		while(p_data != NULL)
		{
			p_next = p_data->next;
			RELEASE(p_data);
			p_data = p_next;
		}

		start = end = NULL;
		size = 0;
	}

	/**
	* read / write operator access directly to a position in the list
	*/
	tdata& operator  [](const unsigned int index)
	{
		long                  pos;
		C_List_item<tdata>*   p_item;
		pos = 0;
		p_item = start;

		while(p_item != NULL)
		{
			if(pos == index)
			{
				break;
			}

			++pos;
			p_item = p_item->next;
		}

		return(p_item->data);
	}

	/**
	* const read operator access directly to a position in the list
	*/
	const tdata& operator  [](const unsigned int index) const
	{
		long                  pos;
		C_List_item<tdata>*   p_item;
		pos = 0;
		p_item = start;

		while(p_item != NULL)
		{
			if(pos == index)
			{
				break;
			}

			++pos;
			p_item = p_item->next;
		}

		ASSERT(p_item);

		return(p_item->data);
	}

	/**
	* const read operator access directly to a position in the list
	*/
	const C_List<tdata>& operator +=(const C_List<tdata>& other_list)
	{
		C_List_item<tdata>*   p_item = other_list.start;

		while(p_item != NULL)
		{
			add(p_item->data);
			p_item = p_item->next;
		}

		return(*this);
	}

	/**
	* const access to a node in a position in the list
	*/
	const C_List_item<tdata>* At(unsigned int index) const
	{
		long                  pos = 0;
		C_List_item<tdata>*   p_item = start;

		while(p_item != NULL)
		{
			if(pos++ == index)
				break;

			p_item = p_item->next;
		}

		return p_item;
	}

	/**
	* access to a node in a position in the list
	*/
	C_List_item<tdata>* At(unsigned int index)
	{
		long                  pos = 0;
		C_List_item<tdata>*   p_item = start;

		while(p_item != NULL)
		{
			if(pos++ == index)
				break;

			p_item = p_item->next;
		}

		return p_item;
	}

	// Sort
	int BubbleSort()
	{
		int ret = 0;
		bool swapped = true;

		while(swapped)
		{
			swapped = false;
			C_List_item<tdata>* tmp = start;

			while(tmp != NULL && tmp->next != NULL)
			{
				++ret;
				if(tmp->data > tmp->next->data)
				{
					SWAP(tmp->data, tmp->next->data);
					swapped = true;
				}

				tmp = tmp->next;
			}
		}

		return ret;
	}

	/**
	* returns the first apperance of data as index (-1 if not found)
	*/
	int find(const tdata& data)
	{
		C_List_item<tdata>* tmp = start;
		int index = 0;

		while(tmp != NULL)
		{
			if(tmp->data == data)
				return(index);

			++index;
			tmp = tmp->next;
		}
		return (-1);
	}

	void Insert(C_List_item<tdata>* prevItem, C_List_item<tdata>* newItem)
	{
		if (!start)
		{
			start = end = newItem;
		}
		else
		{
			if (!prevItem)
			{
				start->prev = newItem;
				newItem->next = start;
				start = newItem;
			}
			else
			{
				if (prevItem->next)
					prevItem->next->prev = newItem;
				else
					end = newItem;
				newItem->next = prevItem->next;
				prevItem->next = newItem;
				newItem->prev = prevItem;
			}	
		}
			size++;
	}

	void InsertAfter(uint position, const C_List<tdata>& list)
	{
		C_List_item<tdata>* p_my_list = At(position);
		C_List_item<tdata>* p_other_list = list.start;

		while(p_other_list != NULL)
		{
			C_List_item<tdata>* p_new_item = new C_List_item<tdata>(p_other_list->data);

			p_new_item->next = (p_my_list) ? p_my_list->next : NULL;

			if(p_new_item->next != NULL)
				p_new_item->next->prev = p_new_item;
			else
				end = p_new_item;

			p_new_item->prev = p_my_list;
				
			if(p_new_item->prev != NULL)
				p_new_item->prev->next = p_new_item;
			else
				start = p_new_item;

			p_my_list = p_new_item;
			p_other_list = p_other_list->next;
		}
	}
};
#endif /*__C_List_H__*/