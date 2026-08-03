#include "Manor.h"

namespace BLOCKMAN
{
	Manor::Manor()
	{

	}


	Manor::~Manor()
	{
		dealloc();
	}

	void Manor::setInfo(ManorInfo* info)
	{
		if (!m_info)
		{
			m_info = new ManorInfo();
		}
		m_info->id = info->id;
		m_info->rose = info->rose;
		m_info->level = info->level;
		m_info->price = info->price;
		m_info->value = info->value;
		m_info->charm = info->charm;
		m_info->houseId = info->houseId;
		m_info->maxLevel = info->maxLevel;
		m_info->seed = info->seed;
		m_info->ground = info->ground;
		m_info->icon = info->icon;
		m_info->manorId = info->manorId;
		m_info->isMaster = info->isMaster;
		m_info->currencyType = info->currencyType;
	}

	void Manor::setNextInfo(ManorInfo * info)
	{
		if (!m_nextInfo)
		{
			m_nextInfo = new ManorInfo();
		}
		m_nextInfo->id = info->id;
		m_nextInfo->rose = info->rose;
		m_nextInfo->level = info->level;
		m_nextInfo->price = info->price;
		m_nextInfo->value = info->value;
		m_nextInfo->charm = info->charm;
		m_nextInfo->houseId = info->houseId;
		m_nextInfo->maxLevel = info->maxLevel;
		m_nextInfo->seed = info->seed;
		m_nextInfo->ground = info->ground;
		m_nextInfo->icon = info->icon;
		m_nextInfo->manorId = info->manorId;
		m_nextInfo->isMaster = info->isMaster;
		m_nextInfo->currencyType = info->currencyType;

	}

	void Manor::addHouse(ManorHouse* house)
	{
		for (ManorHouse* item : m_houses)
		{
			if (item->id == house->id)
			{
				item->id = house->id;
				item->price = house->price;
				item->charm = house->charm;
				item->cdTime = house->cdTime;
				item->unlockLevel = house->unlockLevel;
				item->icon = house->icon;
				item->name = house->name;
				item->isBuy = house->isBuy;
				item->currencyType = house->currencyType;
				return;
			}
		}
		ManorHouse* item = new ManorHouse();
		item->id = house->id;
		item->price = house->price;
		item->charm = house->charm;
		item->cdTime = house->cdTime;
		item->unlockLevel = house->unlockLevel;
		item->icon = house->icon;
		item->name = house->name;
		item->isBuy = house->isBuy;
		item->currencyType = house->currencyType;
		m_houses.push_back(item);
	}

	void Manor::addFurniture(ManorFurniture * info)
	{
		for (ManorFurniture* item : m_furnitures)
		{
			if (item->id == info->id)
			{
				item->id = info->id;
				item->price = info->price;
				item->charm = info->charm;
				item->icon = info->icon;
				item->name = info->name;
				item->operationType = info->operationType;
				item->currencyType = info->currencyType;
				return;
			}
		}
		ManorFurniture* item = new ManorFurniture();
		item->id = info->id;
		item->price = info->price;
		item->charm = info->charm;
		item->icon = info->icon;
		item->name = info->name;
		item->operationType = info->operationType;
		item->currencyType = info->currencyType;
		m_furnitures.push_back(item);
	}

	void Manor::addMessage(ManorMessage * info)
	{
		for (ManorMessage* item : m_messages)
		{
			if (item->id == info->id)
			{
				item->id = info->id;
				item->userId = info->userId;
				item->messageType = info->messageType;
				item->content = info->content;
				item->productId = info->productId;
				item->num = info->num;
				item->userName = info->userName;
				return;
			}
		}
		ManorMessage* item = new ManorMessage();
		item->id = info->id;
		item->userId = info->userId;
		item->messageType = info->messageType;
		item->content = info->content;
		item->num = info->num;
		item->productId = info->productId;
		item->userName = info->userName;
		m_messages.push_back(item);

	}

	void Manor::clearMessage()
	{
		for (ManorMessage* msg : m_messages)
		{
			delete msg;
			msg = nullptr;
		}
		m_messages.clear();
	}

	void Manor::removeMessage(ManorMessage * message)
	{
		delete message;
		m_messages.remove(message);
		message = nullptr;
	}

	void Manor::setHouseSelect(i32 id)
	{
		for (ManorHouse* house : m_houses)
		{
			house->isSelect = house->id == id;
		}
	}

	String Manor::getManorId()
	{
		if (m_info)
		{
			return m_info->manorId;
		}
		return "";
	}

	ManorHouse * Manor::getUsedHouse()
	{
		if (m_info)
		{
			return findHouseById(m_info->houseId);
		}
		return nullptr;
	}

	ManorHouse * Manor::findHouseById(i32 id)
	{
		for (ManorHouse* item : m_houses)
		{
			if (item->id == id || id == 0)
			{
				return item;
			}
		}
		return nullptr;
	}

	ManorFurniture * Manor::findFurnitureById(i32 id)
	{
		for (ManorFurniture* item : m_furnitures)
		{
			if (item->id == id)
			{
				return item;
			}
		}
		return nullptr;
	}
	ManorMessage * Manor::findMessageById(String id)
	{
		for (ManorMessage* item : m_messages)
		{
			if (item->id == id)
			{
				return item;
			}
		}
		return nullptr;
	}

	void Manor::dealloc()
	{
		if (m_info)
		{
			delete m_info;
			m_info = nullptr;
		}
		
		if (m_nextInfo)
		{
			delete m_nextInfo;
			m_nextInfo = nullptr;
		}
		
		for (ManorHouse* item : m_houses)
		{
			delete item;
			item = nullptr;
		}

		for (ManorFurniture* item : m_furnitures)
		{
			delete item;
			item = nullptr;
		}

		for (ManorMessage* msg : m_messages)
		{
			delete msg;
			msg = nullptr;
		}

		m_houses.clear();
		m_furnitures.clear();
		m_messages.clear();
	}

	ManorControl::ManorControl()
	{
	}

	ManorControl::~ManorControl()
	{
		clear();
	}

	void ManorControl::updateOwner(std::vector<ManorOwner> owners)
	{
		m_ownerManors.clear();
		for (auto owner : owners)
		{
			addOwner(owner);
		}
	}

	void ManorControl::addOwner(ManorOwner owner)
	{
		m_ownerManors.push_back(owner);
	}

	void ManorControl::clear()
	{
		m_ownerManors.clear();
	}
}
