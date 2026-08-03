/********************************************************************
filename: 	EffectLayer3DGrid.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_EFFECT_LAYER_3DGRID_H__
#define __LORD_EFFECT_LAYER_3DGRID_H__

#include "EffectLayer.h"

namespace LORD
{
	class EffectLayer3DGrid;
	typedef void (*ES_FUN)(EffectLayer3DGrid* ptr);

	class LORD_CORE_API EffectLayer3DGrid : public EffectLayer
	{
	public:
		virtual void getPropertyList(PropertyList & list);
		virtual bool getPropertyType(const String & name, PropertyType &type);
		virtual bool getPropertyValue(const String & name, String &value);
		virtual bool setPropertyValue(const String & name,const String & value);

	public:
		EffectLayer3DGrid();
		virtual ~EffectLayer3DGrid();

		//
		void setCallBackFunction(ES_FUN fun);

		virtual void getMatrixWorld(Matrix4& mtxWorld);
		virtual void _updateRenderQueue();

		virtual void prepare();
		virtual void unprepare();

		virtual void importData(DataStream* pStream, int version);
		virtual void exportData(DataStream* pStream, int version);
		virtual void mergeAligenBox(Box& aabb);

		virtual void copyAttributesTo(EffectParticle* particle);

		// for editor.
		size_t getRows() const { return mRows; }
		size_t getColumns() const { return mColumns; }
		Vector3 getSurfaceVector(int row, int column)const;
		void setSurfaceVector(int row, int column, const Vector3& newPos);
		Vector3 getSurfaceVector(int index)const;
		void setSurfaceVector(int index, const Vector3& newPos);
		Color getSurfaceColor(int row, int column) const;
		void setSurfaceColor(int row, int column, const Color& newColor);
		Color getSurfaceColor(int index)const;
		void setSurfaceColor(int index, const Color& newColor);
		virtual const Quaternion& getSurfaceOritation() const;

	protected:
		void _adjustGridRows(ui32 rows);
		void _adjustGridColumns(ui32 columns);
		void _adjustGridInterval(float interval);

		ui32		mRows;					
		ui32		mColumns;
		float		mInterval;				
		Vector3*		mpVectorArray;			
		Color*		mpColorArray;			
		float		mOffViewport;
		Box			mGridBox;
		ES_FUN		mCallbackFunction; 
	};
}


#endif