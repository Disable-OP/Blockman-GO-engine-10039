#pragma once
#ifndef __MAIL_MANAGER_HEADER__
#define __MAIL_MANAGER_HEADER__

#include "BM_TypeDef.h"
#include "Core.h"

namespace BLOCKMAN
{
	class EntityPlayer;

	class MailInfo
	{
	public:
		i64 id = 0;
		i64 toUser = 0;
		i64 fromUser = 0;
		i32 mailType = 0;
		String title = "";
		String status = "";
		String content = "";
		MailInfo() {}
	};

	class MailManager : public ObjectAlloc
	{

	private:
		static vector<MailInfo>::type m_mails;

	private:

	public:
		static void addMail(const MailInfo& mail);
		static vector<MailInfo>::type  getMails();

	public:
		static void parseMailData(const String & data);

	public:
		static void release() { m_mails.clear(); }
		
	};

	
};

#endif