#include "Core.h"
#include "Animation.h"
#include "AABBKeyFrame.h"
#include "SRTTrack.h"
#include "Geom/GeomLib.h"
#include "Resource/ResourceGroupManager.h"

namespace LORD
{
	
// Locally key frame search helper
struct AnimKeyFrameTimeLess
{
	bool operator() (const AnimKeyFrame* t1, const AnimKeyFrame* t2) const
	{
		return t1->getTime() < t2->getTime();
	}
};

Animation::Animation(const String& name, ResourceType rt)
	:Resource(name, rt)
	,m_frameCount(0)
	,m_length(0)
	,m_fps(20)
	,m_bHasScaling(true)
	,m_pAnimStream(NULL)
{
}

Animation::~Animation()
{
	unloadImpl();
}

size_t Animation::calculateSize() const
{
	// not implement
	return 0;
}

bool Animation::prepareImpl()
{
	m_pAnimStream = ResourceGroupManager::Instance()->openResource(mName);
	
	if(m_pAnimStream == NULL)
		return false;

	int version;
	m_pAnimStream->read(&version, sizeof(int));

	// fps
	uint fps;
	m_pAnimStream->read(&fps, sizeof(uint));

	// frame count
	uint frameCount;
	m_pAnimStream->read(&frameCount, sizeof(uint));

	// bone count
	uint boneCount;
	m_pAnimStream->read(&boneCount, sizeof(uint));

	m_frameCount = frameCount;
	m_length = ui32((float)frameCount / (float)fps * 1000);
	m_fps = fps;
	m_bHasScaling = false;

	float position[3];
	float rotation[4];
	Vector3 vPos;
	Quaternion qRot;
	Box box;
	for(uint i = 0; i < frameCount; ++i)
	{
		ui32 keyFrameTime = (i*1000) / (fps*1000);
		AABBKeyFrame* pKeyFrame = (AABBKeyFrame*)createAnimKeyFrame(keyFrameTime);
		pKeyFrame->createAnimData(boneCount);
		for(uint j = 0; j < boneCount; ++j)
		{
			m_pAnimStream->read(&position, sizeof(position));
			vPos.set(position[0], position[1], position[2]);

			m_pAnimStream->read(&rotation, sizeof(rotation));
			qRot.set(rotation[0], rotation[1], rotation[2], rotation[3]);

			pKeyFrame->setAnimData(j, qRot, vPos);
		}

		m_pAnimStream->read(&box.vMin, sizeof(Vector3));
		m_pAnimStream->read(&box.vMax, sizeof(Vector3));

		pKeyFrame->setBox(box);
	}
	LordSafeDelete(m_pAnimStream);

	return true;
}

void Animation::unprepareImpl()
{
}

bool Animation::loadImpl()
{
	return true;
}

void Animation::unloadImpl()
{
	for(size_t i = 0; i < m_keyFrames.size(); ++i)
	{
		LordSafeDelete(m_keyFrames[i]);
	}
}

AnimKeyFrame* Animation::createAnimKeyFrame(ui32 time)
{
	AnimKeyFrame* pKeyFrame = LordNew AABBKeyFrame(this, time);;
		
	// Insert just before upper bound
	AnimKeyFrameList::iterator i = std::upper_bound(m_keyFrames.begin(), m_keyFrames.end(), pKeyFrame, AnimKeyFrameTimeLess());
	m_keyFrames.insert(i, pKeyFrame);
	
	return pKeyFrame;
}

void Animation::removeAnimKeyFrameByIdx(uint idx)
{
	// If you hit this assert, then the track index is out of bounds
	LordAssert(idx < (uint)m_keyFrames.size());
	
	AnimKeyFrameList::iterator it = m_keyFrames.begin();
	
	it += idx;
	
	LordSafeDelete(*it);
	m_keyFrames.erase(it);
}

void Animation::removeAnimKeyFrames()
{
	AnimKeyFrameList::iterator it = m_keyFrames.begin();
	for (; it != m_keyFrames.end(); ++it)
		LordSafeDelete(*it);
	
	m_keyFrames.clear();
}

uint Animation::getFrameCount() const
{
	return m_frameCount;
}

ui32 Animation::getLength() const
{
	return m_length;
}

uint Animation::getFPS() const
{
	return m_fps;
}

bool Animation::hasScaling() const
{
	return m_bHasScaling;
}

AnimKeyFrame* Animation::getAnimKeyFrame(ui32 time) const
{
	uint nFrame = (uint)Math::Floor((float)time*0.001f * m_fps);
	LordAssert(nFrame >= 0 && nFrame < m_frameCount)
	
	return m_keyFrames[nFrame];
}

AnimKeyFrame* Animation::getAnimKeyFrameByIdx(uint idx) const
{
	LordAssert(idx <= m_frameCount);
	
	return m_keyFrames[idx];
}

const Animation::AnimKeyFrameList& Animation::getAnimKeyFrameList() const
{
	return m_keyFrames;
}

uint Animation::getFrameIndex(ui32 time) const
{
	int nFrame = (int)Math::Floor((float)time*0.001f * m_fps);
	if(nFrame < 0)
		nFrame = 0;
	else if((uint)nFrame >= m_frameCount)
		nFrame = Math::Max((int)m_frameCount - 1, 0);
	
	return nFrame;
}
}
