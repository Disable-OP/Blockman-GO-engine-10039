#ifndef _SHELL_INTERFACE_H_
#define _SHELL_INTERFACE_H_

#include "Core.h"
#include "TypeDef.h"
#include "UI/GUIString.h"

namespace ClothesPlug
{	
	class ShellInterface : public LORD::ObjectAlloc
	{
	public:  
		ShellInterface();
		virtual ~ShellInterface();
		
		virtual void    gameExit();

		virtual void	onError(int errorCode);
	}; 
}

#endif
