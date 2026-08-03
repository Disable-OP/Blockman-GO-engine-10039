#ifndef __BLOCK_ARCHIVE_HEADER__
#define __BLOCK_ARCHIVE_HEADER__

#include "Core.h"

namespace BLOCKMAN
{
	class BlockArchive : public LORD::FileSystemArchive
	{
	public:
		BlockArchive(const LORD::String& strName, const LORD::String& strArchType)
			: LORD::FileSystemArchive(strName, strArchType)
		{
			const auto& components = LORD::StringUtil::Split(LORD::PathUtil::GetAbsolutePath(strName), "/\\");
			m_pluginName = components[components.size() - 3];
			m_blockName = components[components.size() - 1];
		}

		bool isCaseSensitive(void) const;

		void load();

		void unload();

		LORD::DataStream* open(const LORD::String& strFilename) override;

		LORD::StringArray list(bool bRecursive = true, bool bDirs = false) override;

		LORD::String location(const LORD::String& filename) override;

		bool exists(const LORD::String& strFilename);

		LORD::String getFileName(const LORD::String& innerName) const override 
		{ 
			return addPrefix(innerName); 
		}

	private:
		LORD::String m_pluginName;
		LORD::String m_blockName;

		LORD::String getPrefix() const
		{
			return "plugin/" + m_pluginName + "/block/" + m_blockName + "/";
		}

		LORD::String addPrefix(const LORD::String& fileName) const
		{
			return getPrefix() + fileName;
		}

		LORD::String removePrefix(const LORD::String& fileName) const
		{
			return fileName.substr(getPrefix().size());
		}
	};

	class BlockArchiveFactory : public LORD::ArchiveFactory
	{
	public:
		virtual ~BlockArchiveFactory() {}

		const LORD::String& getType(void) const
		{
			static LORD::String strName = "Block";
			return strName;
		}

		LORD::Archive *createInstance(const LORD::String& strName)
		{
			return LordNew BlockArchive(strName, "Block");
		}

		void destroyInstance(LORD::Archive* arch)
		{
			LordDelete arch;
		}
	};
}

#endif // !__BLOCK_ARCHIVE_HEADER__
