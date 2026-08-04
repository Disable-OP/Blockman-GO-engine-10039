/********************************************************************
filename: 	RTTI.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2017-3-20
*********************************************************************/
#ifndef __BM_RTTI_HEADER__
#define __BM_RTTI_HEADER__

#include "Util/BitSet.h"
#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{


class RuntimeClass
{
public:
	typedef vector<const RuntimeClass*>::type RuntimeClassArr;

public:
	String				m_className;
	int					m_classID;
	RuntimeClass*		m_parent;
	RuntimeClassArr		m_children;

	RuntimeClass(const String& className, int classId, RuntimeClass* parent)
		: m_className(className)
		, m_classID(classId)
		, m_parent(parent)
	{
		if (m_parent)
			m_parent->addchild(this);
	}

	void addchild(const RuntimeClass* child) 
	{
		m_children.push_back(child);
	}

	int getParentClassID() const
	{
		if (m_parent)
			return m_parent->m_classID;
		return 0;
	}

	bool isClass(int classID) const 
	{
		if (classID == m_classID)
			return true;

		if (m_parent)
			return m_parent->isClass(classID);

		return false;
	}

	bool isClass(const String& className) const
	{
		if (className == m_className)
			return true;

		if (m_parent)
			return m_parent->isClass(className);

		return false;
	}
};

#define RTTI_DECLARE(class_name)											\
protected:																	\
	static RuntimeClass rtti##class_name;									\
public:																		\
	virtual bool isClass(int classid) const									\
	{ return rtti##class_name.isClass(classid); }							\
	virtual int getClassID() const											\
	{ return rtti##class_name.m_classID; }									\
	virtual bool isClass(const String& className) const						\
	{ return rtti##class_name.isClass(className); }							\
	virtual int getParentClassID() const									\
	{ return rtti##class_name.getParentClassID(); }							\
	virtual const RuntimeClass* getRuntimeClass()							\
	{ return &rtti##class_name; }											

#define RTTI_IMPLEMENT_BASE(class_name, class_ID)							\
	RuntimeClass class_name::rtti##class_name(								\
		#class_name,														\
		class_ID,															\
		NULL);

#define RTTI_IMPLEMENT(class_name, class_ID, parent_name)					\
	RuntimeClass class_name::rtti##class_name(								\
		#class_name,														\
		class_ID,															\
		&parent_name::rtti##parent_name);

	
}

#endif