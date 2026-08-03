/********************************************************************
filename: 	SectionClient.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2017-06-08
*********************************************************************/
#ifndef __SECTION_CLIENT_HEADER__
#define __SECTION_CLIENT_HEADER__

/** header files from common. */
#include "World/Section.h"

namespace BLOCKMAN
{

class SectionClient : public Section
{
	NBTB_DECLARE_FRIEND(SectionClient);
public:
	SectionClient(int yBase = 0);
	~SectionClient();

	// implement virtual function from Section.
	virtual void setBlockID(const BlockPos& pos, int id) override;
	virtual void setBlockMeta(const BlockPos& pos, int meta) override;
	virtual void setExtSkylightValue(const BlockPos& pos, int skyLightValue) override;
	virtual int getExtSkylightValue(const BlockPos& pos) override;
	virtual void setExtBlocklightValue(const BlockPos& pos, int blockLightValue) override;
	virtual int getExtBlocklightValue(const BlockPos& pos) override;
	virtual void reBuildVisibility() override;
	virtual SetVisibility* getVisibility()  override { return m_setVisibility; }
	NibbleArray* getBlockLightArray() { return m_blocklightArray; }
	NibbleArray* getSkyLightArray() { return m_skylightArray; }
	bool isVisibilityBuild() const { return m_isVisSetBuild; }

	inline ui32 getBlockUI32Data(const BlockPos& pos);

protected:
	/** Set true if the section's visibility is build */
	bool m_isVisSetBuild = false;
	SetVisibility* m_setVisibility = nullptr;
	Mutex m_lock;

	// The NibbleArray containing a block of Block-light data.
	NibbleArray* m_blocklightArray = nullptr;
	// The NibbleArray containing a block of Sky-light data. 
	NibbleArray* m_skylightArray = nullptr;
};

inline ui32 SectionClient::getBlockUI32Data(const BlockPos& pos)
{
	int id = getBlockID(pos);
	int meta = id == 0 ? 0 : getBlockMeta(pos);
	int skyLight = getExtSkylightValue(pos);
	int blockLight = getExtBlocklightValue(pos);
	ui32 ret = id; 
	ret |= ui32(meta) << 16;
	ret |= ui32(skyLight) << 24;
	ret |= ui32(blockLight) << 28; 
	return ret;
}

NBTB_BEGIN(SectionClient, Section)
NBTB_END

}

#endif
