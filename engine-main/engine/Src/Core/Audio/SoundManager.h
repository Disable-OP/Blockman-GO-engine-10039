#ifndef __SOUND_MANAGER_HEADER__
#define __SOUND_MANAGER_HEADER__

#include <random>
#include <cctype>
#include "Core.h"

namespace LORD
{
	struct SoundSet
	{
		String name;
		vector<String>::type sounds;
	};

	class SoundManager
	{
	private:
		static const char SOUND_ROOT_PATH[];
		static const char SOUND_FILE_EXT[];
		map<String, SoundSet>::type m_nameToSoundSetMap;

		std::mt19937 m_rand{ std::random_device()() };

	public:
		void scanSounds();
		String getRandomSoundFile(const String& soundName);
	};

	class SoundSetting
	{
	public:
		typedef LORD::map<int, SoundSetting*>::type SoundSettingMap;

		SoundSetting();
		~SoundSetting();

		LORD::String			GetRandomPath();
		static bool				loadSetting();
		static SoundSetting*	getSoundSetting(int soundId);
		static bool				unloadSetting();
		static void				testSetting();
			
		int				nSoundId;
		bool			bIsRepeat;
		float			fMinInterval;
		float			fMaxInterval;
		LORD::String	sSoundPath;

	private:
		static SoundSettingMap ms_soundSettingMap;
		void	 printSetting();
		bool	 InitSuffix();

		vector<String>::type m_suffixes;
		const String		 m_SoundSuffix = ".mp3";
		std::mt19937		 m_rand{ std::random_device()() };
	};
}

#endif // !__SOUND_MANAGER_HEADER__
