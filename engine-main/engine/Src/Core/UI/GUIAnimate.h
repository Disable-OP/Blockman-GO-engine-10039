/********************************************************************
filename: 	GUIAnimate.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_ANIMATE__H__
#define __GUI_ANIMATE__H__

#include "Core.h"
#include "GUIString.h"
#include "GUIRenderImage.h"

namespace LORD
{	
	class GUIImage;

	class LORD_CORE_API GUIAnimate : public ObjectAlloc
	{
	public:	
		friend class GUIAnimatMgr;

		GUIAnimate();
		virtual ~GUIAnimate();			

		void doPlay(const ERect &rt, const GUIWindow* window = NULL);

		void doPlayEx(const ERect &rt, const GUIWindow* window, GUIRenderImage& render, ui32& timeStamp, int& index, bool& playEnd);

		void clearImage();

		Vector2 getCurrentImageVect2();

		struct GUIImageInfo
		{
			GUIString strWholeImageName;
			ui32 color;
		};
	protected:
		typedef vector<GUIImageInfo>::type GUIImageVect;
		
		GUIImageVect m_vecImages;
		GUIRenderImage m_renderImage;

		i32 m_id;
		GUIString m_name;
		ui32 m_playTime;
		bool m_loop;		
		//²¥·Å¸¨Öú²ÎÊý
		ui32 m_hasRenderTime;
		int  m_index;
		bool m_playContinue;
	};
}


#endif
