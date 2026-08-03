#pragma once
#include <map>
#include<iostream>  
#include<fstream>  
#include<sstream>  
#include<list> 
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{
	struct SensitiveNode
	{
		char data;
		bool last;
		SensitiveNode* next;
		SensitiveNode* sibling;
	};

	class FilterWord
	{	
	public:
		static bool loadSetting();
		static bool replace(std::string& content);
		static void unloadSetting();

	private:
		static bool loadSensitiveText(std::list<std::string>& wordList);
		static bool buildTree(std::list<std::string>& wordList);
		static SensitiveNode* nodeHasExist(SensitiveNode* cur, char data);
		static void releaseTree(SensitiveNode* root);

		static SensitiveNode * m_rootNode;
	};
}