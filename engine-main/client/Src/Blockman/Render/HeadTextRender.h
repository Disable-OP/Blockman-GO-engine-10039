/********************************************************************
filename: 	HeadTextRender.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2017-09-26
*********************************************************************/

#ifndef __HEAD_TEXT_RENDER_H__
#define __HEAD_TEXT_RENDER_H__

#include "Core.h"
#include "UI/GUIString.h"
using namespace LORD;

namespace BLOCKMAN
{

class HeadTextRender : public ObjectAlloc
{
	friend class HeadTextRenderManager;
protected:
	Vector3 m_position;
	GUIString m_text;

public:
	HeadTextRender();

	virtual ~HeadTextRender();

	void setText(const GUIString& text) { m_text = text; }
	const GUIString& getText() const { return m_text; }
	void setPosition(const Vector3& pos) { m_position = pos; }
};

class HeadTextRenderManager;

class HeadTextManualRender : public ManualRenderListener
{
public:
	virtual void render(bool uiRender);
	HeadTextRenderManager* m_pHeadTextManager;
};

struct HeadTextInfo
{
	GUIString text;
	Color	  color;
};

typedef vector<HeadTextInfo>::type HeadTextInfos;

class HeadTextRenderManager : public Singleton<HeadTextRenderManager>, public LORD::ObjectAlloc
{
public:
	static HeadTextRenderManager* Instance();

public:
	typedef std::vector<HeadTextRender> HeadTextRenderArr;

public:
	HeadTextRenderManager();
	~HeadTextRenderManager();

	void addHeadText(const HeadTextRender& headTxt) { m_headTexts.push_back(headTxt); }
	void render();

protected:
	HeadTextManualRender m_manualRender;
	HeadTextRenderArr m_headTexts;
	void parseText(const GUIString& str, HeadTextInfos& outInfos);
};

}


#endif