#include "HeadTextRender.h"

#include "Scene/Camera.h"
#include "UI/GUIFontManager.h"
#include "UI/GUIRenderText.h"

namespace BLOCKMAN
{
	
HeadTextRender::HeadTextRender()
	: m_position(Vector3::ZERO)
{
}

HeadTextRender::~HeadTextRender()
{
}

void HeadTextManualRender::render(bool uiRender)
{
	m_pHeadTextManager->render();
}

HeadTextRenderManager* HeadTextRenderManager::Instance()
{
	return ms_pSingleton;
}

HeadTextRenderManager::HeadTextRenderManager()
{
	m_manualRender.m_pHeadTextManager = this;
	RenderQueue* pRenderQueue = SceneManager::Instance()->getRenderQueue("UI");
	pRenderQueue->setManalRenderBegin(&m_manualRender);
}

HeadTextRenderManager::~HeadTextRenderManager()
{
}

void HeadTextRenderManager::render()
{
	Camera* pCamera = SceneManager::Instance()->getMainCamera();
	Renderer* pRender = Renderer::Instance();
	Matrix4 VPMat = pCamera->getViewProjMatrix();

	std::map<float, std::pair<Vector3, HeadTextRender>> fontSortList;
	
	for (auto it : m_headTexts)
	{
		const HeadTextRender& txt = it;
		//float sqLen = (pCamera->getPosition() - pFont->mParentNode->getWorldPosition()).lenSqr();
		LORD::Vector3 textPos, textPos2;
		pRender->project(textPos, txt.m_position, VPMat);
		Vector3 zRay = pCamera->getDirection();
		Vector3 left = zRay.cross(pCamera->getUp());
		pRender->project(textPos2, txt.m_position + left, VPMat);

		float scale = Math::Abs(textPos2.x - textPos.x) * 0.005f;
		fontSortList.insert(std::make_pair(textPos.z, std::make_pair(Vector3(textPos.x, textPos.y + 4, scale), txt)));
	}

	GUIFont* pFont = GUIFontManager::Instance()->GetFont("HT28");
	vector<FontBuffer>::type buffer;
	HeadTextInfos infos;

	for (auto it = fontSortList.rbegin(); it != fontSortList.rend(); ++it)
	{
		const HeadTextRender& txt = it->second.second;
		Vector2 txtPos(it->second.first.x, it->second.first.y);
		float scale = it->second.first.z;
		vector<FontBuffer>::type buffer2;
		infos.clear();
		parseText(txt.getText(), infos);
		int allLines = infos.size();
		int line = 0;
		for (auto& it : infos)
		{
			buffer2.clear();
			pRender->renderText(it.text, txtPos, pFont, it.color, Vector2(scale, scale), false, &buffer2);

			// reformats the character pos.
			if (buffer2.empty())
				continue;
			float width = buffer2.back().rt.x - buffer2.front().lt.x;
			width /= 2.f;
			float height = buffer2.front().lb.y - buffer2.front().lt.y;
			height *= 1.3f; // 0.3 is line specing.

			for (auto& txt : buffer2)
			{
				txt.rt.x -= width;
				txt.lt.x -= width;
				txt.rb.x -= width;
				txt.lb.x -= width;
				txt.rt.y -= (allLines - line + 1)*height;
				txt.lt.y -= (allLines - line + 1)*height;
				txt.rb.y -= (allLines - line + 1)*height;
				txt.lb.y -= (allLines - line + 1)*height;
			}

			buffer.insert(buffer.end(), buffer2.begin(), buffer2.end());
			line++;
		}
	}

	GUIRenderText::DrawTextBuffer(pFont, buffer);
	m_headTexts.clear();
}

void HeadTextRenderManager::parseText(const GUIString& str, HeadTextInfos& outInfos)
{
	// [#dfdfdf]Superplayer\n LV8 Steve
	outInfos.clear();
	String txt = str.c_str();
	Color defaultColor = Color::WHITE;
	StringArray txts = StringUtil::Split(txt, "\n");
	for (auto& line : txts)
	{
		if (line.find("[#") == 0)
		{
			String colorstr = line.substr(2, 6);
			String txtstr = line.substr(9, line.length() - 9);
			unsigned int nColor = 0;
			sscanf(colorstr.c_str(), "%X", &nColor);
			Color color = nColor;
			color.a = 1.f;
			outInfos.push_back({ GUIString(txtstr.c_str()), color });
		}
		else
		{
			outInfos.push_back({ GUIString(line.c_str()), defaultColor });
		}
	}
}

}