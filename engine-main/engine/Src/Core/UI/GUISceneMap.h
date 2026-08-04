/********************************************************************
filename: 	GUISceneMap.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_SCENE_MAP_H__
#define __GUI_SCENE_MAP_H__

#include "Core.h"
#include "GUIWindow.h"

namespace LORD
{
	class GUIStaticImage;
	class LORD_CORE_API GUISceneMap: public GUIWindow
	{
	public:
		struct IconInfo 
		{
			Vector2		position;
			GUIString	strIconImageName;
			Vector2		iconSize;
		};

		typedef vector<IconInfo>::type IconList;

		GUISceneMap(GUIWindowType nType, const GUIString& strName);
		virtual ~GUISceneMap();

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);
		
		void			InitSceneMap(const GUIString& strImageName, float fSceneWidth, float fSceneHeight, const Vector2& scenePosition);

		void			SetScale(float fScaleX, float fScaleY);

		void			SetCenterPostion(const Vector2& scenePosition);

		virtual bool	TouchDown(const Vector2& position);
		virtual bool	TouchUp(const Vector2& position);
		virtual bool	TouchMove(const Vector2& position);
		virtual void	UpdateSelf(ui32 nTimeElapse);

		void			ClearAllIcon();

		void			SetIcon(const Vector2& scenePosition, const GUIString& strIconName, const Vector2& iconSize);

		const Vector2&	GetTargetPosition() { return m_targetPosition; }

		void			ResetAllWindow();

	protected:

		float			m_fSceneWidth;
		float			m_fSceneHeight;
		// UI显示的大小和场景的大小不一定一致
		float			m_fSceneScaleX;
		float			m_fSceneScaleY;
		float			m_fUIWidth;
		float			m_fUIHeight;

		// 当前地图控件中心的位置在父窗口中的百分比
		float			m_fNowCenterX;
		float			m_fNowCenterY;

		GUIStaticImage*	m_pSceneMap;

		bool			m_bDragging;
		Vector2			m_dragPosition;

		IconList		m_iconList;
		ChildList		m_IconWindowList;

		// 用来保存目标点, 场景坐标
		Vector2			m_targetPosition;
	};
}


#endif