#include "BulletClipSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "Item/Items.h"

namespace BLOCKMAN
{
	std::map<int, BulletClipSetting*> BulletClipSetting::m_bulletClipSettingMap = {};

	bool BulletClipSetting::loadSetting(bool bIsClient)
	{
		BulletClipSetting* pSetting = NULL;
		m_bulletClipSettingMap.clear();
		CsvReader* pReader = new CsvReader();

		std::string path = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/bulletclip.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogError("Can not open bullet clip setting file %s", path.c_str());
			return false;
		}

		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			pSetting = new BulletClipSetting();
			pReader->GetInt("BulletClipId", pSetting->bulletId);
			pReader->GetInt("MaxStack", pSetting->maxStack);

			pReader->GetString("Name", pSetting->bulletName);
			pReader->GetString("ClipModel", pSetting->bulletClipModel);
			pReader->GetString("BulletIcon", pSetting->bulletIcon);
			pReader->GetString("Icon", pSetting->icon);
			m_bulletClipSettingMap[pSetting->bulletId] = pSetting;
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	void BulletClipSetting::unloadSetting()
	{
		for (auto iter = m_bulletClipSettingMap.begin(); iter != m_bulletClipSettingMap.end(); iter++)
		{
			BulletClipSetting* pSetting = iter->second;
			delete pSetting;
		}

		m_bulletClipSettingMap.clear();
	}

	BulletClipSetting * BulletClipSetting::getBulletClipSetting(int bulletId)
	{
		auto iter = m_bulletClipSettingMap.find(bulletId);
		if (iter != m_bulletClipSettingMap.end())
		{
			return iter->second;
		}

		return nullptr;
	}

	void BulletClipSetting::updateBulletClipSetting(BulletClipPluginSetting setting)
	{
		BulletClipSetting * bulletClipSetting = getBulletClipSetting(setting.bulletId);
		if (bulletClipSetting)
		{
			bulletClipSetting->maxStack = setting.maxStack;
			ItemBulletClip* pBulletClip = dynamic_cast<ItemBulletClip*>(Item::itemsList[setting.bulletId]);
			if (pBulletClip) {
				pBulletClip->setMaxStackSize(setting.maxStack);
			}
		}
	}
}


