/********************************************************************
filename: 	UFactory
file path:	H:\sandboxol\client\blockmango-client\dev\common\Src\Util\UFactory.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/20
*********************************************************************/
#ifndef __UFACTORY_H__
#define __UFACTORY_H__
#include <map>

template<typename BaseType, typename SubType>
class Creater
{
public:
	static BaseType * create()
	{
		return LordNew SubType();
	}	
};

template<typename BaseType, typename KeyType>
class UFactory
{ 
public:  
	UFactory() {}  
	virtual ~UFactory() {}  
	UFactory(UFactory&);  

public:  
	template<typename SubType>  
	void registerCreator(const KeyType& _type)  
	{  
		mCreators[_type] = Creater<BaseType, SubType>::create;  
	}  
 
	void unregisterAllCreators()  
	{  
		mCreators.clear();  
	}  
  
	BaseType* create(const KeyType& _type)  
	{  
		auto type = mCreators.find(_type);  
		if (type != mCreators.end())  
		{  
			CreaterDelegate create = type->second;  
			if (create != 0)  
				return create();  
		}  
		return 0;  
	}
private:  
	typedef BaseType* (*CreaterDelegate)();

	std::map<KeyType, CreaterDelegate>  mCreators; 
};
#endif
