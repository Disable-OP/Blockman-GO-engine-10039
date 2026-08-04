#include "Backpack.h"
#include "Math/LordMath.h"

namespace BLOCKMAN
{
	Backpack::Backpack()
	{
	}

	Backpack::~Backpack()
	{
	}

	void Backpack::addCapacity(int capacity)
	{
		capacity = Math::Abs(capacity);
		if (this->maxCapacity == -1) // no max
			this->capacity = this->capacity + capacity;
		else
			this->capacity = this->maxCapacity > this->capacity + capacity ? this->capacity + capacity : this->maxCapacity;
	}

	void Backpack::subCapacity(int capacity)
	{
		capacity = Math::Abs(capacity);
		this->capacity = this->capacity - capacity > 0 ? this->capacity - capacity : 0;
	}
}