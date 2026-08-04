#include "Core.h"
#include "EffectLayer3DGrid.h"
#include "EffectSystemManager.h"
#include "Scene/SceneNode.h"
#include "Resource/DataStream.h"

namespace LORD
{
	EffectLayer3DGrid::EffectLayer3DGrid()
		: EffectLayer()
		, mRows(4)
		, mColumns(4)
		, mInterval(0.3f)
		, mpVectorArray(NULL)
		, mpColorArray(NULL)
		, mOffViewport(0)
		, mCallbackFunction(NULL)
	{
		direction = Vector3::UNIT_Z;
		originalDirection = Vector3::UNIT_Z;
		mType = ELT_3DEditableSurface;
	}

	EffectLayer3DGrid::~EffectLayer3DGrid()
	{
		LordSafeFree(mpVectorArray);
		LordSafeFree(mpColorArray);
	}

	void EffectLayer3DGrid::setCallBackFunction(ES_FUN fun)
	{
		mCallbackFunction = fun;
	}

	void EffectLayer3DGrid::prepare()
	{
		EffectLayer::prepare();
		LordAssert(mRows>1);
		LordAssert(mColumns>1);
		LordAssert(mInterval>0.001f);
		if(!mpVectorArray)
		{
			mpVectorArray = (Vector3*)(LordMalloc(sizeof(Vector3)*mColumns*mRows));

			float xbegin = -((mColumns-1)*mInterval/2.0f);
			float ybegin = ((mRows-1)*mInterval/2.0f);
			for(size_t i=0; i<mRows; ++i)
			{
				for(size_t j=0; j<mColumns; ++j)
				{
					Vector3& vec = mpVectorArray[i*mColumns+j];
					vec.x = xbegin + j*mInterval;
					vec.y = ybegin - i*mInterval;
					vec.z = 0.0f;
				}
			}
		}
		if(!mpColorArray)
		{
			mpColorArray = (Color*)(LordMalloc(sizeof(Color)*mColumns*mRows));

			for(size_t i=0; i<mRows; ++i)
			{
				for(size_t j=0; j<mColumns; ++j)
				{
					mpColorArray[i*mColumns+j] = Color::WHITE;
				}
			}
		}
		mGridBox.reset();
		for(size_t i=0; i<mRows; ++i)
		{
			for(size_t j=0; j<mColumns; ++j)
			{
				mGridBox.addPoint(mpVectorArray[i*mColumns+j]);
			}
		}
	}

	void EffectLayer3DGrid::unprepare()
	{
		EffectLayer::unprepare();
	}

	void EffectLayer3DGrid::_adjustGridRows(ui32 rows)
	{
		if(rows < 2)
			return ;

		if(mRows != rows)
		{
			if(mpVectorArray && mpColorArray)
			{
				ui32 newSize = rows * mColumns;
				Vector3* pNewVectorArray = (Vector3*)LordMalloc(sizeof(Vector3)*newSize);
				Color* pNewColorArray = (Color*)LordMalloc(sizeof(Color)*newSize);
				for(ui32 i=0; i<rows; ++i)
				{
					for(ui32 j=0; j<mColumns; ++j)
					{
						if(i<mRows)
						{
							pNewVectorArray[i*(mColumns)+j] = mpVectorArray[i*(mColumns)+j];
							pNewColorArray[i*(mColumns)+j] = mpColorArray[i*(mColumns)+j];
						}
						else
						{
							pNewVectorArray[i*(mColumns)+j] = pNewVectorArray[(i-1)*(mColumns)+j];
							pNewVectorArray[i*(mColumns)+j].y -= mInterval;
							pNewColorArray[i*(mColumns)+j] = Color::WHITE;
						}
					}
				}
				float offset = mInterval*(int(rows)-int(mRows))/2.0f;
				for(size_t i=0; i<newSize; ++i)
					pNewVectorArray[i].y += offset;

				LordSafeFree(mpVectorArray);
				LordSafeFree(mpColorArray);
				mpVectorArray = pNewVectorArray;
				mpColorArray = pNewColorArray;
			}
			mRows = rows;
		}
	}

	void EffectLayer3DGrid::_adjustGridColumns(ui32 columns)
	{
		if(columns < 2)
			return;

		if(mColumns != columns)
		{
			if(mpVectorArray && mpColorArray)
			{
				size_t newSize = (columns) * (mRows);
				Vector3* pNewVectorArray = (Vector3*)LordMalloc(sizeof(Vector3)*newSize);
				Color* pNewColorArray = (Color*)LordMalloc(sizeof(Color)*newSize);
				for(size_t i=0; i<mRows; ++i)
				{
					Vector3 edgeVector = mpVectorArray[(i+1)*(mColumns)-1];
					Color edgeColor = mpColorArray[(i+1)*(mColumns)-1];
					for(size_t j=0; j<columns; ++j)
					{
						if(j<mColumns)
						{
							pNewVectorArray[i*(columns)+j] = mpVectorArray[i*(mColumns)+j];
							pNewColorArray[i*(columns)+j] = mpColorArray[i*(mColumns)+j];
						}
						else
						{
							edgeVector.x += mInterval;
							pNewVectorArray[i*(columns)+j] = edgeVector;
							pNewColorArray[i*(columns)+j] = Color::WHITE;
						}
					}
				}

				float offset = -mInterval*(int(columns) - int(mColumns))/2.0f;
				for(size_t i=0; i<newSize; ++i)
					pNewVectorArray[i].x += offset;

				LordSafeFree(mpVectorArray);
				LordSafeFree(mpColorArray);
				mpVectorArray = pNewVectorArray;
				mpColorArray = pNewColorArray;
			}
			mColumns = columns;
		}
	}

	void EffectLayer3DGrid::_adjustGridInterval(float interval)
	{
		if(interval != mInterval)
		{
			mInterval = interval;
			if(mpVectorArray)
			{
				float xbegin = -((mColumns-1)*mInterval/2.0f);
				float ybegin = ((mRows-1)*mInterval/2.0f);
				for(size_t i=0; i<mRows; ++i)
				{
					for(size_t j=0; j<mColumns; ++j)
					{
						Vector3& vec = mpVectorArray[i*mColumns+j];
						vec.x = xbegin + j*mInterval;
						vec.y = ybegin - i*mInterval;
						vec.z = 0.0f;

						Color& color = mpColorArray[i*mColumns+j];
						color = Color::WHITE;
					}
				}
			}
		}
	}

	//-----------------------------------------------------------------------
	Vector3 EffectLayer3DGrid::getSurfaceVector(int row, int column)const
	{
		Vector3 ret = Vector3::ZERO;
		if(mpVectorArray)
		{
			ret = mpVectorArray[row*mColumns+column];
		}

		return ret;
	}
	//-----------------------------------------------------------------------
	void EffectLayer3DGrid::setSurfaceVector(int row, int column, const Vector3& newPos)
	{
		if(mpVectorArray)
		{
			mpVectorArray[row*mColumns+column] = newPos;
		}
	}
	//-----------------------------------------------------------------------
	Vector3 EffectLayer3DGrid::getSurfaceVector(int index)const
	{
		Vector3 ret = Vector3::ZERO;
		if(mpVectorArray)
		{
			ret = mpVectorArray[index];
		}

		return ret;
	}
	//-----------------------------------------------------------------------
	void EffectLayer3DGrid::setSurfaceVector(int index, const Vector3& newPos)
	{
		if(mpVectorArray)
		{
			mpVectorArray[index] = newPos;
		}
	}
	//-----------------------------------------------------------------------
	Color EffectLayer3DGrid::getSurfaceColor(int row, int column)const
	{
		Color ret = Color::WHITE;
		if(mpColorArray)
		{
			ret = mpColorArray[row*mColumns+column];
		}

		return ret;
	}
	//-----------------------------------------------------------------------
	void EffectLayer3DGrid::setSurfaceColor(int row, int column, const Color& newColor)
	{
		if(mpColorArray)
		{
			mpColorArray[row*mColumns+column] = newColor;
		}
	}
	//-----------------------------------------------------------------------
	Color EffectLayer3DGrid::getSurfaceColor(int index)const
	{
		Color ret = Color::WHITE;
		if(mpColorArray)
		{
			ret = mpColorArray[index];
		}

		return ret;
	}
	//-----------------------------------------------------------------------
	void EffectLayer3DGrid::setSurfaceColor(int index, const Color& newColor)
	{
		if(mpColorArray)
		{
			mpColorArray[index] = newColor;
		}
	}

	const Quaternion& EffectLayer3DGrid::getSurfaceOritation() const
	{
		return oritation;
	}

	void EffectLayer3DGrid::_updateRenderQueue()
	{
		if ((!mbAffected) || (!mbVisible))
			return;

		EffectRenderable* pRenderable = EffectSystemManager::Instance()->getRenderable(mMaterial->getTexturePtr(), mRenderGroup, mMaterial->getBlendMode(), mMaterial, mColumns*mRows, 6 * (mRows - 1)*(mColumns - 1), mViewSpace);
		if(!pRenderable)
			return;

		pRenderable->setSpecialRender(mParentSystem->IsSpecialRender());

		EffectVertexFmt* pVertexPtr = NULL;
		ui16* pIdxLockPtr = NULL;
		ui16 vertBegin;
		if(pRenderable->getLockPtr(mColumns*mRows, 6*(mRows-1)*(mColumns-1), vertBegin, pVertexPtr, pIdxLockPtr))
		{
			//ui32 colour = color.getABGR();

			Matrix4 mtxWorld;
			getMatrixWorld(mtxWorld);

			ui32 i,j;
			float tl = mMaterial->getTexAddrLeft();
			float tr = mMaterial->getTexAddrRight();
			float tt = mMaterial->getTexAddrTop();
			float tb = mMaterial->getTexAddrButton();
			float txSeg = (tr-tl)/(mColumns-1);
			float tySeg = (tb-tt)/(mRows-1);
			bool swapUV = mMaterial->getSwapUV();
			Color gridColor;
			for(i=0; i<mRows; ++i)
			{
				for(j=0; j<mColumns; ++j)
				{
					pVertexPtr->pos = mpVectorArray[i*mColumns+j] * mtxWorld;
					gridColor = color * mpColorArray[i*mColumns+j];
					pVertexPtr->color = gridColor.getABGR();
					if(swapUV)
					{
						pVertexPtr->tx = tl+i*txSeg;
						pVertexPtr->ty = tb-j*tySeg;
					}
					else
					{
						pVertexPtr->tx = tl+j*txSeg;
						pVertexPtr->ty = tt+i*tySeg;
					}
					++pVertexPtr;
				}
			}

			for(i=0; i<mRows-1; ++i)
			{
				for(j=0; j<mColumns-1; ++j)
				{
					*pIdxLockPtr++ = vertBegin + i*mColumns+j;
					*pIdxLockPtr++ = vertBegin + (i+1)*mColumns+j;
					*pIdxLockPtr++ = vertBegin + i*mColumns+j+1;

					*pIdxLockPtr++ = vertBegin + (i+1)*mColumns+j;
					*pIdxLockPtr++ = vertBegin + (i+1)*mColumns+j+1;
					*pIdxLockPtr++ = vertBegin + i*mColumns+j+1;
				}
			}

		}
	}

	void EffectLayer3DGrid::getMatrixWorld(Matrix4& mtxWorld)
	{
		mtxWorld = mParentSystem->getEffectSceneNode()->getWorldMatrix();

		Matrix4 mtxLocal;
		mtxLocal.makeScaling(scale, scale, scale);

		Matrix4 mtxRot;
		Quaternion quanRot;
		quanRot.rotateAxis(originalDirection, selfRotation*Math::DEG2RAD);
		quanRot = quanRot*oritation;
		quanRot.toMat4(mtxRot);
		mtxLocal = mtxLocal * mtxRot;

		mtxLocal.translate(position);
		mtxWorld = mtxLocal * mtxWorld;
	}

	void EffectLayer3DGrid::copyAttributesTo(EffectParticle* particle)
	{
		EffectLayer::copyAttributesTo(particle);
		EffectLayer3DGrid* grid = static_cast<EffectLayer3DGrid*>(particle);
		grid->mRows = mRows;
		grid->mColumns = mColumns;
		grid->mInterval = mInterval;
		LordFree(grid->mpVectorArray);
		LordFree(grid->mpColorArray);

		grid->mpVectorArray = (Vector3*)LordMalloc(sizeof(Vector3)*mRows*mColumns);
		grid->mpColorArray = (Color*)LordMalloc(sizeof(Color)*mRows*mColumns);
		memcpy(grid->mpVectorArray, mpVectorArray, sizeof(Vector3)*mRows*mColumns);
		memcpy(grid->mpColorArray, mpColorArray, sizeof(Color)*mRows*mColumns);
		grid->mOffViewport = mOffViewport;
	}

	void EffectLayer3DGrid::exportData(DataStream* pStream, int version)
	{
		EffectLayer::exportData(pStream, version);
		pStream->write(&mRows, sizeof(ui32));
		pStream->write(&mColumns, sizeof(ui32));
		pStream->write(&mInterval, sizeof(float));
		pStream->write(&mOffViewport, sizeof(float));
		pStream->write(mpVectorArray, sizeof(Vector3)*mRows*mColumns);
		pStream->write(mpColorArray, sizeof(Color)*mRows*mColumns);
	}

	void EffectLayer3DGrid::importData(DataStream* pStream, int version)
	{
		EffectLayer::importData(pStream, version);
		pStream->read(&mRows, sizeof(ui32));
		pStream->read(&mColumns, sizeof(ui32));
		pStream->read(&mInterval, sizeof(ui32));
		pStream->read(&mOffViewport, sizeof(float));
		LordAssertX(mpColorArray==NULL, "Error: EffectLayer3DGrid::importData mpColorArray!=NULL. ");
		LordAssertX(mpVectorArray==NULL, "Error: EffectLayer3DGrid::importData mpVectorArray!=NULL. ");
		mpVectorArray = (Vector3*)LordMalloc(sizeof(Vector3)*mRows*mColumns);
		mpColorArray = (Color*)LordMalloc(sizeof(Color)*mRows*mColumns);
		pStream->read(mpVectorArray, sizeof(Vector3)*mRows*mColumns);
		pStream->read(mpColorArray, sizeof(Color)*mRows*mColumns);
		mGridBox.reset();
		for(size_t i=0; i<mRows; ++i)
		{
			for(size_t j=0; j<mColumns; ++j)
			{
				mGridBox.addPoint(mpVectorArray[i*mColumns+j]);
			}
		}
	}

	void EffectLayer3DGrid::mergeAligenBox(Box& aabb)
	{
		if (!mbAffected)
			return;

		Matrix4 mtxLocal;
		mtxLocal.makeScaling(scale, scale, scale);

		Matrix4 mtxRot;
		Quaternion quanRot;
		quanRot.rotateAxis(originalDirection, selfRotation*Math::DEG2RAD);
		quanRot = quanRot*oritation;
		quanRot.toMat4(mtxRot);
		mtxLocal = mtxLocal * mtxRot;

		mtxLocal.translate(position);

		for(size_t i=0; i<mRows*mColumns; ++i)
		{
			aabb.addPoint(mpVectorArray[i]*mtxLocal);
		}
	}

	void EffectLayer3DGrid::getPropertyList(PropertyList & list)
	{
		EffectLayer::getPropertyList(list);
		list.push_back(token[TOKEN_LY_ES_ROW]);
		list.push_back(token[TOKEN_LY_ES_COLUMN]);
		list.push_back(token[TOKEN_LY_ES_INTERVAL]);
		list.push_back(token[TOKEN_LY_ES_OFFVIEWPORT]);
	}

	bool EffectLayer3DGrid::getPropertyType(const String & name, PropertyType &type)
	{
		if(EffectLayer::getPropertyType(name, type))
			return true;

		if (name == token[TOKEN_LY_ES_ROW])
		{
			type = IElement::PT_INT; return true;
		}
		else if(name == token[TOKEN_LY_ES_COLUMN])
		{
			type = IElement::PT_INT; return true;
		}
		else if(name == token[TOKEN_LY_ES_INTERVAL])
		{
			type = IElement::PT_REAL; return true;
		}
		else if(name == token[TOKEN_LY_ES_OFFVIEWPORT])
		{
			type = IElement::PT_REAL; return true;
		}
		return false;
	}

	bool EffectLayer3DGrid::getPropertyValue(const String & name, String &value)
	{
		if(EffectLayer::getPropertyValue(name, value))
			return true;

		if (name == token[TOKEN_LY_ES_ROW])
		{
			value = StringUtil::ToString(mRows); return true;
		}
		else if (name == token[TOKEN_LY_ES_COLUMN])
		{
			value = StringUtil::ToString(mColumns); return true;
		}
		else if (name == token[TOKEN_LY_ES_INTERVAL])
		{
			value = StringUtil::ToString(mInterval); return true;
		}
		else if (name == token[TOKEN_LY_ES_OFFVIEWPORT])
		{
			value = StringUtil::ToString(mOffViewport); return true;
		}

		return false;
	}

	bool EffectLayer3DGrid::setPropertyValue(const String & name,const String & value)
	{
		if(EffectLayer::setPropertyValue(name, value))
			return true;

		if (name == token[TOKEN_LY_ES_ROW])
		{
			int rows = StringUtil::ParseInt(value);
			_adjustGridRows(rows);
			if(mCallbackFunction)
				mCallbackFunction(this);
			return true;
		}
		else if (name == token[TOKEN_LY_ES_COLUMN])
		{
			int columns = StringUtil::ParseInt(value);
			_adjustGridColumns(columns);
			if(mCallbackFunction)
				mCallbackFunction(this);
			return true;
		}
		else if (name == token[TOKEN_LY_ES_INTERVAL])
		{
			float interval = StringUtil::ParseReal(value);
			_adjustGridInterval(interval);
			if(mCallbackFunction)
				mCallbackFunction(this);
			return true;
		}
		else if (name == token[TOKEN_LY_ES_OFFVIEWPORT])
		{
			mOffViewport = StringUtil::ParseReal(value); return true;
		}

		return false;
	}
}