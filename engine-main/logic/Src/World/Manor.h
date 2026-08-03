#pragma once
#ifndef __MANOR_HEADER__
#define __MANOR_HEADER__

#include "BM_TypeDef.h"
#include "Inventory/Wallet.h"
#include "Network/protocol/PacketDefine.h"
#include <list>

namespace BLOCKMAN
{

	enum class OperationType
	{
		BUY,
		USE,
		RECYCLE
	};

	class ManorOwner
	{
	public:
		i32 index = 0;
		ui64 ownerId = 0;
		Vector3i origin = Vector3i::ZERO;
		Vector3i termini = Vector3i::ZERO;
		DEFINE_NETWORK_SERIALIZER(index, ownerId, origin, termini);
	};

	class ManorInfo 
	{
	public:
		i32 id = 0;
		i32 rose = 0;
		i32 level = 1;
		i32 price = 0;
		i32 value = 0;
		i32 charm = 0;
		i32 houseId = 0;
		i32 maxLevel = 0;
		String seed = "";
		String ground = "";
		String icon = "";
		String manorId = "";
		bool isMaster = false;
		CurrencyType currencyType = CurrencyType::PLATFORM_MONEY;
		DEFINE_NETWORK_SERIALIZER(id, rose, level, price, value, charm, houseId, maxLevel, seed, ground, icon, manorId, isMaster, currencyType);
		ManorInfo() {};
	};

	class ManorHouse
	{
	public:
		i32 id = 0;
		i32 price = 0;
		i32 cdTime = 0;
		i32 charm = 0;
		i32 unlockLevel = 0;
		String icon = "";
		String name = "";
		bool isBuy = false;
		bool isSelect = false;
		CurrencyType currencyType = CurrencyType::PLATFORM_MONEY;

		DEFINE_NETWORK_SERIALIZER(id, price, cdTime, charm, unlockLevel, icon, name, isBuy, currencyType);
		ManorHouse() {}
	};

	class ManorFurniture
	{
	public:
		i32 id = 0;
		i32 price = 0;
		i32 charm = 0;
		String icon = "";
		String name = "";
		OperationType operationType = OperationType::BUY;
		CurrencyType currencyType = CurrencyType::PLATFORM_MONEY;

		DEFINE_NETWORK_SERIALIZER(id, price, charm, icon, name, operationType, currencyType);
		ManorFurniture() {}
	};

	class ManorMessage 
	{
	public:
		String id = "";
		i32 num = 0;
		ui64 userId = 0;
		i32 productId = 0;
		i32 messageType = 0;
		String content = "";
		String userName = "";
		DEFINE_NETWORK_SERIALIZER(id, num, userId, productId, messageType, content, userName);
		ManorMessage() {}
	};

	class Manor : public ObjectAlloc
	{
	public:
		Manor();
		~Manor();

	public:
		void setInfo(ManorInfo* info);
		void setNextInfo(ManorInfo* info);
		void addHouse(ManorHouse* house);
		void addFurniture(ManorFurniture* furniture);
		void addMessage(ManorMessage* message);
		void clearMessage();
		void removeMessage(ManorMessage* message);
		void setHouseSelect(i32 id);

		ManorInfo* getInfo()  { return m_info; }
		ManorInfo* getNextInfo() { return m_nextInfo; } 
		list<ManorHouse*>::type  getHouses() { return m_houses; }
		list<ManorFurniture*>::type getFurnitures() { return m_furnitures; }
		list<ManorMessage*>::type getMessages() { return m_messages; }

		String getManorId();
		ManorHouse* getUsedHouse();
		ManorHouse* findHouseById(i32 id);
		ManorFurniture* findFurnitureById(i32 id);
		ManorMessage* findMessageById(String id);

	public:
		void dealloc();

	private:
		ManorInfo* m_info = nullptr;
		ManorInfo* m_nextInfo = nullptr;

		list<ManorHouse*>::type m_houses;
		list<ManorFurniture*>::type m_furnitures;
		list<ManorMessage*>::type m_messages;
	};
	
	class ManorControl : public ObjectAlloc
	{
	public:
		ManorControl();
		~ManorControl();

	public:
		void updateOwner(std::vector<ManorOwner> owners);
		list<ManorOwner>::type getOwners() { return m_ownerManors; }

	private:
		void addOwner(ManorOwner owner);
		void clear();

	private:
		list<ManorOwner>::type m_ownerManors;
	};
};

#endif