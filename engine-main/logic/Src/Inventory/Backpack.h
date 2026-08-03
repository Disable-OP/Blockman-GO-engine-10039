#ifndef __BACKPACK_HEADER__
#define __BACKPACK_HEADER__

#include "BM_TypeDef.h"

namespace BLOCKMAN
{
	class Backpack : public ObjectAlloc 
	{
	private:
		int capacity = -1;
		int maxCapacity = -1;
	public:
		Backpack();
		~Backpack();
		int getCapacity() { return capacity; }
		int getMaxCapacity() { return maxCapacity; }
		void setCapacity(int capacity) { this->capacity = capacity; }
		void setMaxCapacity(int capacity) { maxCapacity = capacity; }
		void addCapacity(int capacity);
		void subCapacity(int capacity);
	};
}

#endif // __BACKPACK_HEADER__