#include "MultiLanTipSetting.h"
#include "Core.h"
#include <sstream>
#include "Object/Root.h"

using namespace LORD;

MultiLanTipSetting::MultiLanTipSettingMap MultiLanTipSetting::ms_multiLanTipSettingMap = {};
std::set<std::string>					  MultiLanTipSetting::ms_languageSet = 
{
	"en_US",			//Ӣ��/����
	"zh_CN",			//���ļ���
	"zh_TW",			//���ķ��� 
	"de_DE",			//����
	"nl_BE",			//������/����ʱ
	"nl_NL",			//������/����
	"en_AU",			//Ӣ��/�Ĵ�����
	"en_GB",			//Ӣ��/Ӣ��
	"en_CA",			//Ӣ��/���ô�
	"en_NZ",			//Ӣ��/������
	"en_SG",			//Ӣ��/�¼���
	"fr_BE",			//����/����ʱ
	"fr_CA",			//����/���ô�
	"fr_FR",			//����/����
	"fr_CH",			//����/��ʿ
	"de_AT",			//����/�µ���
	"de_LI",			//����/��֧��ʿ��
	"de_CH",			//����/��ʿ
	"it_IT",			//�������, �����
	"it_CH",			//�������, ��ʿ
	"ja_JP",			//����
	"ko_KR",			//����
	"pl_PL",			//������
	"ru_RU",			//����
	"es_ES",			//��������
	"ar_EG",			//��������, ����
	"ar_IL",			//��������, ��ɫ��
	"bg_BG",			//����������, ��������
	"ca_ES",			//��̩��������, ������
	"hr_HR",			//���޵�����, ���޵���
	"da_DK",			//������, ����
	"en_IN",			//Ӣ��, ӡ��
	"en_IE",			//Ӣ��, ������
	"en_ZA",			//Ӣ��, ��Ͳ�Τ
	"fi_FI",			//������, ����
	"el_GR",			//ϣ����, ϣ��
	"iw_IL",			//ϣ������, ��ɫ��
	"hi_IN",			//��ӡ����, ӡ��
	"hu_HU",			//��������, ������
	"in_ID",			//ӡ����������, ӡ��������
	"lv_LV",			//����ά����, ����ά��
	"lt_LT",			//��������, ������
	"nb_NO",			//Norwegian-Bokmol, Ų��
	"pt_BR",			//��������, ����
	"pt_PT",			//��������, ������
	"ro_RO",			//����������, ��������
	"sr_RS",			//����ά����
	"sk_SK",			//˹�工����, ˹�工��
	"sl_SI",			//˹��ά������, ˹��������
	"es_US",			//��������, ����
	"sv_SE",			//�����, ���
	"tl_PH",			//����������, ���ɱ�
	"th_TH",			//̩����, ̩��
	"tr_TR",			//��������, ������
	"uk_UA",			//�ڿ�����, �ڿ���
	"vi_VN",			//Խ����, Խ��
};

MultiLanTipSetting::MultiLanTipSetting()
{

}

MultiLanTipSetting::~MultiLanTipSetting()
{
	m_TipMap.clear();
}

bool MultiLanTipSetting::loadSetting(bool isClient)
{
	ms_multiLanTipSettingMap.clear();
	CsvReader* pReader = new CsvReader();
	String EngineBaseDir = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/GameTip");
	LordLogInfo("Load GameTip EngineBaseDir: %s", EngineBaseDir.c_str());
	StringArray gameTipFiles;
	PathUtil::EnumFilesInDir(gameTipFiles, EngineBaseDir, false, false, true);

	for (String path : gameTipFiles)
	{
		LordLogInfo("Load EngineGameTip FilePath: %s", path.c_str());
		std::string filePath = path.c_str();
		loadFileToSetting(filePath, pReader);
	}

	pReader->Close();
	LordSafeDelete(pReader)
	return true;
}

bool MultiLanTipSetting::loadMapSetting(bool isClient)
{
	CsvReader* pReader = new CsvReader();
	String MapBaseDir = "";
	if (isClient) {
		MapBaseDir = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "GameTip");
	} else {
		MapBaseDir = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "GameTip");
	}
	LordLogInfo("Load GameTip MapBaseDir: %s", MapBaseDir.c_str());
	StringArray gameTipFiles;
	PathUtil::EnumFilesInDir(gameTipFiles, MapBaseDir, false, false, true);
	for (String path : gameTipFiles)
	{
		LordLogInfo("Load MapGameTip FilePath: %s", path.c_str());
		std::string filePath = path.c_str();
		loadFileToSetting(filePath, pReader);
	}

	pReader->Close();
	LordSafeDelete(pReader)
	return true;
}

bool MultiLanTipSetting::unloadSetting()
{
	for (auto iter = ms_multiLanTipSettingMap.begin(); iter != ms_multiLanTipSettingMap.end(); iter++)
	{
		MultiLanTipSetting* pSetting = iter->second;
		delete pSetting;
	}

	ms_multiLanTipSettingMap.clear();
	return true;
}

std::string MultiLanTipSetting::getMessage(String& lanType, int nTipType, const char* args)
{
	std::string msg = getMessage(lanType, nTipType);
	return formatMessage(lanType, msg, args);
}

std::string MultiLanTipSetting::formatMessage(String& lanType, std::string& msg, const char* args)
{
	if (!args)
		return msg;

	std::string::size_type		pos;
	unsigned int				index		= 1;
	std::string::size_type		size		= msg.size();
	StringArray					argsList	= StringUtil::Split(args, "\t", 0, true); //the args spilts with "\t"
	std::string					pattern;

	while (true)
	{
		if (index > argsList.size())
			break;

		pattern = StringUtil::concatToString("%", index, "s").c_str();
		pos = msg.find(pattern, 0);
		if (pos >= size)
			break;

		std::string arg = argsList[index - 1].c_str();
		std::string argString = getArgString(lanType, arg);
		msg = msg.replace(pos, pattern.size(), argString);
		size = msg.size();
		index++;
	}
	
	return msg;
}

bool MultiLanTipSetting::loadFileToSetting(std::string& path, CsvReader* pReader)
{
	bool bRet = pReader->LoadFile(path);
	if (!bRet)
	{
		LordLogError("Can not open MultiLanTipSetting file %s", path.c_str());
		return false;
	}

	MultiLanTipSetting* pSetting = NULL;
	pReader->Read(); //discard the second line

	while (pReader->Read())
	{
		pSetting = new MultiLanTipSetting();
		pReader->GetInt("TipType", pSetting->m_nTipId);

		std::string tipStr;
		for (auto iter = ms_languageSet.begin(); iter != ms_languageSet.end(); iter++)
		{
			if (pReader->GetString(*iter, tipStr))
			{
				pSetting->m_TipMap[*iter] = tipStr;
			}
		}

		ms_multiLanTipSettingMap[pSetting->m_nTipId] = pSetting;
	}
	return true;
}

std::string MultiLanTipSetting::getMessage(String& lanType, int nTipType)
{
	MultiLanTipSetting* pSetting = NULL;

	auto iter = ms_multiLanTipSettingMap.find(nTipType);
	if (iter != ms_multiLanTipSettingMap.end())
	{
		pSetting = iter->second;
	}
	else
	{
		LordLogError("Can not find TipType %d", nTipType);
		return "";
	}

	auto it = pSetting->m_TipMap.find(lanType.c_str());
	if (it == pSetting->m_TipMap.end())
	{
		//LordLogError("Can not find TipType %d LanType %s, return default English!!!", nTipType, lanType.c_str());
		it = pSetting->m_TipMap.find("en_US");
	}

	if (it != pSetting->m_TipMap.end())
		return it->second;
	else
		return "";
}

//for example: replace "#66#" with the template which nTipType == 66
std::string MultiLanTipSetting::getArgString(String& lanType, std::string& arg)
{
	if (arg.size() < 3)
		return arg;

	unsigned int pos1;
	unsigned int pos2;
	unsigned int startIndex = 0;
	int nTipType = -1;
	std::string substr;
	std::stringstream ss;
	std::string actualStr;

	while (true)
	{
		pos1 = arg.find("#", 0);
		if (pos1 >= arg.size() - 2)
			return arg;

		pos2 = arg.find("#", pos1 + 2);
		if (pos2 > arg.size())
			return arg;

		substr = arg.substr(pos1 + 1, pos2 - pos1 - 1);
		ss.clear();
		ss << substr;
		ss >> nTipType;
		if (nTipType < 0)
		{
			return arg;
		}

		actualStr = getMessage(lanType, nTipType);
		if (actualStr.size() > 0)
		{
			arg = arg.replace(pos1, pos2 - pos1 + 1, actualStr);
		}
		else
		{
			break;
		}
	}

	return arg;
}

