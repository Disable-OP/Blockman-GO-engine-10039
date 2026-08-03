#pragma once

namespace LORD
{
	/*
	音效配置管理的定义
	*/
	enum SoundType
	{
		ST_Invalid = -1,

		//音效：UI操作
		ST_ClickSure,						// 0 点击确定按钮
		ST_ClickCancel,						// 1 点击取消按钮
		ST_ErrorOperation,					// 2 错误操作
		ST_InvalidOperation,				// 3 无效操作
		ST_OpenPanel,						// 4 打开一级面板
		ST_ClosePanel,						// 5 关闭一级面板
		ST_OpenSubPanel,					// 6 打开二级或更低级面板
		ST_CloseSubPanel,					// 7 关闭二级或更低级面板
		ST_PopUpTips,						// 8 弹出提示窗
		ST_GetMail,							// 9 邮件
		ST_PrivateChat,						// 10 私聊
		ST_NormalCountDown,					// 11 倒计时，普通
		ST_UrgentCountDown,					// 12 倒计时，急迫
		ST_GetItem,							// 13 获得物品
		ST_GetCoin,							// 14 获得金币
		ST_GetDiamond,						// 15 获得钻石

		//音效：战斗内
		ST_MalePlayerBeHit,					// 16 男玩家受击
		ST_FemalePlayerBeHit,				// 17 女玩家受击
		ST_PlayerBeHit3,					// 18 玩家受击3
		ST_PullTheBow,						// 19 拉弓
		ST_Shoot,							// 20 发射
		ST_LaunchRocket,					// 21 发射火箭
		ST_ShootArrowSp,					// 22 发射特殊箭
		ST_MonsterWound,					// 23 僵尸受伤
		ST_MonsterDead,						// 24 僵尸死亡
		ST_PlaceGrassBlock,					// 25 放置草方块
		ST_BreakGrassBlock,					// 26 破坏草方块
		ST_PlaceDirtBlock,					// 27 放置泥方块
		ST_BreakDirtBlock,					// 28 破坏泥方块
		ST_PlaceWoodBlock,					// 29 放置木方块
		ST_BreakWoodBlock,					// 30 破坏木方块
		ST_PlaceSoftBlock,					// 31 放置软质方块
		ST_BreakSoftBlock,					// 32 破坏软质方块
		ST_PlaceSandBlock,					// 33 放置沙子方块
		ST_BreakSandBlock,					// 34 破坏沙子方块
		ST_PlaceSnowBlock,					// 35 放置雪方块	
		ST_BreakSnowBlock,					// 36 破坏雪方块
		ST_PlaceStoneBlock,					// 37 放置石方块
		ST_BreakStoneBlock,					// 38 破坏石方块
		ST_PlaceIronBlock,					// 39 放置铁方块
		ST_BreakIronBlock,					// 40 破坏铁方块
		ST_PlaceGoldBlock,					// 41 放置金方块
		ST_BreakGoldBlock,					// 42 破坏金方块
		ST_PlaceDiamondBlock,				// 43 放置钻石方块
		ST_BreakDiamondBlock,				// 44 破坏钻石方块
		ST_StepOnCloth,						// 45 行走布质地面
		ST_StepOnGrass,						// 46 行走草地
		ST_StepOnSand,						// 47 行走沙地
		ST_StepOnGravel,					// 48 行走碎石地
		ST_StepOnLadder,					// 49 行走梯子
		ST_StepOnStone,						// 50 行走石地板
		ST_StepOnWood,						// 51 行走木地板
		ST_Fuse,							// 52 引信
		ST_BrokenGlass,						// 53 碎玻璃
		ST_WoodButton,						// 54 木按钮
		ST_OpenWoodDoor,					// 55 开木门
		ST_CloseWoodDoor,					// 56 关木门
		ST_OpenIronDoor,					// 57 开铁门
		ST_CloseIronDoor,					// 58 关铁门
		ST_Portal,							// 59 传送门 portal.portal
		ST_Fizz,							// 60 发出嘶嘶声
		ST_Explosion,						// 61 爆炸 random.explode
		ST_Stealth,							// 62 隐身状态
		ST_SpeedUp,							// 63 加速状态
		ST_Jump,							// 64 跳跃状态
		ST_NightVison,						// 65 夜视状态

		//音效：环境类
		ST_Rain,							// 66 下雨
		ST_Thunder,							// 67 打雷
		ST_Lava,							// 68 熔岩 liquid.lava
		ST_Water,							// 69 流水 liquid.water

		//游戏现有音乐枚举：
		ST_Click,							// 70 点击 random.click
		ST_OpenDoor,						// 71 开门 random.door_open
		ST_CloseDoor,						// 72 关门 random.door_close
		ST_BreakAnvil,						// 73 random.anvil_break
		ST_UseAnvil,						// 74 random.anvil_use
		ST_LandAnvil,						// 75 random.anvil_land
		ST_Glass,							// 76 random.glass
		ST_Swim,							// 77 liquid.swim	
		ST_ThornsHurt,						// 78 damage.thorns
		ST_LiquidSplash,					// 79 liquid.splash
		ST_BowHit,							// 80 random.bowhit
		ST_Pop,								// 81 random.pop
		ST_Break,							// 82 random.break
		ST_FallBig,							// 83 damage.fallbig
		ST_FallSmall,						// 84 damage.fallsmall
		ST_Orb,								// 85 random.orb
		ST_LevelUp,							// 86 random.levelup
		ST_Burp,							// 87 random.burp
		ST_IgniteFire,						// 88 fire.ignite
		ST_OpenChest,						// 89 random.chestopen
		ST_CloseChest,						// 90 random.chestclosed
		ST_PistonOut,						// 91 tile.piston.out
		ST_PistonIn,						// 92 tile.piston.in
		ST_LavaPop,							// 93 liquid.lavapop
		ST_Fire,							// 94 fire.fire
		ST_BreakGravelBlock,				// 95 
		ST_PlaceGravelBlock,				// 96
		ST_PlaceGlassBlock,					// 97
		ST_BreakGlassBlock,					// 98
		ST_StepOnGlass,						// 99
		ST_PlaceClothBlock,					// 100
		ST_BreakClothBlock,					// 101
		ST_StepOnSnow,						// 102
		ST_PlaceLadderBlock,				// 103
		ST_BreakLadderBlock,				// 104
		ST_PlaceAnvilBlock,					// 105
		ST_StepOnAnvil,						// 106
		ST_Drink,							// 107 喝水 random.drink
		ST_Eat,								// 108 吃东西 random.eat
		ST_MobWitherSpawn,					// 109 mob.wither.spawn
		ST_MobEndDragon,					// 110 mob.enddragon.end
		ST_MobZombieUnfect,					// 111 mob.zombie.unfect
		ST_MobZombieInfect,					// 112 mob.zombie.infect
		ST_MobBatTakeoff,					// 113 mob.bat.takeoff
		ST_MobZombieWoodbreak,				// 114 mob.zombie.woodbreak
		ST_MobWitherShoot,					// 115 mob.wither.shoot
		ST_MobZombieMetal,					// 116 mob.zombie.metal
		ST_MobZombieWood,					// 117 mob.zombie.wood
		ST_MobGhastFireball,				// 118 mob.ghast.fireball
		ST_MobGhastCharge,					// 119 mob.ghast.charge

		ST_BuildWarGrade1 =	135,			// 135 
		ST_BuildWarGrade2,					// 136
		ST_BuildWarGrade3,					// 137
		ST_BuildWarGrade4,					// 138
		ST_BuildWarGrade5,					// 139
		ST_BuildWarGrade6,					// 140
		ST_BuildWarTelePos,					// 141
		ST_BuildWarTrans,					// 142
		ST_BuildWarFire,					// 143

		ST_Air_Dig = 311,					// 311 air_dig
		ST_Kill_Person = 312,				// 312 kill person

		//10000以后的ID给音乐BGM用
		ST_BgmStart			= 10000,		// 10000
		ST_BgmThemeSea		= ST_BgmStart,	// 10000 主题音乐1
		ST_BgmThemeHome,					// 10001 主题音乐2
		ST_BgmThemeDead,					// 10002 死亡
		ST_BgmThemeGameOver,				// 10003 游戏结束
		ST_AircraftSound,					//10004 飞机音效
		ST_Loading,							//10005	加载主界面音乐
		ST_ThemeSnowallBattle,				//10006	雪球主题音乐
		ST_ThemeSnowallBattleWaiting,		//10007	雪球等待进入音乐
		ST_ThemeZombie,						//10008	僵尸大战主题音乐
		ST_ThemeZombieWaiting,				//10009	僵尸大战等待进入音乐
		ST_ThemeSkywar,						//10010 空岛主题音乐
		ST_ThemeSkywarWaiting,				//10011	空岛等待进入音乐
		ST_ThemeTNTrun,						//10012	TNT主题音乐
		ST_ThemeSandDigger,				    //10013	挖沙等待进入音乐
		ST_ThemeSandDiggerWaiting,		    //10014	挖沙主题音乐
		ST_ThemeJailBreak,					//10015	警匪等待进入音乐
		ST_ThemeJailBreakWaiting,			//10016	警匪主题音乐
		ST_ThemeHungerGame,					//10017	饥饿游戏等待进入音乐
		ST_ThemeHungerGameWaiting,			//10018	饥饿游戏主题音乐
		ST_ThemeHideAndSeek,				//10019	躲猫猫等待进入音乐
		ST_ThemehideAndSeekWaiting,			//10020	躲猫猫主题音乐
		ST_ThemeClanwar,					//10021	部落战争主题音乐
		ST_ThemeMurdermystery,			    //10022	神秘杀手主题音乐
		ST_GameCompleteWin,					//10023 游戏结束，胜利
		ST_GameCompleteLose,				//10024	游戏结束，失败
		ST_TurnToZombie,                    //10025 变僵尸
		ST_Landing,							//10026 降落
		ST_Hit,								//10027 攻击音效
		ST_Crash,							//10028 塌方音效
		ST_ParkourBGM,						//10029 螺旋跑酷主题音乐
		ST_BuildBattle,						//10030 建筑战争
		ST_ThrowPotWaiting,					//10031 丢锅大战等待进入音乐
		ST_Ranchers,						//10032
		ST_GameCompleteDraw,				//10033 游戏结束，平局
		ST_Chase_Sneer,						//10034 躲猫猫嘲讽音效
		ST_Bird_Simulator,					//10035 Bird_Simulator
		ST_PIXEL_GUN_HALL,					//10036 像素射击大厅BGM
		ST_PIXEL_GUN_GAME,					//10037 像素射击游戏BGM
		ST_Total,
	};
}
