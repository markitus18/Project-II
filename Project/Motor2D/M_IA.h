#ifndef __IA_H__
#define __IA_H__

#include "j1Module.h"

class M_IA : public j1Module
{
public:
	M_IA(bool);
	~M_IA(){};

	bool Start();

	bool Update(float dt);

	bool CleanUp();

private:

};

#endif //__MISSIL_H__