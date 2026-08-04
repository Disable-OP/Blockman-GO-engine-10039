#pragma once
struct FloatProxy
{
	float value;
};

struct BoolProxy
{
	bool value;
};

struct IntProxy
{
	int value;
};

struct LongProxy
{
	long long value;
};

struct ShoppingAttr
{
	bool isConsumeCoin;
	bool isAddGoods;
};

struct CommodityInfo
{
	int goodsId;
	int goodsNum;
	int coinId;
	int coinNum;
};