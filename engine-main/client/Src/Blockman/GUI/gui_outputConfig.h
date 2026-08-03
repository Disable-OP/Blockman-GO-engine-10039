#pragma once
#ifndef __GUI_OUTPUT_CONFIG_HEADER__
#define __GUI_OUTPUT_CONFIG_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "UI/GUIButton.h"
#include "UI/GUILayout.h"
#include "UI/GUIEditBox.h"
#include "Util/CSVWriter.h"

using namespace LORD;

namespace BLOCKMAN
{
	enum OUTPUT_TYPE
	{
		POSITION = 1
	}; 

	class PositionOutput
	{
	private:
		CsvWriter* writer;
	public:
		PositionOutput();
		~PositionOutput();
		void output(String remarks);
	};

	class gui_outputConfig : public gui_layout
	{
	private:
		PositionOutput* output_pos = nullptr;
	private:
		std::map<OUTPUT_TYPE, std::function<void()>> outputFuncs;

		OUTPUT_TYPE outputType = POSITION;

		GUILayout* ltRemarks = nullptr;
		GUIEditBox* ebRemarksInput = nullptr;
		GUIButton* btnRemarksConfirm = nullptr;

		GUIButton* btnOutputPosition = nullptr;
	public:
		gui_outputConfig();
		~gui_outputConfig();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

		bool onRemarksChanged(const EventArgs & events);
		bool onRemarksConfirm(const EventArgs & events);
		bool onOutputConfig(const EventArgs & events);

		void outputPosition();
	};

}
#endif