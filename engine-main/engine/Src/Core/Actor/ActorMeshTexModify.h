/********************************************************************
filename: 	ActorMeshTexModify.h
file path:	dev\engine\Src\Core\Actor\

version:	1
author:		ajohn
company:	supernano
date:		2017-3-13
*********************************************************************/
#ifndef __ACTOR_MESH_TEX_MODIFY_HEADER__
#define __ACTOR_MESH_TEX_MODIFY_HEADER__

#include "Core.h"

namespace LORD
{

class ActorMeshTexModify : public ObjectAlloc
{
	struct CharFontMem
	{
		int w;
		int h;
		ui8 data[40 * 40];
	};

	struct ModifyInfo
	{
		float ratio;
		String numbers;
	};
		
public:
	ActorMeshTexModify(const String& master, const String& slave, const Vector2i& pos, const Vector2i& size);
	virtual ~ActorMeshTexModify();

	void setBackGround(const Color& c) { m_backGround = c.getARGB(); }
	void setProcessing(const Color& c) { m_processing = c.getARGB(); }
	void setTextColor(const Color& c) { m_textColor = c; }

	const String& getMasterName() const { return m_masterName; }
	const String& getSlaveName() const { return m_slaveName; }

	TextureModify* getModifyPtr() { return &m_modifyier; }

	void updateString(float ratio, const String& number);

protected:
	void setupCharacters();
	
protected:
	TextureModify m_modifyier;

	String m_masterName;
	String m_slaveName;

	ui32	 m_backGround;
	ui32	 m_processing;
	Color	 m_textColor;

	float    m_ratio;
	String	 m_showNumber;


	static bool	 s_fontInited;
	static CharFontMem s_characters[11]; // '0123456789/'
};

}

#endif
