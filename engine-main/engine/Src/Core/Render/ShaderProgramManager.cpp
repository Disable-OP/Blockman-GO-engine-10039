#include "Core.h"
#include "ShaderProgramManager.h"
#include "Util/StringUtil.h"
#include "Util/PathUtil.h"
#include "Object/Exception.h"
#include "Object/Root.h"

#include "Renderer.h"
#include "Image/PixelFormat.h"
#include "Resource/ResourceGroupManager.h"
#include "MaterialDesc.h"

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"

#include "rapidjson/document.h"

namespace LORD
{
	ShaderProgramManager::ShaderProgramManager()
	{
		Renderer* pRenderer = Renderer::Instance();
		Shader::ShaderDesc vsDesc;
		String vsFileName = "tesselator_skin.vs";
		String errMsg;
		Shader *pVertexShader = pRenderer->createShader(Shader::ST_VERTEXSHADER, vsDesc, vsFileName, errMsg);
		if (!pVertexShader)
		{
			Root::Instance()->setSupportGPUSkin(false);
			LordLogDebug("This rendor can't support hardawre skinned mesh. [%s]", vsFileName.c_str());
		}
		else
		{
			m_Shaders.insert(ShaderHandleMap::value_type(vsFileName, pVertexShader));
		}

		initialize();
	}

	ShaderProgramManager::~ShaderProgramManager()
	{
		free();
	}

	void ShaderProgramManager::initialize()
	{
		loadFromFile("ShaderDefine.json");
		//loadFromFile("ShaderDefine.xml");
	}

	void ShaderProgramManager::free()
	{
		ShaderProgramMap::iterator itprogram = m_ShaderProgramMap.begin();
		for (; itprogram != m_ShaderProgramMap.end(); ++itprogram)
		{
			if (itprogram->second)
			{
				LordSafeDelete(itprogram->second);
			}
		}

		m_ShaderProgramMap.clear();

		ShaderHandleMap::iterator ithandle = m_Shaders.begin();
		for (; ithandle != m_Shaders.end(); ++ithandle)
		{
			if (ithandle->second)
			{
				LordSafeDelete(ithandle->second);
			}
		}

		m_Shaders.clear();
	} 

	void ShaderProgramManager::loadFromFile(const String& filename)
	{
		//add by maxicheng
		try
		{
			free();
			DataStream* pJSONStream = ResourceGroupManager::Instance()->openResource(filename);
			if (pJSONStream == NULL)
			{	
				LordException("ShaderDefine file not found [%s].", filename.c_str());								
			}

			size_t fileSize = pJSONStream->size();
			char* jsonData = (char*)LordMalloc(fileSize + 1);
			pJSONStream->read(jsonData, fileSize);
			jsonData[fileSize] = 0;

			rapidjson::Document* doc = new rapidjson::Document;
			doc->Parse(jsonData);

			if (doc->HasParseError())
			{
				LordException("The ShaderDefine file content is invalid.");
			}
			else
			{			
				LordAssert(doc->IsObject());
				//LordLogInfo("open ShderDefine.json successful!");
				loadShaderFrom(doc);
			}
		
			LordDelete(doc);
			LordFree(jsonData);
			LordDelete(pJSONStream);
		}
		catch (bool)
		{
			free();
			LordException("ShaderProgramManager::loadFromFile: Fail to Parse ShaderDefine file [%s].", filename.c_str());
		}
		
		

		/*
		try 
		{
			free();
			// assigned to some body.
			DataStream* pXMLStream = ResourceGroupManager::Instance()->openResource(filename);
			if(pXMLStream == NULL)
			{
				LordException("ShaderDefine file not found [%s].", filename.c_str());
			}
			size_t fileSize = pXMLStream->size();
			char* xmlData = (char*)LordMalloc(fileSize + 1);
			pXMLStream->read(xmlData, fileSize);
			xmlData[fileSize] = 0;
			rapidxml::xml_document<>* doc = LordNewT(rapidxml::xml_document<>);
			doc->parse<0>(xmlData);

			rapidxml::xml_node<> *pRootNode = doc->first_node();
			if(!pRootNode)
			{
				LordException("The ShaderDefine file content is valid.");
			}

			loadShaderFrom(pRootNode->first_node());
			LordDeleteT(doc, xml_document<>);
			LordFree(xmlData);
			LordDelete pXMLStream;
		}
		catch(bool)
		{
			free();
			LordException("ShaderProgramManager::loadFromFile: Fail to Parse ShaderDefine file [%s].", filename.c_str());
		}
		*/
	}

	bool ShaderProgramManager::loadShaderFrom( void* pNode )
	{
		//add by maxicheng
		rapidjson::Document *doc = static_cast<rapidjson::Document*>(pNode);
		const rapidjson::Value& shaderGroup = doc->FindMember("ShderDefineGroup")->value;

		ui32 uuid = 0;
		for (rapidjson::Value::ConstMemberIterator it = shaderGroup.MemberBegin(); it != shaderGroup.MemberEnd(); ++it)
		{
			LordAssert(it->name.IsString());
			LordAssert(it->value["shader"]["vs"].IsString());
			LordAssert(it->value["shader"]["vs"].IsString());

			String vsPrograme, psPrograme, struuid;
			vsPrograme = it->value["shader"]["vs"].GetString();
			psPrograme = it->value["shader"]["ps"].GetString();
			
			
			ShaderProgram* pPrograme = createShaderProgram(vsPrograme, psPrograme, uuid++);
			m_ShaderProgramMap.insert(std::make_pair(it->name.GetString(), pPrograme));
			//LordLogInfo("insert shader programe %s  successful!", it->name.GetString());
		}

		return true;

		/*
		rapidxml::xml_node<>* pSubNode = static_cast<rapidxml::xml_node<>*>(pNode);
		try
		{
			ui32 uuid = 0;
			do 
			{
				rapidxml::xml_node<>* pShaderNode = pSubNode->first_node();	LordAssert(pShaderNode);
				rapidxml::xml_attribute<>* pVarNode = pShaderNode->first_attribute();
				String vsPrograme, psPrograme, struuid;
				while (pVarNode)
				{
					const char *str = pVarNode->value();
					String strName = pVarNode->name();
					if (strName == "vs")
					{
						vsPrograme = str;
					}
					else if (strName == "ps")
					{
						psPrograme = str;
					}
					
					pVarNode = pVarNode->next_attribute();
				}
				try
				{
					ShaderProgram* pPrograme = createShaderProgram(vsPrograme, psPrograme, uuid++);
					m_ShaderProgramMap.insert(std::make_pair(pSubNode->name(), pPrograme));
				}
				catch (Exception& e)
				{
					LordLogError(e.getMessage().c_str());
				}

				pSubNode = pSubNode->next_sibling();
			} while (pSubNode);
			
			return true;
		}
		catch (bool)
		{
			free();
			return false;
		}
		*/
	}
	
	ShaderProgram* ShaderProgramManager::createShaderProgram( const String& vsFileName, const String& psFileName, ui32 uuid )
	{
		String errMsg;
		Renderer* pRenderer = Renderer::Instance();
		Shader *pVertexShader = NULL;
		Shader *pPixelShader = NULL;
		ShaderHandleMap::iterator it = m_Shaders.find(vsFileName);
		if (it == m_Shaders.end())
		{
			Shader::ShaderDesc vsDesc;
			pVertexShader = pRenderer->createShader(Shader::ST_VERTEXSHADER, vsDesc, vsFileName, errMsg);
			if (!pVertexShader)
			{
				String output = "Error in create vs file: ";
				output += vsFileName;
				LordException(output.c_str());
			}
			m_Shaders.insert(std::make_pair(vsFileName, pVertexShader));
		}
		else
		{
			pVertexShader = it->second;
		}
		it = m_Shaders.find(psFileName);
		if (it == m_Shaders.end())
		{
			Shader::ShaderDesc psDesc;
			pPixelShader = pRenderer->createShader(Shader::ST_PIXELSHADER, psDesc, psFileName, errMsg);

			if (!pPixelShader)
			{
				String output = "Error in create ps file: ";
				output += psFileName;
				LordException(output.c_str());
			}
			m_Shaders.insert(std::make_pair(psFileName, pPixelShader));
		}
		else
		{
			pPixelShader = it->second;
		}

		// create shader program
		ShaderProgram* pShaderProgram = pRenderer->createShaderProgram(uuid);
		pShaderProgram->attachShader(pVertexShader);
		pShaderProgram->attachShader(pPixelShader);
		pShaderProgram->linkShaders();

		return pShaderProgram;
	}

	ShaderProgram* ShaderProgramManager::getShaderProgram(const String& name)
	{
		ShaderProgramMap::iterator it = m_ShaderProgramMap.find(name);
		if (it != m_ShaderProgramMap.end())
		{
			return it->second;
		}
		else
		{
			LordException("ShaderProgramManager::getShaderPrograme: no found shaderprograme:[%s]", name.c_str());
		}
		return NULL;
	}
}
