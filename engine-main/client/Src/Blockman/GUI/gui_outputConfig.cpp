#include "gui_outputConfig.h"
#include "Object/Root.h"
#include "Util/PathUtil.h"
#include "cWorld/Blockman.h"
#include "Math/LordMath.h"
#include "game.h"

namespace BLOCKMAN
{

	PositionOutput::PositionOutput()
	{	
		String fileName = "Config/Position/Position_" + PathUtil::GetTime() + ".csv";
		std::string path = PathUtil::ConcatPath(GameClient::CGame::Instance()->getConfigPath(), fileName).c_str();
		std::vector<std::string> header =
		{
			"PlayerX", "PlayerY", "PlayerZ", "PlayerYaw", "BlockX", "BlockY", "BlockZ", "BlockId", "Remarks"
		};
		writer = new CsvWriter(path, header);
		writer->Create();
	}

	PositionOutput::~PositionOutput()
	{
		writer->Close();
		writer = nullptr;
	}

	void PositionOutput::output(String remarks)
	{
		Vector3 playerPos = Blockman::Instance()->getPlayerPosition();
		BlockPos blockPos = Blockman::Instance()->getBlockPosition();
		int yaw = StringUtil::ParseInt(StringUtil::ToString(Math::Round(Blockman::Instance()->getPlayerYaw())));
		yaw = (yaw + 360) % 360;
		int blockId = Blockman::Instance()->getBlockId();
		std::map<String, String> result = 
		{
			{ "PlayerX", StringUtil::ToString(Math::Round(playerPos.x)) },
			{ "PlayerY", StringUtil::ToString(Math::Round(playerPos.y - 2.0f)) },
			{ "PlayerZ", StringUtil::ToString(Math::Round(playerPos.z)) },
			{ "PlayerYaw", StringUtil::ToString(yaw) },
			{ "BlockX", StringUtil::ToString(blockPos.x + 0.5f) },
			{ "BlockY", StringUtil::ToString(blockPos.y) },
			{ "BlockZ", StringUtil::ToString(blockPos.z + 0.5f) },
			{ "BlockId", StringUtil::ToString(blockId) },
			{ "Remarks", remarks },
		};
		writer->Write(result);
	}

	gui_outputConfig::gui_outputConfig()
		: gui_layout("OutputConfig.json")
	{
		output_pos = new PositionOutput();
		outputFuncs = 
		{
			{ OUTPUT_TYPE::POSITION, std::bind(&gui_outputConfig::outputPosition, this) }
		};
	}

	gui_outputConfig::~gui_outputConfig()
	{
		delete output_pos;
	}

	void gui_outputConfig::onLoad()
	{
		ltRemarks = getWindow<GUILayout>("OutputConfig-Input-Remarks");
		ebRemarksInput = getWindow<GUIEditBox>("OutputConfig-Remarks-Input");
		btnRemarksConfirm = getWindow<GUIButton>("OutputConfig-Remarks-Confirm");
		ltRemarks->SetVisible(false);

		btnOutputPosition = getWindow<GUIButton>("OutputConfig-Output-Position");

		ebRemarksInput->subscribeEvent(EventWindowTextChanged, std::bind(&gui_outputConfig::onRemarksChanged, this, std::placeholders::_1));
		btnRemarksConfirm->subscribeEvent(EventButtonClick, std::bind(&gui_outputConfig::onRemarksConfirm, this, std::placeholders::_1));
		btnOutputPosition->subscribeEvent(EventButtonClick, std::bind(&gui_outputConfig::onOutputConfig, this, std::placeholders::_1));
	}

	void gui_outputConfig::onUpdate(ui32 nTimeElapse)
	{
	}

	bool gui_outputConfig::onRemarksChanged(const EventArgs & events)
	{
		std::string text = ebRemarksInput->GetText().c_str();
		ebRemarksInput->SetText(text.c_str());
		return true;
	}

	bool gui_outputConfig::onRemarksConfirm(const EventArgs & events)
	{
		auto iter = outputFuncs.find(outputType);
		if (iter != outputFuncs.end())
		{
			auto func = iter->second;
			func();
		}
		ebRemarksInput->SetText("");
		ltRemarks->SetVisible(false);
		return true;
	}

	bool gui_outputConfig::onOutputConfig(const EventArgs & events)
	{
		outputType = OUTPUT_TYPE::POSITION;
		ltRemarks->SetVisible(true);
		return true;
	}

	void gui_outputConfig::outputPosition()
	{
		if (output_pos != nullptr)
		{
			output_pos->output(ebRemarksInput->GetText().c_str());
		}
	}

}