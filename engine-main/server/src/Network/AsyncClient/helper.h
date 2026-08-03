#ifndef _VCS_HELPER_H
#define _VCS_HELPER_H

#include "CoreDef.h"

#if (LORD_PLATFORM == LORD_PLATFORM_LINUX)
#include "buffer.h"

namespace vcs
{
	class Helper
	{
		public:
			class Socket
			{
				public:
					static int create();
					static int accept(int listenerFd);

					static int listen(const char* addr, unsigned short port);
					static int connect(const char* addr, unsigned short port);

					static bool read(int fd, Buffer* buffer);
					static bool write(int fd, Buffer* buffer);
			};

			class Application
			{
				public:
					static void daemonize();
					static bool setSignalHandler(int signal, void (*action)(int));
			};
	};

	template <typename T>
	class Singleton
	{
	public:
		static T* instance(){
			static T self;
			return &self;
		}

	private:
		Singleton() {}
		Singleton(Singleton const&) {};
		Singleton& operator=(Singleton const&) {};
		virtual ~Singleton() {}
	};
}

#endif
#endif
