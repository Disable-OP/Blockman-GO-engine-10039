#include "MailManager.h"
#include "Common.h"
#include "Util/ClientEvents.h"
#include <sstream>
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"

using namespace rapidjson;

namespace BLOCKMAN
{
	vector<MailInfo>::type MailManager::m_mails = vector<MailInfo>::type();

	void MailManager::addMail(const MailInfo & mail)
	{
		m_mails.push_back(mail);
	}

	vector<MailInfo>::type MailManager::getMails()
	{
		return m_mails;
	}

	void MailManager::parseMailData(const String & data)
	{
		release();
		Document  doc = Document();
		doc.Parse(data.c_str());
		if (!doc.HasParseError())
		{
			const auto& content = doc.GetArray();
			for (const auto& item : content)
			{
				MailInfo mailInfo = MailInfo();
				if (item.HasMember("id") && !item.FindMember("id")->value.IsNull())
				{
					mailInfo.id = item.FindMember("id")->value.GetInt64();
				}

				if (item.HasMember("fromUser") && !item.FindMember("fromUser")->value.IsNull())
				{
					mailInfo.fromUser = item.FindMember("fromUser")->value.GetInt64();
				}

				if (item.HasMember("toUser") && !item.FindMember("toUser")->value.IsNull())
				{
					mailInfo.toUser = item.FindMember("toUser")->value.GetInt64();
				}

				if (item.HasMember("mailType") && !item.FindMember("mailType")->value.IsNull())
				{
					mailInfo.mailType = item.FindMember("mailType")->value.GetInt();
				}

				if (item.HasMember("title") && !item.FindMember("title")->value.IsNull())
				{
					mailInfo.title = item.FindMember("title")->value.GetString();
				}

				if (item.HasMember("content") && !item.FindMember("content")->value.IsNull())
				{
					mailInfo.content = item.FindMember("content")->value.GetString();
				}
				
				if (item.HasMember("status") && !item.FindMember("status")->value.IsNull())
				{
					mailInfo.status = item.FindMember("status")->value.GetString();
				}
				addMail(mailInfo);
			}
		}
		MailUpdateEvent::emit();
	}
}
