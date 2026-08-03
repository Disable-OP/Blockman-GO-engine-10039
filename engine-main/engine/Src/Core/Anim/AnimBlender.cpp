#include "Core.h"
#include "AnimBlender.h"
#include "Animation.h"
#include "SRTTrack.h"
#include "AnimTrack.h"
#include "AnimState.h"
#include "Skeleton.h"
#include "AnimKeyFrame.h"
#include "Object/Exception.h"
#include "Object/Root.h"
#include "Bone.h"

namespace LORD
{
	AnimBlender::AnimBlender(Skeleton* pSkel)
		:m_pSkel(pSkel)
		, m_mode(BM_CROSSFADE)
		, m_pAnimState(NULL)
		, m_pUpperState(NULL)
		, m_pTargetAnimState(NULL)
		, m_blendTime(0)
		, m_leftBlendTime(0)
		, m_AnimMatrix(NULL)
		, m_KeyOrientation(NULL)
		, m_KeyPosition(NULL)
		, m_pBlenderEvent(NULL)
		, mbNeedMerge(false)
		, mbIsLoopSkill(false)
		, m_hasExtRotation(false)
		, m_extIdx(-1)
		, m_extRotation(Quaternion::IDENTITY)
	{
		size_t boneCount = pSkel->getBoneCount();
		m_AnimMatrix = static_cast<Matrix4*>(LordMalloc(boneCount * sizeof(Matrix4))); memset(m_AnimMatrix, 0, boneCount * sizeof(Matrix4));
		m_KeyOrientation = static_cast<Quaternion*>(LordMalloc(boneCount * sizeof(Quaternion))); memset(m_KeyOrientation, 0, boneCount * sizeof(Quaternion));
		m_KeyPosition = static_cast<Vector3*>(LordMalloc(boneCount * sizeof(Vector3))); memset(m_KeyPosition, 0, boneCount * sizeof(Vector3));
	}

	AnimBlender::~AnimBlender()
	{
		if (m_pBlenderEvent)
		{
			m_pBlenderEvent->RestoreEvent();
			m_pBlenderEvent = NULL;//交由副线程删除
		}
		LordSafeFree(m_KeyOrientation);
		LordSafeFree(m_KeyPosition);
		LordSafeFree(m_AnimMatrix);
	}

	bool AnimBlender::blend_upper(AnimState* pAnimState, bool bLoop, float timeScale)
	{
		m_pUpperState = pAnimState;
		m_pUpperState->enable(true);
		m_pUpperState->enableLoop(bLoop);
		m_pUpperState->setTimeScale(timeScale);
		m_pUpperState->setTimePosition(0);
		return true;
	}

	bool AnimBlender::blend(AnimState* pAnimState, ui32 blendTime)
	{
		if (NULL == m_pAnimState)
		{
			AnimKeyFrame* pSrcKeyFrame = pAnimState->getCurrentAnimKeyFrame();
			for (ui32 i = 0; i < m_pSkel->getBoneCount(); ++i)
			{
				pSrcKeyFrame->getAnimQuaternion(i).toMat4(m_AnimMatrix[i]);
				m_AnimMatrix[i].translate(pSrcKeyFrame->getAnimTranslate(i));
			}//for
			m_pAnimState = pAnimState;
		}

		switch (m_mode)
		{
		case BM_REPLACE:
		{
			//m_pAnimState->enable(false);
			m_pAnimState = pAnimState;
			m_blendTime = blendTime;
			m_leftBlendTime = 0;
		}
		break;
		case BM_CROSSFADE:
		{
			m_pTargetAnimState = pAnimState;
			m_blendTime = blendTime;
			m_leftBlendTime = blendTime;
		}
		break;
		default: break;
		}

		return true;
	}

	void AnimBlender::setBlendMode(BlendMode mode)
	{
		m_mode = mode;
	}

	Skeleton* AnimBlender::getSkeleton() const
	{
		return m_pSkel;
	}

	bool AnimBlender::isBlending() const
	{
		return (m_leftBlendTime > 0);
	}

	AnimState* AnimBlender::getAnimState() const
	{
		return m_pAnimState;
	}

	const Matrix4& AnimBlender::getAnimMatrix(uint idx) const
	{
		return m_AnimMatrix[idx];
	}

	const Matrix4* AnimBlender::getAnimMatrixPtr() const
	{
		return m_AnimMatrix;
	}

	void AnimBlender::frameMove(ui32 deltaTime)
	{
		if (NULL == m_pAnimState)
			return;

		m_pAnimState->frameMove(deltaTime);

		// add the uppper anim tracker.
		bool hasUpper = false;
		if (m_pUpperState)
		{
			m_pUpperState->frameMove(deltaTime);
			hasUpper = m_pUpperState->isEnable();
		}

		ui32 boneCount = m_pSkel->getBoneCount();
		if (m_mode == BM_CROSSFADE)
		{
			if (m_pTargetAnimState && m_pTargetAnimState != m_pAnimState) //既然动画一样又何须融合！？
			{
				mbNeedMerge = true;
				m_pTargetAnimState->frameMove(deltaTime);
				if (m_leftBlendTime > 0)
				{
					m_leftBlendTime = m_leftBlendTime - deltaTime;
					if (m_leftBlendTime <= 0)
					{
						m_leftBlendTime = 0;
						m_pAnimState = m_pTargetAnimState;
						mbNeedMerge = false;
					}
				}

				if (mbNeedMerge && !hasUpper)
				{
					AnimKeyFrame* pkf[6];
					float dstWeight = 1.0f - (float)m_leftBlendTime / (float)m_blendTime;
					pkf[0] = m_pAnimState->getCurrentAnimKeyFrame();
					pkf[1] = m_pAnimState->getAnimKeyFrame(m_pAnimState->getNextFrameIndex());
					pkf[2] = m_pTargetAnimState->getCurrentAnimKeyFrame();
					pkf[3] = m_pTargetAnimState->getAnimKeyFrame(m_pTargetAnimState->getNextFrameIndex());

					float Ratio[3] = { 0.0f };
					if (pkf[0] != pkf[1])
					{
						float avgTime = (float)m_pAnimState->getAnim()->getLength() / (float)m_pAnimState->getAnim()->getFrameCount();
						float srcTime = avgTime*m_pAnimState->getCurrentFrameIndex();
						Ratio[0] = ((float)m_pAnimState->getTimePosition() - srcTime) / avgTime;
					}

					if (pkf[2] != pkf[3])
					{
						float avgTime = (float)m_pTargetAnimState->getAnim()->getLength() / (float)m_pTargetAnimState->getAnim()->getFrameCount();
						float srcTime = avgTime*m_pTargetAnimState->getCurrentFrameIndex();
						Ratio[1] = ((float)m_pTargetAnimState->getTimePosition() - srcTime) / avgTime;
					}

					if (hasUpper) // the uppper anim tracker.
					{
						pkf[4] = m_pUpperState->getCurrentAnimKeyFrame();
						pkf[5] = m_pUpperState->getAnimKeyFrame(m_pUpperState->getNextFrameIndex());
						if (pkf[4] != pkf[5])
						{
							float avgTime = (float)m_pUpperState->getAnim()->getLength() / (float)m_pUpperState->getAnim()->getFrameCount();
							float srcTime = avgTime*m_pUpperState->getCurrentFrameIndex();
							Ratio[2] = ((float)m_pUpperState->getTimePosition() - srcTime) / avgTime;
						}
					}

					for (ui32 i = 0; i < boneCount; ++i)
					{
						Bone* pBone = m_pSkel->getBoneById(i);
						const Vector3& vBone = pBone->getPosition();

						if (pBone->isUpper() && hasUpper) // the uppper anim tracker.
						{
							const Quaternion& oriSrc = pkf[4]->getAnimQuaternion(i);
							const Quaternion& oriDst = pkf[5]->getAnimQuaternion(i);
							const Vector3& posSrc = oriSrc * vBone + pkf[4]->getAnimTranslate(i);
							const Vector3& posDst = oriDst * vBone + pkf[5]->getAnimTranslate(i);

							Quaternion::Slerp(m_KeyOrientation[i], oriSrc, oriDst, Ratio[2], true);
							if (m_hasExtRotation && m_extIdx == i && !hasUpper)
							{
								m_KeyOrientation[i] = m_extRotation;
							}
							m_KeyPosition[i] = (posSrc*(1 - Ratio[2]) + posDst*Ratio[2]) - m_KeyOrientation[i] * vBone;
						}
						else
						{
							// 旋转插值
							const Quaternion& ori_00 = pkf[0]->getAnimQuaternion(i);
							const Quaternion& ori_01 = pkf[1]->getAnimQuaternion(i);
							const Quaternion& ori_10 = pkf[2]->getAnimQuaternion(i);
							const Quaternion& ori_11 = pkf[3]->getAnimQuaternion(i);

							Quaternion ori_0, ori_1;
							Quaternion::Slerp(ori_0, ori_00, ori_01, Ratio[0], true);
							Quaternion::Slerp(ori_1, ori_10, ori_11, Ratio[1], true);
							Quaternion::Slerp(m_KeyOrientation[i], ori_0, ori_1, dstWeight, true);

							if (m_hasExtRotation && m_extIdx == i && !hasUpper)
							{
								m_KeyOrientation[i] = m_extRotation;
							}

							// 位移插值
							const Vector3& pos_00 = ori_00 * vBone + pkf[0]->getAnimTranslate(i);
							const Vector3& pos_01 = ori_01 * vBone + pkf[1]->getAnimTranslate(i);
							const Vector3& pos_10 = ori_10 * vBone + pkf[2]->getAnimTranslate(i);
							const Vector3& pos_11 = ori_11 * vBone + pkf[3]->getAnimTranslate(i);

							Vector3 posMerge = pos_00 * (1 - Ratio[0]) * (1 - dstWeight)
								+ pos_01 * Ratio[0] * (1 - dstWeight)
								+ pos_10 * (1 - Ratio[1]) * dstWeight
								+ pos_11 * Ratio[1] * dstWeight;

							m_KeyPosition[i] = posMerge - m_KeyOrientation[i] * vBone;
							m_KeyOrientation[i].toMat4(m_AnimMatrix[i]);
							m_AnimMatrix[i].translate(m_KeyPosition[i]);
						}
					}//for

					return;
				}
			}
		}
#ifdef LERPCALCANIM
		AnimKeyFrame* pkf[4] = { NULL };
		float Ratio[2] = { 0.0f };

		// base animation
		pkf[0] = m_pAnimState->getCurrentAnimKeyFrame();
		pkf[1] = m_pAnimState->getAnimKeyFrame(m_pAnimState->getNextFrameIndex());
		int upRootId = m_pSkel->getUpRootId();
		Vector3 upRootTras1, upRootTrans2;
		Vector3 upRootPos;
		Quaternion upRootOrit;
		Matrix4 upRootMat;

		if (mbIsLoopSkill && (pkf[1] == pkf[0]) && (m_pAnimState == m_pTargetAnimState))
			pkf[1] = m_pAnimState->getAnimKeyFrame(0);
		if (pkf[1] != pkf[0])
		{
			float avgTime = (float)m_pAnimState->getAnim()->getLength() / (float)m_pAnimState->getAnim()->getFrameCount();
			float srcTime = avgTime*m_pAnimState->getCurrentFrameIndex();
			Ratio[0] = ((float)m_pAnimState->getTimePosition() - srcTime) / avgTime;

			const Vector3& vBone = m_pSkel->getBoneById(upRootId)->getPosition();
			const Quaternion& oriSrc = pkf[0]->getAnimQuaternion(upRootId);
			const Quaternion& oriDst = pkf[1]->getAnimQuaternion(upRootId);
			const Vector3& posSrc = oriSrc * vBone + pkf[0]->getAnimTranslate(upRootId);
			const Vector3& posDst = oriDst * vBone + pkf[1]->getAnimTranslate(upRootId);

			Quaternion::Slerp(upRootOrit, oriSrc, oriDst, Ratio[0], true);
			upRootTras1 = (posSrc*(1 - Ratio[0]) + posDst*Ratio[0]) - upRootOrit * vBone;
		}
		else
		{
			upRootOrit = pkf[0]->getAnimQuaternion(upRootId);
			upRootTras1 = pkf[0]->getAnimTranslate(upRootId);
		}
		upRootPos = upRootTras1;

		if (hasUpper) // the uppper anim tracker.
		{
			Quaternion orit;
			pkf[2] = m_pUpperState->getCurrentAnimKeyFrame();
			pkf[3] = m_pUpperState->getAnimKeyFrame(m_pUpperState->getNextFrameIndex());
			if (pkf[2] != pkf[3])
			{
				float avgTime = (float)m_pUpperState->getAnim()->getLength() / (float)m_pUpperState->getAnim()->getFrameCount();
				float srcTime = avgTime*m_pUpperState->getCurrentFrameIndex();
				Ratio[1] = ((float)m_pUpperState->getTimePosition() - srcTime) / avgTime;

				const Vector3& vBone = m_pSkel->getBoneById(upRootId)->getPosition();
				const Quaternion& oriSrc = pkf[2]->getAnimQuaternion(upRootId);
				const Quaternion& oriDst = pkf[3]->getAnimQuaternion(upRootId);
				const Vector3& posSrc = oriSrc * vBone + pkf[2]->getAnimTranslate(upRootId);
				const Vector3& posDst = oriDst * vBone + pkf[3]->getAnimTranslate(upRootId);

				Quaternion::Slerp(orit, oriSrc, oriDst, Ratio[1], true);
				upRootTrans2 = (posSrc*(1 - Ratio[1]) + posDst*Ratio[1]) - orit * vBone;
			}
			else
			{
				orit = pkf[2]->getAnimQuaternion(upRootId);
				upRootTrans2 = pkf[2]->getAnimTranslate(upRootId);
			}
			upRootOrit.diff(orit, upRootOrit);

			// make adjustment matrix for upper animation.
			Matrix4 temp;
			upRootMat.makeTranslation(upRootTras1);
			temp.identity();
			temp.fromQuan(upRootOrit);
			upRootMat.joint(temp);
			temp.identity();
			temp.translate(-upRootTrans2);
			upRootMat.joint(temp);
		}

		for (ui32 i = 0; i < boneCount; ++i)
		{
			Bone* pBone = m_pSkel->getBoneById(i);
			const Vector3& vBone = pBone->getPosition();

			AnimKeyFrame* pkf0 = pkf[0];
			AnimKeyFrame* pkf1 = pkf[1];
			float ratio = Ratio[0];
			if (pBone->isUpper() && hasUpper) // the uppper anim tracker.
			{
				pkf0 = pkf[2];
				pkf1 = pkf[3];
				ratio = Ratio[1];
			}

			if (pkf1 != pkf0)
			{
				const Quaternion& oriSrc = pkf0->getAnimQuaternion(i);
				const Quaternion& oriDst = pkf1->getAnimQuaternion(i);
				const Vector3& posSrc = oriSrc * vBone + pkf0->getAnimTranslate(i);
				const Vector3& posDst = oriDst * vBone + pkf1->getAnimTranslate(i);

				Quaternion::Slerp(m_KeyOrientation[i], oriSrc, oriDst, ratio, true);
				if (m_hasExtRotation && m_extIdx == i && !hasUpper)
				{
					m_KeyOrientation[i] = m_extRotation;
				}
				m_KeyPosition[i] = (posSrc*(1 - ratio) + posDst*ratio) - m_KeyOrientation[i] * vBone;
			}
			else
			{
				m_KeyOrientation[i] = pkf0->getAnimQuaternion(i);
				m_KeyPosition[i] = pkf0->getAnimTranslate(i);
				if (m_hasExtRotation && m_extIdx == i && !hasUpper)
				{
					Vector3 pos = m_KeyOrientation[i] * vBone + m_KeyPosition[i];
					m_KeyOrientation[i] = m_extRotation;
					m_KeyPosition[i] = pos - m_KeyOrientation[i] * vBone;
				}
			}

			m_KeyOrientation[i].toMat4(m_AnimMatrix[i]);
			m_AnimMatrix[i].translate(m_KeyPosition[i]);

			// The code is disgusting, this function is too slower...any another good idea?
			// sparate
			if (pBone->isUpper() && hasUpper) // the uppper anim tracker.
			{
				m_AnimMatrix[i] *= upRootMat;
				Matrix4 temp = m_AnimMatrix[i];
				m_KeyPosition[i].x = temp.m30;
				m_KeyPosition[i].y = temp.m31;
				m_KeyPosition[i].z = temp.m32;
				temp.m30 = 0.f;
				temp.m31 = 0.f;
				temp.m32 = 0.f;
				m_KeyOrientation[i].fromMat4(temp);
			}
		}//for
#else
		AnimKeyFrame* pKeyFrame = m_pAnimState->getCurrentAnimKeyFrame();
		memcpy(m_KeyOrientation, pKeyFrame->getAnimQuaternionPtr(), sizeof(Quaternion)*boneCount);
		memcpy(m_KeyPosition, pKeyFrame->getAnimTranslatePtr(), sizeof(Vector3)*boneCount);

		for (size_t i = 0; i < boneCount; ++i)
		{
			m_KeyOrientation[i].toMat4(m_AnimMatrix[i]);
			m_AnimMatrix[i].translate(m_KeyPosition[i]);
		}
#endif
	}

	void AnimBlender::frameMoveForCalcThread(ui32 deltaTime)
	{
		if (NULL == m_pAnimState)
			return;

		m_pAnimState->frameMove(deltaTime);

		if (m_mode == BM_CROSSFADE)
		{
			if (m_pTargetAnimState && m_pTargetAnimState != m_pAnimState) //既然动画一样又何须融合！？
			{
				mbNeedMerge = true;
				m_pTargetAnimState->frameMove(deltaTime);
				if (m_leftBlendTime > 0)
				{
					m_leftBlendTime = m_leftBlendTime - deltaTime;
					if (m_leftBlendTime <= 0)
					{
						m_leftBlendTime = 0;
						m_pAnimState = m_pTargetAnimState;
						mbNeedMerge = false;
					}
				}
			}
		}
		if (!m_pBlenderEvent)
		{
			m_pBlenderEvent = LordNew BlenderEvent(this);
		}

		if (!m_pBlenderEvent->GetInCalcThread())
		{
			Root::Instance()->getCalcThread()->AddRequest(m_pBlenderEvent);
			m_pBlenderEvent->SetInCalcThread(true);
		}
	}

	const Quaternion& AnimBlender::getAnimOrientation(uint idx) const
	{
		return m_KeyOrientation[idx];
	}

	const Quaternion* AnimBlender::getAnimOrientationPtr() const
	{
		return m_KeyOrientation;
	}

	const Vector3& AnimBlender::getAnimPosition(uint idx) const
	{
		return m_KeyPosition[idx];
	}

	const Vector3* AnimBlender::getAnimPositionPtr() const
	{
		return m_KeyPosition;
	}

	void AnimBlender::setExtRotation(int idx, const Quaternion& q)
	{
		m_extIdx = idx;
		m_hasExtRotation = (idx >= 0);
		m_extRotation = q;
	}

	void AnimBlender::excludeCalcThread()
	{
		if (m_pBlenderEvent && m_pBlenderEvent->GetInCalcThread())
		{
			m_pBlenderEvent->RestoreEvent();
			m_pBlenderEvent->SetInCalcThread(false);
			m_pBlenderEvent = NULL;
		}
	}

	void AnimBlender::setIsLoopSkill(bool isLoopSkill)
	{
		mbIsLoopSkill = isLoopSkill;
	}

	//---------------------------------BlenderEvent------------------------------------
	BlenderEvent::BlenderEvent(AnimBlender* blender)
		: mBlender(blender)
		, mInCalcThread(false)
		, mBlenderOrientation(NULL)
		, mBlenderPos(NULL)
	{
	}

	BlenderEvent::~BlenderEvent()
	{
		mBlender = NULL;
		mBlenderOrientation = NULL;
		mBlenderPos = NULL;
	}

	bool BlenderEvent::ProcessEvent()
	{
		if (!mBlender)
			return false;

		mBlenderOrientation = mBlender->m_KeyOrientation;
		mBlenderPos = mBlender->m_KeyPosition;
		AnimState* pTargetState = mBlender->m_pTargetAnimState;
		AnimState* pSrcState = mBlender->m_pAnimState;

		AnimKeyFrame* pSrcKeyFrame = NULL;
		AnimKeyFrame* pDstKeyFrame = NULL;
		if (mBlender->m_mode == AnimBlender::BM_CROSSFADE)
		{
			if (mBlender->mbNeedMerge)
			{
				AnimKeyFrame* pkf[4];
				float dstWeight = 1.0f - (float)mBlender->m_leftBlendTime / (float)mBlender->m_blendTime;
				pkf[0] = pSrcState->getCurrentAnimKeyFrame();
				pkf[1] = pSrcState->getAnimKeyFrame(pSrcState->getNextFrameIndex());
				pkf[2] = pTargetState->getCurrentAnimKeyFrame();
				pkf[3] = pTargetState->getAnimKeyFrame(pTargetState->getNextFrameIndex());

				float Ratio[2] = { 0.0f };
				if (pkf[0] != pkf[1])
				{
					float avgTime = (float)pSrcState->getAnim()->getLength() / (float)pSrcState->getAnim()->getFrameCount();
					float srcTime = avgTime*pSrcState->getCurrentFrameIndex();
					Ratio[0] = ((float)pSrcState->getTimePosition() - srcTime) / avgTime;
				}

				if (pkf[2] != pkf[3])
				{
					float avgTime = (float)pTargetState->getAnim()->getLength() / (float)pTargetState->getAnim()->getFrameCount();
					float srcTime = avgTime*pTargetState->getCurrentFrameIndex();
					Ratio[1] = ((float)pTargetState->getTimePosition() - srcTime) / avgTime;
				}

				for (ui32 i = 0; i < mBlender->m_pSkel->getBoneCount(); ++i)
				{
					Bone* pBone = mBlender->m_pSkel->getBoneById(i);
					const Vector3& vBone = pBone->getPosition();

					// 旋转插值
					const Quaternion& ori_00 = pkf[0]->getAnimQuaternion(i);
					const Quaternion& ori_01 = pkf[1]->getAnimQuaternion(i);
					const Quaternion& ori_10 = pkf[2]->getAnimQuaternion(i);
					const Quaternion& ori_11 = pkf[3]->getAnimQuaternion(i);

					Quaternion ori_0, ori_1;
					Quaternion::Slerp(ori_0, ori_00, ori_01, Ratio[0], true);
					Quaternion::Slerp(ori_1, ori_10, ori_11, Ratio[1], true);
					Quaternion::Slerp(mBlenderOrientation[i], ori_0, ori_1, dstWeight, true);
					if (mBlender->m_hasExtRotation && mBlender->m_extIdx == i)
						mBlenderOrientation[i] = mBlender->m_extRotation;

					// 位移插值
					const Vector3& pos_00 = ori_00 * vBone + pkf[0]->getAnimTranslate(i);
					const Vector3& pos_01 = ori_01 * vBone + pkf[1]->getAnimTranslate(i);
					const Vector3& pos_10 = ori_10 * vBone + pkf[2]->getAnimTranslate(i);
					const Vector3& pos_11 = ori_11 * vBone + pkf[3]->getAnimTranslate(i);

					Vector3 posMerge = pos_00 * (1 - Ratio[0]) * (1 - dstWeight)
						+ pos_01 * Ratio[0] * (1 - dstWeight)
						+ pos_10 * (1 - Ratio[1]) * dstWeight
						+ pos_11 * Ratio[1] * dstWeight;

					mBlenderPos[i] = posMerge - mBlenderOrientation[i] * vBone;

					mBlenderOrientation[i].toMat4(mBlender->m_AnimMatrix[i]);
					mBlender->m_AnimMatrix[i].translate(mBlenderPos[i]);
				}//for

				return true;
			}
		}
		float Ratio = 0.0f;
		pSrcKeyFrame = pSrcState->getCurrentAnimKeyFrame();
		pDstKeyFrame = pSrcState->getAnimKeyFrame(pSrcState->getNextFrameIndex());

		if (mBlender->mbIsLoopSkill && (pDstKeyFrame == pSrcKeyFrame) && (pSrcState == pTargetState))
		{
			pDstKeyFrame = pSrcState->getAnimKeyFrame(0);
		}
		if (pDstKeyFrame != pSrcKeyFrame)
		{
			float avgTime = (float)pSrcState->getAnim()->getLength() / (float)pSrcState->getAnim()->getFrameCount();
			float srcTime = avgTime*pSrcState->getCurrentFrameIndex();
			Ratio = ((float)pSrcState->getTimePosition() - srcTime) / avgTime;
		}
		for (ui32 i = 0; i < mBlender->m_pSkel->getBoneCount(); ++i)
		{
			if (pDstKeyFrame != pSrcKeyFrame)
			{
				const Vector3& vBone = mBlender->m_pSkel->getBoneById(i)->getPosition();

				const Quaternion& oriSrc = pSrcKeyFrame->getAnimQuaternion(i);
				const Quaternion& oriDst = pDstKeyFrame->getAnimQuaternion(i);
				const Vector3& posSrc = oriSrc * vBone + pSrcKeyFrame->getAnimTranslate(i);
				const Vector3& posDst = oriDst * vBone + pDstKeyFrame->getAnimTranslate(i);

				Quaternion::Slerp(mBlenderOrientation[i], oriSrc, oriDst, Ratio, true);
				if (mBlender->m_hasExtRotation && mBlender->m_extIdx == i)
					mBlenderOrientation[i] = mBlender->m_extRotation;
				mBlenderPos[i] = (posSrc*(1 - Ratio) + posDst*Ratio) - mBlenderOrientation[i] * vBone;
			}
			else
			{
				mBlenderOrientation[i] = pSrcKeyFrame->getAnimQuaternion(i);
				if (mBlender->m_hasExtRotation && mBlender->m_extIdx == i)
					mBlenderOrientation[i] = mBlender->m_extRotation;
				mBlenderPos[i] = pSrcKeyFrame->getAnimTranslate(i);
			}

			mBlenderOrientation[i].toMat4(mBlender->m_AnimMatrix[i]);
			mBlender->m_AnimMatrix[i].translate(mBlenderPos[i]);
		}//for

		return true;
	}

	void BlenderEvent::SetInCalcThread(bool inThread)
	{
		mInCalcThread = inThread;
	}

	bool BlenderEvent::GetInCalcThread()
	{
		return mInCalcThread;
	}

	void BlenderEvent::RestoreEvent()
	{
		mBlender = NULL;
	}
}
