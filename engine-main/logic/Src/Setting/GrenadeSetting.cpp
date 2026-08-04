#include "GrenadeSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{

	std::map<int, SGrenade*>  GrenadeSetting::m_GrenadeMap = {};

	bool GrenadeSetting::loadSetting(bool isClient)
	{
		m_GrenadeMap.clear();
		m_GrenadeMap.insert({ 425, new SGrenade() });

		SGrenade* pGrenade = NULL;
		CsvReader* pReader = new CsvReader();

		std::string path = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/GrenadeSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogError("Can not open GrenadeSetting setting file %s", path.c_str());
			return false;
		}
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			pGrenade = new SGrenade();
			pReader->GetInt("ItemId", pGrenade->itemId);
			pReader->GetString("ItemIcon", pGrenade->itemIcon);
			pReader->GetBool("IsExplosion", pGrenade->isExplosion);
			pReader->GetFloat("ExplosionPower", pGrenade->explosionPower);
			pReader->GetBool("IsExplosionEffect", pGrenade->isExplosionEffect);
			pReader->GetString("EffectName", pGrenade->effectName);
			pReader->GetInt("EffectTime", pGrenade->effectTime);
			m_GrenadeMap[pGrenade->itemId] = pGrenade;
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	void GrenadeSetting::unloadSetting()
	{
		for (auto iter = m_GrenadeMap.begin(); iter != m_GrenadeMap.end(); iter++)
		{
			delete iter->second;
		}
		m_GrenadeMap.clear();
	}

	SGrenade * GrenadeSetting::getGrenadeSetting(int itemId)
	{
		auto iter = m_GrenadeMap.find(itemId);
		if (iter != m_GrenadeMap.end())
		{
			return iter->second;
		}
		else
		{
			if (itemId == 425)
			{
				SGrenade* pGrenade = new SGrenade();
				m_GrenadeMap.insert({ itemId, pGrenade });
				return pGrenade;
			}
			else
			{
				return getGrenadeSetting(425);
			}
		}
	}

}