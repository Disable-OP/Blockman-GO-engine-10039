#ifndef __GUI_WINDOW_FACTORY_HEADER__
#define __GUI_WINDOW_FACTORY_HEADER__

#include <functional>
#include <stdexcept>
#include <utility>
#include "UI/GUIWindow.h"
#include "Memory/MemAllocDef.h"

namespace LORD
{

	class duplicate_gui_window_creator : std::runtime_error
	{
		using std::runtime_error::runtime_error;
	public:
		duplicate_gui_window_creator(const String& str) : std::runtime_error(str.c_str()) {}
	};

	class LORD_CORE_API GUIWindowFactory : public ObjectAlloc
	{
	public:
		using Creator = std::function<GUIWindow*(const GUIWindowType&, const GUIString&)> ;

	private:
		GUIWindowFactory() = default;
		struct GUIWindowTypeComparator
		{
			bool operator()(const GUIWindowType& type1, const GUIWindowType& type2) const
			{
				return strcmp(type1.value, type2.value) < 0;
			}
		};

		map<const GUIWindowType, Creator, GUIWindowTypeComparator>::type creators;

	public:
		~GUIWindowFactory() = delete;
		static GUIWindowFactory* Instance();

		void registerWindowType(const GUIWindowType& type, Creator creator)
		{
			if (creators.find(type) != creators.end())
			{
				throw duplicate_gui_window_creator("duplicate creators are registered for " + type.str());
			}
			creators[type] = creator;
		}

		GUIWindow* create(const GUIWindowType& type, const GUIString& strName)
		{
			return creators.at(type)(type, strName);
		}

		const decltype(creators) registry() const
		{
			return creators;
		}
	};
}

#endif // !__GUI_WINDOW_FACTORY_HEADER__

