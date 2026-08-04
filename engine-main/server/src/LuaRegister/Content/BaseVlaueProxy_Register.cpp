#include "LuaRegister/Template/LuaRegister.h"
#include "Script/VarProxy/BaseValueProxy.h"
#include "Script/VarProxy/LogicVarProxy.h"

_BEGIN_REGISTER_CLASS(FloatProxy)
_CLASSREGISTER_AddVar(value, FloatProxy::value)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(ShoppingAttr)
_CLASSREGISTER_AddVar(isConsumeCoin, ShoppingAttr::isConsumeCoin)
_CLASSREGISTER_AddVar(isAddGoods, ShoppingAttr::isAddGoods)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(BoolProxy)
_CLASSREGISTER_AddVar(value, BoolProxy::value)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(IntProxy)
_CLASSREGISTER_AddVar(value, IntProxy::value)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(LongProxy)
_CLASSREGISTER_AddVar(value, LongProxy::value)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(ExplosionAttr)
_CLASSREGISTER_AddVar(isBreakBlock, ExplosionAttr::isBreakBlock)
_CLASSREGISTER_AddVar(isCanHurt, ExplosionAttr::isCanHurt)
_CLASSREGISTER_AddVar(isHurtEntityItem, ExplosionAttr::isHurtEntityItem)
_CLASSREGISTER_AddVar(custom_explosionSize, ExplosionAttr::custom_explosionSize)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(CommodityInfo)
_CLASSREGISTER_AddVar(goodsId, CommodityInfo::goodsId)
_CLASSREGISTER_AddVar(goodsNum, CommodityInfo::goodsNum)
_CLASSREGISTER_AddVar(coinId, CommodityInfo::coinId)
_CLASSREGISTER_AddVar(coinNum, CommodityInfo::coinNum)
_END_REGISTER_CLASS()
