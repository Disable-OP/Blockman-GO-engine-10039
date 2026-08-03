#pragma once
#include "LuaRegister/Template/ClassRegister.h"
#include "LuaRegister/Template/GlobleFuncRegister.h"
#include "LuaRegister/Template/LuaEngine.h"
#include "LuaRegister/Template/Util/AutoRunner.h"

namespace LUA_REGISTER
{
#define  _BEGIN_REGISTER_CLASS(b)                                                                    \
         AutoRunner   b##1([](){                                                              \
	     lua_State *L = LuaEngine::getInstance()->getLuaState();                                     \
         ClassRegister<b> reg = ClassRegister<b>(#b, L);                                             

#define _CLASSREGISTER_AddBaseClass(a)                                                               \
              reg.addBaseClass<a>(#a);

#define _CLASSREGISTER_AddCtor(...)                                                                  \
              reg.addCtor<__VA_ARGS__>();        

#define _CLASSREGISTER_AddMember(name, a)                                                            \
              reg.addMember(#name, &a);

#define _CLASSREGISTER_AddVar(name, a)                                                               \
              reg.addVarMember(#name, &a);

#define _CLASSREGISTER_AddMember_Override(name, a, ...)                                              \
              reg.addOverrideMember<__VA_ARGS__>(#name, &a);

#define _CLASSREGISTER_AddStaticMember(name, a)                                                      \
              reg.addStaticMember(#name, &a);

#define _CLASSREGISTER_AddStaticMember_Override(name, a, ...)                                   \
              reg.addOverrideStaticMember<__VA_ARGS__>(#name, &a);                    

#define _END_REGISTER_CLASS()                                                                        \
         });

#define  _REGISTER_FUNC(b)                                                                           \
         AutoRunner   b##1([](){                                                                     \
          lua_State *L = LuaEngine::getInstance()->getLuaState();                                    \
         GlobleFunctionRegister::registerFunc(L, #b, &b);});   
}