#ifndef __SKILL_SETTING_HEADER__
#define __SKILL_SETTING_HEADER__

#include <map>
#include "Core.h"
#include "Network/protocol/PacketDefine.h"

using namespace LORD;

namespace BLOCKMAN
{
	class SkillEffect
	{
	public:
		int id = 0;
		String name = "";
		float duration = 0.0f;
		int width = 0;
		int height = 0;
		int density = 0;
		Vector3 color;
	};

	class SkillSetting
	{
	public:
		static void unloadSetting();
		static SkillEffect* getSkillEffect(int skillId);
		static void addSkillEffect(SkillEffect setting);
	private:
		static std::map<int, SkillEffect*> m_skillEffectMap;
	};

	class SkillItem
	{
		public:
			int Occupation = 0;
			int ItemId = 0;
			int SkillId = 0;
			float ItemSkillCd = 0;
			float ItemSkillDistace = 0;
			float MoveSpeed = 0;
			float DropSpeed = 0;
			bool Parabola = false;
			String AtlasName = "";
			String ItemIconName = "";
			bool AutoCast = false;
			bool IsCrossHair = true;
			String EffectName = "";
			DEFINE_NETWORK_SERIALIZER(Occupation, ItemId, SkillId, ItemSkillCd, ItemSkillDistace, MoveSpeed, DropSpeed, Parabola, AtlasName, ItemIconName, AutoCast, IsCrossHair, EffectName);
	};

	class SkillItemSetting
	{
	public:
		static void unloadSetting();
		static bool isSkillItem(int itemId);
		static bool isAutoCastSkill(int itemId);
		static bool isShowCrossHair(int ItemId);
		static bool isThrowSkillItem(int itemId);
		static SkillItem* getSkillItem(int itemId,int Occupation);
		static SkillItem* getSkillItem(int skillId);
		static void addSkillItem(SkillItem setting);
	private:
		static std::map<int, SkillItem*> m_skillItemMap;
	};
}

#endif