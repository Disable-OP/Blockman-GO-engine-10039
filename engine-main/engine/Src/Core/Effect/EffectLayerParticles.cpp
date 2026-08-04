#include "Core.h"
#include "EffectLayerParticles.h"
#include "EffectSystemManager.h"
#include "Scene/SceneNode.h"
#include "Resource/DataStream.h"

namespace LORD
{

	const i32		EffectLayerParticles::DEFAULT_PT_QUOTA = -1;	
	const bool		EffectLayerParticles::DEFAULT_PT_3DBILLBOARD = false;
	const bool		EffectLayerParticles::DEFAULT_PT_NOTRANSSCALE = false;
	const i32		EffectLayerParticles::DEFAULT_PT_EMITTERRATE = 10;
	const float		EffectLayerParticles::DEFAULT_PT_EMITTERANGLE = 0;
	const float		EffectLayerParticles::DEFAULT_PT_PTSPEED = 0;
	const float		EffectLayerParticles::DEFAULT_PT_MAXPTSPEED = 0;
	const float		EffectLayerParticles::DEFAULT_PT_PTADDSPEED = 0;
	const i32		EffectLayerParticles::DEFAULT_PT_PTLEAVETIME = 1000;
	const i32		EffectLayerParticles::DEFAULT_PT_MAXPTLEAVETIME = 1000;
	const Color		EffectLayerParticles::DEFAULT_PT_MINCOLOR(1,1,1,1);
	const Color		EffectLayerParticles::DEFAULT_PT_MAXCOLOR(1,1,1,1);
	const float		EffectLayerParticles::DEFAULT_PT_MINSCALE = 1;
	const float		EffectLayerParticles::DEFAULT_PT_MAXSCALE = 1;
	const float		EffectLayerParticles::DEFAULT_PT_MINANGLE = 0;
	const float		EffectLayerParticles::DEFAULT_PT_MAXANGLE = 0;
	const bool		EffectLayerParticles::DEFAULT_PT_FALLOWPARENT = true;
	const bool		EffectLayerParticles::DEFAULT_PT_EMITTERINSURFACE = false;

	EffectLayerParticles::EffectLayerParticles()
		:EffectLayer()
		, mParticleQuota(DEFAULT_PT_QUOTA)
		, mWidth(DEFAULT_LY_WIDTH)
		, mHeight(DEFAULT_LY_HEIGHT)
		, mb3dBillboard(DEFAULT_PT_3DBILLBOARD)
		, mbPerpendicular(DEFAULT_LY_PERPENDICULAR)
		, mb3DBillboardTrans(false)
		, mbNoTransScale(DEFAULT_PT_NOTRANSSCALE)
		, mbNoScaleWidth(DEFAULT_LY_NOSCALEWIDTH)
		, mbNoScaleHeight(DEFAULT_LY_NOSCALEHEIGHT)
		, mCenterOffsetW(DEFAULT_LY_CENTEROFFSETW)
		, mCenterOffsetH(DEFAULT_LY_CENTEROFFSETH)
		, mEmitterRate(DEFAULT_PT_EMITTERRATE)
		, mEmitterAngle(DEFAULT_PT_EMITTERANGLE)
		, mParticleSpeed(DEFAULT_PT_PTSPEED)
		, mParticleMaxSpeed(DEFAULT_PT_MAXPTSPEED)
		, mParticleAddSpeed(DEFAULT_PT_PTADDSPEED)
		, mParticleLeaveTime(DEFAULT_PT_PTLEAVETIME)
		, mParticleMaxLeaveTime(DEFAULT_PT_MAXPTLEAVETIME)
		, mMinColor(DEFAULT_PT_MINCOLOR)
		, mMaxColor(DEFAULT_PT_MAXCOLOR)
		, mMinScale(DEFAULT_PT_MINSCALE)
		, mMaxScale(DEFAULT_PT_MAXSCALE)
		, mMin2dAngle(DEFAULT_PT_MINANGLE)
		, mMax2dAngle(DEFAULT_PT_MAXANGLE)
		, mFallowParent(DEFAULT_PT_FALLOWPARENT)
		, mEmitterInSurface(DEFAULT_PT_EMITTERINSURFACE)
		, mbStopEmitter(false)
		, mParticleControllerSet(0)
	{
		direction = Vector3::UNIT_Z;
		mbParticleSystem = true;
		createParticleControllerSet();
	}

	EffectLayerParticles::~EffectLayerParticles()
	{
		destroyParticleControllerSet();
	}

	EffectParticleControllerSet* EffectLayerParticles::createParticleControllerSet()
	{
		EffectParticleControllerSet* particleControllerSet = 
			EffectSystemManager::Instance()->createParticleControllerSet();

		setParticleControllerSet(particleControllerSet);
		return particleControllerSet;
	}

	void EffectLayerParticles::setParticleControllerSet(EffectParticleControllerSet* particleControllerSet)
	{
		mParticleControllerSet = particleControllerSet;
	}

	EffectParticleControllerSet* EffectLayerParticles::getParticleControllerSet() const
	{
		return mParticleControllerSet;
	}

	void EffectLayerParticles::destroyParticleControllerSet()
	{
		if(mParticleControllerSet)
		{
			EffectSystemManager::Instance()->destroyParticleControllerSet(mParticleControllerSet);	
			mParticleControllerSet = NULL;
		}
	}

	void EffectLayerParticles::prepare()
	{
		EffectLayer::prepare();

		i32 poolSize = i32(mEmitterRate * (float(mParticleMaxLeaveTime)/1000.0f)) + 1;

		mEmitterTimeStep = 1000/mEmitterRate;

		//add by ajohn 
		if(poolSize > mParticleQuota && mParticleQuota > 0)
			poolSize = mParticleQuota + 1;
		clearPool();
		createPool(poolSize);
	}

	void EffectLayerParticles::unprepare()
	{
		EffectLayer::unprepare();

		clearPool();
	}

	void EffectLayerParticles::_notifyStart()
	{
		EffectLayer::_notifyStart();

		mParticlePoolFree.splice(mParticlePoolFree.end(), mParticlePoolInUse);
		mEmitterTime = 0;
		mInnerParticleQuota = mParticleQuota;

		assert(mParticleControllerSet);
		mParticleControllerSet->_notifyStart();

		mbStopEmitter = false;
	}

	void EffectLayerParticles::_notifyReStart()
	{
		EffectLayer::_notifyReStart();

		mParticlePoolFree.splice(mParticlePoolFree.end(), mParticlePoolInUse);
		mEmitterTime = 0;
		mInnerParticleQuota = mParticleQuota;

		assert(mParticleControllerSet);
		mParticleControllerSet->_notifyStart();

		mbStopEmitter = false;
	}

	void EffectLayerParticles::_notifyShowVisualHelp(bool show)
	{
		EffectLayer::_notifyShowVisualHelp(show);
	}

	void EffectLayerParticles::emitterParticle(EffectParticle* particle)
	{
		Vector3 v1 = Vector3::UNIT_Z;
		Vector3 v2 = Vector3::UNIT_Z;

		if(mEmitterAngle != DEFAULT_PT_EMITTERANGLE)
		{
			Quaternion q1, q2;
			q1.rotateAxis(Vector3::UNIT_X, Math::IntervalRandom(-mEmitterAngle, mEmitterAngle)*Math::DEG2RAD);
			q2.rotateAxis(Vector3::UNIT_Y, Math::IntervalRandom(-mEmitterAngle, mEmitterAngle)*Math::DEG2RAD);
			v1 = q1 * v1;
			v2 = q2 * v2;
		}

		if(mFallowParent)
		{
			particle->direction = particle->originalDirection = mb3DBillboardTrans?Vector3::UNIT_Y:Vector3::UNIT_Z;
			particle->oritation = particle->originalOritation = Quaternion::IDENTITY;
			particle->movementDirection = ((v1+v2).normalizedCopy());
		}
		else 
		{
			particle->direction = particle->originalDirection = direction;
			particle->oritation = particle->originalOritation = oritation;
			particle->movementDirection = ((mParentSystem->getEffectSceneNode()->getWorldOrientation()) * oritation) * ((v1+v2).normalizedCopy());
		}
		particle->mTime = 0;

		if(mParticleLeaveTime != mParticleMaxLeaveTime)
			particle->mLeaveTime = (i32)Math::IntervalRandom((float)mParticleLeaveTime, (float)mParticleMaxLeaveTime);
		else
			particle->mLeaveTime = mParticleLeaveTime;

		if(mParticleSpeed != mParticleMaxSpeed)
			particle->originalSpeed = Math::IntervalRandom(mParticleSpeed, mParticleMaxSpeed);
		else
			particle->originalSpeed = mParticleSpeed;

		particle->mInController = false;

		if(mMinColor!=mMaxColor)
		{
			particle->originalColor.r = Math::IntervalRandom(mMinColor.r, mMaxColor.r);
			particle->originalColor.g = Math::IntervalRandom(mMinColor.g, mMaxColor.g);
			particle->originalColor.b = Math::IntervalRandom(mMinColor.b, mMaxColor.b);
			particle->originalColor.a = Math::IntervalRandom(mMinColor.a, mMaxColor.a);
		}
		else
			particle->originalColor = mMinColor;


		if(mMinScale != mMaxScale)
			particle->originalScale = Math::IntervalRandom(mMinScale, mMaxScale);
		else
			particle->originalScale = mMinScale;

		if(mMin2dAngle != mMax2dAngle)
			particle->originalSelfRotation = Math::IntervalRandom(mMin2dAngle, mMax2dAngle);
		else
			particle->originalSelfRotation = mMin2dAngle;
	}

	int EffectLayerParticles::_update(i32 time)
	{
		//int currentParticlesNum = 0;

		if(EffectLayer::_update(time))
		{
			if(mbAffected)
			{
				/// 1 test emitter particles.
				mEmitterTime += time;
				while(mEmitterTime > mEmitterTimeStep && mInnerParticleQuota != 0)
				{
					--mInnerParticleQuota;
					mEmitterTime -= mEmitterTimeStep;
					// emitter a particle
					if(mParticlePoolFree.empty())
						break;

					EffectParticle* particle = mParticlePoolFree.front();
					mParticlePoolFree.pop_front();

					mParticlePoolInUse.push_back(particle);
					if (!mbStopEmitter)
					{
						emitterParticle(particle);
					}

					if(mFallowParent)
					{
						particle->mlastPosition = particle->position;
					}
					else
					{
						SceneNode* parentNode = mParentSystem->getEffectSceneNode();
						const Vector3& pscale = parentNode->getWorldScaling();
						const Quaternion& porita = parentNode->getWorldOrientation();
						const Vector3& ptrans = parentNode->getWorldPosition();
						particle->position = particle->position * pscale;
						particle->originalPosition = particle->position = (porita * originalOritation) * particle->position;

						//particle->mEmitOritation = porita * oritation;
						particle->scenePosition = ptrans + porita*position*pscale;
						particle->originalScale = particle->scale = pscale.x * particle->originalScale;
						particle->originalOritation = particle->oritation = porita * particle->oritation;

						//particle->position = mSystemOrientation * originalOritation * particle->position;
						if (!mbNoTransScale)
						{
							particle->position *= scale;
						}
						particle->position += particle->scenePosition;
						particle->mlastPosition = particle->position;
					}
				}

				/// 2 run all particles by the controllers.
				ParticlePoolItor Iter = mParticlePoolInUse.begin();
				for(; Iter!=mParticlePoolInUse.end(); )
				{
					EffectParticle* particle = *Iter;

					--particle->mReDirection;
					particle->mTime += time;
					if(particle->mTime > particle->mLeaveTime)
					{
						//drop the died particles.
						mParticlePoolInUse.erase(Iter++);
						mParticlePoolFree.push_back(particle);
						continue;
					}

					/// a controller test.
					particle->reParticleState();
					mParticleControllerSet->_updateParticle(particle);

					/// b slef movement.
					if(particle->originalSpeed != 0)
					{
						float particleTime = float(particle->mTime)*0.001f;
						float ds = particle->originalSpeed*particleTime + 0.5f*mParticleAddSpeed*particleTime*particleTime;
						particle->position += particle->movementDirection*ds;
					}

					/// c trans by parent layer.
					if(mFallowParent)
					{
						if (mbPerpendicular)
						{
							Vector3 deltaPos = particle->position - particle->mlastPosition;
							if (deltaPos.lenSqr() > 0.000001f)
							{
								particle->direction = deltaPos.normalizedCopy();
							}
							else
							{
								particle->direction = particle->originalDirection;
							}
							particle->mlastPosition = particle->position;
						}

						particle->position = oritation * particle->position;
						particle->oritation = oritation * particle->oritation;

						if (!mbPerpendicular)
							particle->direction = particle->oritation*particle->originalDirection;
						else
							particle->direction = oritation * particle->direction;

						if (!mbNoTransScale)
						{
							particle->position *= scale;
							particle->position += position;
						}
						else
						{
							particle->position += position;
						}
					}
					else  // world space.
					{
						if(!mbNoTransScale)
						{
							particle->position *= scale;
						}
						particle->position += particle->scenePosition;

						if (mbPerpendicular)
						{
							Vector3 deltaPos = particle->position - particle->mlastPosition;
							if (deltaPos.lenSqr() > 0.000001f)
							{
								particle->direction = deltaPos.normalizedCopy();
							}
							else
							{
								particle->direction = particle->originalDirection;
							}
							particle->mlastPosition = particle->position;
						}
					}

					particle->color = color * particle->color;

					//++currentParticlesNum;
					++Iter;
				}
			}
			return	mFrameIndex<mKeyFrames.size()||mbReplayIdel ? 1 : 0;
			//return currentParticlesNum + ((mEmitterTime <= mEmitterTimeStep && mInnerParticleQuota != 0) ? 1 : 0);
		}
		else return 0;
	}

	void EffectLayerParticles::clearPool()
	{
		ParticlePoolItor it = mParticlePoolInUse.begin();
		for(; it!=mParticlePoolInUse.end(); ++it)
		{
			LordDeleteT(*it, EffectParticle);
		}

		it = mParticlePoolFree.begin();
		for(; it!=mParticlePoolFree.end(); ++it)
		{
			LordDeleteT(*it, EffectParticle);
		}

		mParticlePoolInUse.clear();
		mParticlePoolFree.clear();
	}

	void EffectLayerParticles::createPool(size_t size)
	{
		for(size_t i=0; i<size; ++i)
		{
			EffectParticle* particle = LordNewT(EffectParticle)();
			mParticlePoolFree.push_back(particle);
		}
	}

	void EffectLayerParticles::_updateRenderQueue()
	{
		if((!mbAffected) || (!mbVisible))
			return;

		ui16 particleNum = (ui16)mParticlePoolInUse.size();

		EffectRenderable* pRenderable = EffectSystemManager::Instance()->getRenderable(mMaterial->getTexturePtr(), mRenderGroup, mMaterial->getBlendMode(), mMaterial, 4 * particleNum, 6 * particleNum, mViewSpace);
		if(!pRenderable)
			return;

		pRenderable->setSpecialRender(mParentSystem->IsSpecialRender());

		EffectVertexFmt* pVetLockPtr = NULL;
		ui16* pIdxLockPtr = NULL;
		ui16 vertBegin;
		ui16 particleUsed = 0;
		if(pRenderable->getLockPtr(4*particleNum, 6*particleNum, vertBegin, pVetLockPtr, pIdxLockPtr))
		{
			ParticlePoolItor Iter = mParticlePoolInUse.begin();
			for(; Iter!=mParticlePoolInUse.end(); ++Iter)
			{
				EffectParticle* particle = *Iter;
				if(particle->mReDirection<1)
				{
					renderBillboard(particle, pVetLockPtr);
					particleUsed++;
					pVetLockPtr += 4;
				}
			}
			// reflash index.
			for(ui16 i=0; i<particleUsed; ++i)
			{
				ui16 innerbegin = vertBegin + i*4;
				*pIdxLockPtr++ = innerbegin;
				*pIdxLockPtr++ = innerbegin+2;
				*pIdxLockPtr++ = innerbegin+1;
				*pIdxLockPtr++ = innerbegin+1;
				*pIdxLockPtr++ = innerbegin+2;
				*pIdxLockPtr++ = innerbegin+3;
			}

			pRenderable->reversePtr(4*(particleNum-particleUsed), 6*(particleNum-particleUsed));
		}
	}

	void EffectLayerParticles::renderBillboard(const EffectParticle* billboard, EffectVertexFmt* pVertexPtr)
	{
		float leftOff = -mCenterOffsetW;
		float rightOff = 1.0f - mCenterOffsetW;
		float topOff = -mCenterOffsetH;
		float bottomOff = 1.0f - mCenterOffsetH;
		Vector3 tmpBbPos = billboard->position;
		Vector3 vOwnOffset[4];
		float bwidth = mbNoScaleWidth?mWidth*scale:(mWidth*scale*billboard->scale);
		float bheight = mbNoScaleHeight?mHeight*scale:(mHeight*scale*billboard->scale);
		float tl = mMaterial->getTexAddrLeft();
		float tr = mMaterial->getTexAddrRight();
		float tt = mMaterial->getTexAddrTop();
		float tb = mMaterial->getTexAddrButton();
		bool swapUV = mMaterial->getSwapUV();
		ui32 colour = billboard->color.getABGR();

		if(mb3dBillboard && !mbPerpendicular)
		{
			leftOff = leftOff * bwidth;
			rightOff = rightOff * bwidth;
			topOff = topOff * bheight;
			bottomOff = bottomOff * bheight;

			if(mb3DBillboardTrans)
			{
				vOwnOffset[0] = Vector3(leftOff, 0, topOff);
				vOwnOffset[1] = Vector3(rightOff, 0, topOff);
				vOwnOffset[2] = Vector3(leftOff, 0, bottomOff);
				vOwnOffset[3] = Vector3(rightOff, 0, bottomOff);
			}
			else
			{
				vOwnOffset[0] = Vector3(leftOff, topOff, 0);
				vOwnOffset[1] = Vector3(rightOff, topOff, 0);
				vOwnOffset[2] = Vector3(leftOff, bottomOff, 0);
				vOwnOffset[3] = Vector3(rightOff, bottomOff, 0);
			}

			Quaternion selfOritation;
			if(Math::Abs(billboard->selfRotation)>1.f)
				selfOritation.rotateAxis(billboard->direction, billboard->selfRotation*Math::DEG2RAD);
			Matrix4 localMatrix;
			selfOritation = selfOritation * billboard->oritation;
			localMatrix.fromQuan(selfOritation);
			localMatrix.translate(tmpBbPos);
			if(mFallowParent)
			{
				localMatrix = localMatrix * mParentSystem->getEffectSceneNode()->getWorldMatrix();
			}

			vOwnOffset[0] = vOwnOffset[0] * localMatrix;
			vOwnOffset[1] = vOwnOffset[1] * localMatrix;
			vOwnOffset[2] = vOwnOffset[2] * localMatrix;
			vOwnOffset[3] = vOwnOffset[3] * localMatrix;
		}
		else
		{
			Vector3 camX, camY;
			Quaternion camQ;
			if(mFallowParent)
				camQ = mParentSystem->getCameraOriInEffect();
			else
				camQ = mParentSystem->getCurrentCamera()->getOritation();
			if(mb3dBillboard)
			{
				Vector3 camDir;
				if (mParentSystem->getCurrentCamera()->getProjectionMode() == Camera::PM_ORTHO)
				{
					camDir = -mParentSystem->getCameraOriInEffect().zAxis();
				}
				else
				{
					camDir = (tmpBbPos - mParentSystem->getCameraPosInEffect()).normalizedCopy();
				}
				camY = billboard->direction;
				camX = camDir.cross(Vector3::UNIT_X);
				camX.normalize();
			}
			else
			{
				camX = camQ * Vector3::UNIT_X;
				camY = camQ * Vector3::UNIT_Y;
			}
			Vector3 vLeftOff, vRightOff, vTopOff, vBottomOff;
			vLeftOff   = camX * ( leftOff   * bwidth );
			vRightOff  = camX * ( rightOff  * bwidth );
			vTopOff    = camY * ( topOff   * bheight );
			vBottomOff = camY * ( bottomOff * bheight );

			// Make final offsets to vertex positions
			vOwnOffset[0] = vLeftOff  + vTopOff;
			vOwnOffset[1] = vRightOff + vTopOff;
			vOwnOffset[2] = vLeftOff  + vBottomOff;
			vOwnOffset[3] = vRightOff + vBottomOff;

			Vector3 axis = (vOwnOffset[3] - vOwnOffset[0]).cross(vOwnOffset[2] - vOwnOffset[1]).normalizedCopy();

			if(Math::Abs(billboard->selfRotation) > 1.f)
			{
				Quaternion rotateSelf;
				rotateSelf.rotateAxis(axis, billboard->selfRotation*Math::DEG2RAD);

				vOwnOffset[0] = rotateSelf * vOwnOffset[0];
				vOwnOffset[1] = rotateSelf * vOwnOffset[1];
				vOwnOffset[2] = rotateSelf * vOwnOffset[2];
				vOwnOffset[3] = rotateSelf * vOwnOffset[3];
			}

			vOwnOffset[0] = vOwnOffset[0] + tmpBbPos;
			vOwnOffset[1] = vOwnOffset[1] + tmpBbPos;
			vOwnOffset[2] = vOwnOffset[2] + tmpBbPos;
			vOwnOffset[3] = vOwnOffset[3] + tmpBbPos;

			if(mFallowParent)
			{
				Matrix4 mtxWorld = mParentSystem->getEffectSceneNode()->getWorldMatrix();
				vOwnOffset[0] = vOwnOffset[0] * mtxWorld;
				vOwnOffset[1] = vOwnOffset[1] * mtxWorld;
				vOwnOffset[2] = vOwnOffset[2] * mtxWorld;
				vOwnOffset[3] = vOwnOffset[3] * mtxWorld;
			}		
		}

		pVertexPtr[0].pos = vOwnOffset[0];
		pVertexPtr[0].color = colour;
		pVertexPtr[1].pos = vOwnOffset[1];
		pVertexPtr[1].color = colour;
		pVertexPtr[2].pos = vOwnOffset[2];
		pVertexPtr[2].color = colour;
		pVertexPtr[3].pos = vOwnOffset[3];
		pVertexPtr[3].color = colour;

		if(swapUV)
		{
			pVertexPtr[0].tx = tr;
			pVertexPtr[0].ty = tb;
			pVertexPtr[1].tx = tr;
			pVertexPtr[1].ty = tt;
			pVertexPtr[2].tx = tl;
			pVertexPtr[2].ty = tb;
			pVertexPtr[3].tx = tl;
			pVertexPtr[3].ty = tt;
		}
		else
		{
			pVertexPtr[0].tx = tl;
			pVertexPtr[0].ty = tb;
			pVertexPtr[1].tx = tr;
			pVertexPtr[1].ty = tb;
			pVertexPtr[2].tx = tl;
			pVertexPtr[2].ty = tt;
			pVertexPtr[3].tx = tr;
			pVertexPtr[3].ty = tt;
		}
	}

	void EffectLayerParticles::copyAttributesTo(EffectParticle* layer)
	{
		EffectLayer::copyAttributesTo(layer);
		EffectLayerParticles* Particles = static_cast<EffectLayerParticles*>(layer);
		Particles->destroyParticleControllerSet();

		Particles->mParticleQuota = mParticleQuota;
		Particles->mb3dBillboard = mb3dBillboard;
		Particles->mbPerpendicular = mbPerpendicular;
		Particles->mb3DBillboardTrans = mb3DBillboardTrans;
		Particles->mbNoTransScale = mbNoTransScale;
		Particles->mEmitterRate = mEmitterRate;
		Particles->mEmitterAngle = mEmitterAngle;
		Particles->mParticleSpeed = mParticleSpeed;
		Particles->mParticleMaxSpeed = mParticleMaxSpeed;
		Particles->mParticleAddSpeed = mParticleAddSpeed;
		Particles->mParticleLeaveTime = mParticleLeaveTime;
		Particles->mParticleMaxLeaveTime = mParticleMaxLeaveTime;
		Particles->mMinColor = mMinColor;
		Particles->mMaxColor = mMaxColor;
		Particles->mMinScale = mMinScale;
		Particles->mMaxScale = mMaxScale;
		Particles->mMin2dAngle = mMin2dAngle;
		Particles->mMax2dAngle = mMax2dAngle;
		Particles->mFallowParent = mFallowParent;
		Particles->mEmitterInSurface = mEmitterInSurface;
		Particles->mWidth = mWidth;
		Particles->mHeight = mHeight;
		Particles->mbNoScaleWidth = mbNoScaleWidth;
		Particles->mbNoScaleHeight = mbNoScaleHeight;
		Particles->mCenterOffsetW = mCenterOffsetW;
		Particles->mCenterOffsetH = mCenterOffsetH;

		Particles->setParticleControllerSet(EffectSystemManager::Instance()->cloneParticleControllerSet(mParticleControllerSet));
	}

	void EffectLayerParticles::exportData(DataStream* pStream, int version)
	{
		EffectLayer::exportData(pStream, version);
		mParticleControllerSet->exportData(pStream, version);
		pStream->write(&mParticleQuota, sizeof(i32));
		pStream->write(&mb3dBillboard, sizeof(bool));
		pStream->write(&mbPerpendicular, sizeof(bool));
		pStream->write(&mb3DBillboardTrans, sizeof(bool));
		pStream->write(&mbNoTransScale, sizeof(bool));
		pStream->write(&mEmitterRate, sizeof(i32));
		pStream->write(&mEmitterAngle, sizeof(float));
		pStream->write(&mParticleSpeed, sizeof(float));
		pStream->write(&mParticleMaxSpeed,sizeof(float));
		pStream->write(&mParticleAddSpeed, sizeof(float));
		pStream->write(&mParticleLeaveTime, sizeof(i32));
		pStream->write(&mParticleMaxLeaveTime, sizeof(i32));
		pStream->write(&mMinColor, sizeof(Color));
		pStream->write(&mMaxColor, sizeof(Color));
		pStream->write(&mMinScale, sizeof(float));
		pStream->write(&mMaxScale, sizeof(float));
		pStream->write(&mMin2dAngle, sizeof(float));
		pStream->write(&mMax2dAngle, sizeof(float));
		pStream->write(&mFallowParent, sizeof(bool));
		pStream->write(&mEmitterInSurface, sizeof(bool));
		pStream->write(&mWidth, sizeof(float));
		pStream->write(&mHeight, sizeof(float));
		pStream->write(&mbNoScaleWidth, sizeof(bool));
		pStream->write(&mbNoScaleHeight, sizeof(bool));
		pStream->write(&mCenterOffsetW, sizeof(float));
		pStream->write(&mCenterOffsetH, sizeof(float));
	}

	void EffectLayerParticles::importData(DataStream* pStream, int version)
	{
		EffectLayer::importData(pStream, version);
		mParticleControllerSet->importData(pStream, version);
		pStream->read(&mParticleQuota, sizeof(i32));
		pStream->read(&mb3dBillboard, sizeof(bool));
		pStream->read(&mbPerpendicular, sizeof(bool));
		pStream->read(&mb3DBillboardTrans, sizeof(bool));
		pStream->read(&mbNoTransScale, sizeof(bool));
		pStream->read(&mEmitterRate, sizeof(i32));
		pStream->read(&mEmitterAngle, sizeof(float));
		pStream->read(&mParticleSpeed, sizeof(float));
		pStream->read(&mParticleMaxSpeed,sizeof(float));
		pStream->read(&mParticleAddSpeed, sizeof(float));
		pStream->read(&mParticleLeaveTime, sizeof(i32));
		pStream->read(&mParticleMaxLeaveTime,sizeof(i32));
		pStream->read(&mMinColor, sizeof(Color));
		pStream->read(&mMaxColor, sizeof(Color));
		pStream->read(&mMinScale, sizeof(float));
		pStream->read(&mMaxScale, sizeof(float));
		pStream->read(&mMin2dAngle, sizeof(float));
		pStream->read(&mMax2dAngle, sizeof(float));
		pStream->read(&mFallowParent, sizeof(bool));
		pStream->read(&mEmitterInSurface, sizeof(bool));
		pStream->read(&mWidth, sizeof(float));
		pStream->read(&mHeight, sizeof(float));
		pStream->read(&mbNoScaleWidth, sizeof(bool));
		pStream->read(&mbNoScaleHeight, sizeof(bool));
		pStream->read(&mCenterOffsetW, sizeof(float));
		pStream->read(&mCenterOffsetH, sizeof(float));
	}

	void EffectLayerParticles::mergeAligenBox(Box& aabb)
	{
		if (!mbAffected)
			return;

		ParticlePoolItor Iter = mParticlePoolInUse.begin();
		for(; Iter!=mParticlePoolInUse.end(); ++Iter)
		{
			EffectParticle* particle = *Iter;
			if(particle->mReDirection<1)
			{
				float fWidth = mbNoScaleWidth ? (mWidth*scale) : (mWidth*scale*particle->scale);
				float fHeight = mbNoScaleHeight ? (mHeight*scale) : (mHeight*scale*particle->scale);
				float fAdjust = Math::Max(fWidth/2.f, fHeight/2.f);
				Vector3 vecAdjust(fAdjust, fAdjust, fAdjust);
				aabb.addPoint(particle->position - vecAdjust);
				aabb.addPoint(particle->position + vecAdjust);
			}
		}
	}

	void EffectLayerParticles::getPropertyList(PropertyList& list)
	{
		EffectLayer::getPropertyList(list);
		list.push_back(token[TOKEN_LY_PT_QUOTA]);
		list.push_back(token[TOKEN_LY_PT_WIDTH]);
		list.push_back(token[TOKEN_LY_PT_HEIGHT]);
		list.push_back(token[TOKEN_LY_PT_3DBILLBOARD]);
		list.push_back(token[TOKEN_LY_PT_3DTRANS]);
		list.push_back(token[TOKEN_LY_PT_PERPENDICULAR]);
		list.push_back(token[TOKEN_LY_PT_NOTRANSSCALE]);
		list.push_back(token[TOKEN_LY_PT_NOSCALEWIDTH]);
		list.push_back(token[TOKEN_LY_PT_NOSCALEHEIGHT]);
		list.push_back(token[TOKEN_LY_PT_CENTEROFFSETW]);
		list.push_back(token[TOKEN_LY_PT_CENTEROFFSETH]);
		list.push_back(token[TOKEN_LY_PT_EMITTERRATE]);
		list.push_back(token[TOKEN_LY_PT_EMITTERANGLE]);
		list.push_back(token[TOKEN_LY_PT_SPEED]);
		list.push_back(token[TOKEN_LY_PT_MAXSPEED]);
		list.push_back(token[TOKEN_LY_PT_ADDSPEED]);
		list.push_back(token[TOKEN_LY_PT_LEAVETIME]);
		list.push_back(token[TOKEN_LY_PT_MAXLEAVETIME]);
		list.push_back(token[TOKEN_LY_PT_MINCOLOR]);
		list.push_back(token[TOKEN_LY_PT_MAXCOLOR]);
		list.push_back(token[TOKEN_LY_PT_MINSCALE]);
		list.push_back(token[TOKEN_LY_PT_MAXSCALE]);
		list.push_back(token[TOKEN_LY_PT_MIN2DANGLE]);
		list.push_back(token[TOKEN_LY_PT_MAX2DANGLE]);
		list.push_back(token[TOKEN_LY_PT_FALLOWPARENT]);
		list.push_back(token[TOKEN_LY_PT_EMITTERINSURFACE]);
	}

	bool EffectLayerParticles::getPropertyType(const String& name, PropertyType& type)
	{
		if(EffectLayer::getPropertyType(name, type))
			return true;
		if (name == token[TOKEN_LY_PT_QUOTA])
		{
			type = IElement::PT_INT; return true;
		}
		else if(name == token[TOKEN_LY_PT_WIDTH])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_PT_HEIGHT])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_PT_3DBILLBOARD])
		{
			type = IElement::PT_BOOL; return true;
		}
		else if(name == token[TOKEN_LY_PT_PERPENDICULAR])
		{
			type = IElement::PT_BOOL; return true;
		}
		else if(name == token[TOKEN_LY_PT_3DTRANS])
		{
			type = IElement::PT_BOOL; return true;
		}
		else if(name == token[TOKEN_LY_PT_NOTRANSSCALE])
		{
			type = IElement::PT_BOOL; return true;
		}
		else if(name == token[TOKEN_LY_PT_NOSCALEWIDTH])
		{
			type = IElement::PT_BOOL; return true;
		}
		else if(name == token[TOKEN_LY_PT_NOSCALEHEIGHT])
		{
			type = IElement::PT_BOOL; return true;
		}
		else if(name == token[TOKEN_LY_PT_CENTEROFFSETW])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_PT_CENTEROFFSETH])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_PT_EMITTERRATE])
		{
			type = IElement::PT_INT; return true;
		}
		else if(name == token[TOKEN_LY_PT_EMITTERANGLE])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_PT_SPEED])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_PT_MAXSPEED])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_PT_ADDSPEED])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_PT_LEAVETIME])
		{
			type = IElement::PT_INT; return true;
		}
		else if(name == token[TOKEN_LY_PT_MAXLEAVETIME])
		{
			type = IElement::PT_INT; return true;
		}
		else if(name == token[TOKEN_LY_PT_MINCOLOR])
		{
			type = IElement::PT_COLOR; return true;
		}
		else if(name == token[TOKEN_LY_PT_MAXCOLOR])
		{
			type = IElement::PT_COLOR; return true;
		}
		else if(name == token[TOKEN_LY_PT_MINSCALE])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_PT_MAXSCALE])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_PT_MIN2DANGLE])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_PT_MAX2DANGLE])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_PT_FALLOWPARENT])
		{
			type = IElement::PT_BOOL; return true;
		}
		else if(name == token[TOKEN_LY_PT_EMITTERINSURFACE])
		{
			type = IElement::PT_BOOL; return true;
		}
		return false;
	}

	bool EffectLayerParticles::getPropertyValue(const String& name, String& value)
	{
		if(EffectLayer::getPropertyValue(name, value))
			return true;

		if (name == token[TOKEN_LY_PT_QUOTA])
		{
			value = StringUtil::ToString(mParticleQuota); return true;
		}
		else if(name == token[TOKEN_LY_PT_WIDTH])
		{
			value = StringUtil::ToString(mWidth); return true;
		}
		else if(name == token[TOKEN_LY_PT_HEIGHT])
		{
			value = StringUtil::ToString(mHeight); return true;
		}
		else if(name == token[TOKEN_LY_PT_3DBILLBOARD])
		{
			value = StringUtil::ToString(mb3dBillboard); return true;
		}
		else if(name == token[TOKEN_LY_PT_PERPENDICULAR])
		{
			value = StringUtil::ToString(mbPerpendicular); return true;
		}
		else if(name == token[TOKEN_LY_PT_3DTRANS])
		{
			value = StringUtil::ToString(mb3DBillboardTrans); return true;
		}
		else if(name == token[TOKEN_LY_PT_NOTRANSSCALE])
		{
			value = StringUtil::ToString(mbNoTransScale); return true;
		}
		else if(name == token[TOKEN_LY_PT_NOSCALEWIDTH])
		{
			value = StringUtil::ToString(mbNoScaleWidth); return true;
		}
		else if(name == token[TOKEN_LY_PT_NOSCALEHEIGHT])
		{
			value = StringUtil::ToString(mbNoScaleHeight); return true;
		}
		else if(name == token[TOKEN_LY_PT_CENTEROFFSETW])
		{
			value = StringUtil::ToString(mCenterOffsetW); return true;
		}
		else if(name == token[TOKEN_LY_PT_CENTEROFFSETH])
		{
			value = StringUtil::ToString(mCenterOffsetH); return true;
		}
		else if(name == token[TOKEN_LY_PT_EMITTERRATE])
		{
			value = StringUtil::ToString(mEmitterRate); return true;
		}
		else if(name == token[TOKEN_LY_PT_EMITTERANGLE])
		{
			value = StringUtil::ToString(mEmitterAngle); return true;
		}
		else if(name == token[TOKEN_LY_PT_SPEED])
		{
			value = StringUtil::ToString(mParticleSpeed); return true;
		}
		else if(name == token[TOKEN_LY_PT_MAXSPEED])
		{
			value = StringUtil::ToString(mParticleMaxSpeed); return true;
		}
		else if(name == token[TOKEN_LY_PT_ADDSPEED])
		{
			value = StringUtil::ToString(mParticleAddSpeed); return true;
		}
		else if(name == token[TOKEN_LY_PT_LEAVETIME])
		{
			value = StringUtil::ToString(mParticleLeaveTime); return true;
		}
		else if(name == token[TOKEN_LY_PT_MAXLEAVETIME])
		{
			value = StringUtil::ToString(mParticleMaxLeaveTime); return true;
		}
		else if(name == token[TOKEN_LY_PT_MINCOLOR])
		{
			value = StringUtil::ToString(mMinColor); return true;
		}
		else if(name == token[TOKEN_LY_PT_MAXCOLOR])
		{
			value = StringUtil::ToString(mMaxColor); return true;
		}
		else if(name == token[TOKEN_LY_PT_MINSCALE])
		{
			value = StringUtil::ToString(mMinScale); return true;
		}
		else if(name == token[TOKEN_LY_PT_MAXSCALE])
		{
			value = StringUtil::ToString(mMaxScale); return true;
		}
		else if(name == token[TOKEN_LY_PT_MIN2DANGLE])
		{
			value = StringUtil::ToString(mMin2dAngle); return true;
		}
		else if(name == token[TOKEN_LY_PT_MAX2DANGLE])
		{
			value = StringUtil::ToString(mMax2dAngle); return true;
		}
		else if(name == token[TOKEN_LY_PT_FALLOWPARENT])
		{
			value = StringUtil::ToString(mFallowParent); return true;
		}
		else if(name == token[TOKEN_LY_PT_EMITTERINSURFACE])
		{
			value = StringUtil::ToString(mEmitterInSurface); return true;
		}

		return false;
	}

	bool EffectLayerParticles::setPropertyValue(const String& name,const String& value)
	{
		if(EffectLayer::setPropertyValue(name, value))
			return true;

		if (name == token[TOKEN_LY_PT_QUOTA])
		{
			mParticleQuota = StringUtil::ParseInt(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_WIDTH])
		{
			mWidth = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_HEIGHT])
		{
			mHeight = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_3DBILLBOARD])
		{
			mb3dBillboard = StringUtil::ParseBool(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_PERPENDICULAR])
		{
			mbPerpendicular = StringUtil::ParseBool(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_3DTRANS])
		{
			mb3DBillboardTrans = StringUtil::ParseBool(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_NOTRANSSCALE])
		{
			mbNoTransScale = StringUtil::ParseBool(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_NOSCALEWIDTH])
		{
			mbNoScaleWidth = StringUtil::ParseBool(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_NOSCALEHEIGHT])
		{
			mbNoScaleHeight = StringUtil::ParseBool(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_CENTEROFFSETW])
		{
			mCenterOffsetW = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_CENTEROFFSETH])
		{
			mCenterOffsetH = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_EMITTERRATE])
		{
			mEmitterRate = StringUtil::ParseInt(value); 
			if(mEmitterRate < 1)
			{
				mEmitterRate = 1;
			}
			return true;
		}
		else if(name == token[TOKEN_LY_PT_EMITTERANGLE])
		{
			mEmitterAngle = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_SPEED])
		{
			mParticleSpeed = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_MAXSPEED])
		{
			mParticleMaxSpeed = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_ADDSPEED])
		{
			mParticleAddSpeed = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_LEAVETIME])
		{
			mParticleLeaveTime = StringUtil::ParseInt(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_MAXLEAVETIME])
		{
			mParticleMaxLeaveTime = StringUtil::ParseInt(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_MINCOLOR])
		{
			mMinColor = StringUtil::ParseColor(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_MAXCOLOR])
		{
			mMaxColor = StringUtil::ParseColor(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_MINSCALE])
		{
			mMinScale = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_MAXSCALE])
		{
			mMaxScale = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_MIN2DANGLE])
		{
			mMin2dAngle = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_MAX2DANGLE])
		{
			mMax2dAngle = StringUtil::ParseReal(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_FALLOWPARENT])
		{
			mFallowParent = StringUtil::ParseBool(value); return true;
		}
		else if(name == token[TOKEN_LY_PT_EMITTERINSURFACE])
		{
			mEmitterInSurface = StringUtil::ParseBool(value); return true;
		}
		return false;
	}

	void EffectLayerParticles::_notifyStop()
	{
		mbStopEmitter = true;
	}

}