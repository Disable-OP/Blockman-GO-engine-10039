#include "FilterWord.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	SensitiveNode* FilterWord::m_rootNode = NULL;

	bool FilterWord::loadSetting()
	{
		std::list<std::string> wordList;
		if (!loadSensitiveText(wordList))
			return false;

		buildTree(wordList);
		return true;
	}

	bool FilterWord::loadSensitiveText(std::list<std::string>& wordList)
	{
		std::ifstream fi;
		std::string path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/words.txt")).c_str();
		fi.open(path, std::ios::binary);
		if (!fi.is_open())
		{
			LordLogError("Can not open file %s", path.c_str());
			return false;
		}

		std::string line;
		while (!fi.eof())
		{
			std::getline(fi, line);
			if (line.empty()) continue;
			unsigned int pos = line.find_last_of("\r"); // replace the \r in line
			if (pos <= line.size())
			{
				line.replace(pos, 1, "");
			}
			if (line.empty()) continue;

			wordList.push_back(line);
		}

		fi.close();
		return true;
	}

	bool FilterWord::buildTree(std::list<std::string>& wordList)
	{
		m_rootNode = new SensitiveNode();
		m_rootNode->next = NULL;
		m_rootNode->sibling = NULL;
		m_rootNode->data = 0;

		SensitiveNode* cur = m_rootNode;
		for (auto it = wordList.begin(); it != wordList.end(); it++)
		{
			unsigned int len = it->length();
			for (size_t i = 0; i < len; i++)
			{
				SensitiveNode* node = nodeHasExist(cur->next, (*it)[i]);//check char node does exist in cur->next link list  

				if (node == NULL) 
				{
					node = new SensitiveNode();
					node->data = (*it)[i];
					node->last = false;
					node->next = NULL;
					node->sibling = NULL;
					if (cur->next == NULL)
					{
						cur->next = node;//make node become cur->next's first  
					}
					else 
					{
						SensitiveNode* temp = cur->next;
						while (temp->sibling != NULL) //make point move to cur->next's last
						{  
							temp = temp->sibling;
						}
						temp->sibling = node;//add node at cur-next's last  
					}
				}

				cur = node;
			}

			cur->last = true;
			cur = m_rootNode;
		}

		return true;
	}

	SensitiveNode * FilterWord::nodeHasExist(SensitiveNode * cur, char data)
	{
		while (cur != NULL)
		{
			if (cur->data == data)
				return cur;

			cur = cur->sibling;
		}

		return NULL;
	}

	void FilterWord::releaseTree(SensitiveNode * root)
	{
		if (root != NULL)
		{
			releaseTree(root->next);
			releaseTree(root->sibling);
			delete root;
		}
	}

	bool FilterWord::replace(std::string& content)
	{
		SensitiveNode* cur = m_rootNode->next;
		int start = -1;
		std::stringstream ss;
		bool shouldReview = false;
		for (size_t i = 0; i < content.length(); i++) //traversal content  
		{

			bool hasfind = false;//if hasfind,find next char  

			char ch = content[i];

			while (cur != NULL) 
			{
				if (cur->data == ch) 
				{
					hasfind = true;
					if (start == -1) //first time find char ,record first time index  
					{
						start = i;
					}
					if (cur->last && cur->next == NULL) //it has arrived at last branch   
					{
						start = -1;
						ss << "*";
						cur = m_rootNode->next;
						shouldReview = false;
					}
					else if (cur->last) //it may not arrived at last branch
					{  
						start = i + 1;
						ss << "*";
						cur = cur->next;
						shouldReview = true;
					}
					else 
					{
						cur = cur->next;
					}
					break;
				}
				cur = cur->sibling;
			}

			if (shouldReview && !hasfind)
			{
				i = start - 1;//this time is checking cur->next,it should also check root->next,start-1 because of i++  
				shouldReview = false;
				start = -1;
				cur = m_rootNode->next;
			}
			else if (!hasfind) 
			{
				if (start > -1)
				{
					i = start;//start must have checked with root->next,so it was not used to be -1  
				}
				ss << content[i];
				start = -1;
				cur = m_rootNode->next;
			}
		}

		if (start > -1 && !shouldReview) 
		{
			for (size_t j = start; j < content.length(); j++)
			{
				ss << content[j];
			}
		}

		content = ss.str();
		//std::cout << ss.str() << std::endl;
		return true;
	}

	void FilterWord::unloadSetting()
	{
		releaseTree(m_rootNode);
	}
}