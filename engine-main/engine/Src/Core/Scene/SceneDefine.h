/********************************************************************
filename: 	SceneDefine.h
file path:	dev\engine\Src\Core\Scene\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/


#ifndef __LORD_SCENE_DEF_H__
#define __LORD_SCENE_DEF_H__

namespace LORD
{

	enum PropertyType
	{
		// base
		PT_BOOL,
		PT_CHAR,
		PT_UCHAR,
		PT_SHORT,
		PT_USHORT,
		PT_INT,
		PT_UINT,
		PT_LONG,
		PT_ULONG,
		PT_REAL,
		PT_FLOAT,
		PT_DOUBLE,
		PT_VECTOR2,
		PT_VECTOR3,
		PT_DIRECTION,
		PT_VECTOR4,
		PT_MATRIX3,
		PT_MATRIX4,
		PT_QUATERNION,
		PT_COLOR,
		PT_STRING,
		// object
		PT_MIRROR,
		PT_PIROR,
		// smObj
		PT_HLMAP,
		// dmObj
		PT_ANIMATION,
		// environArea
		PT_EATYPE,
		PT_EASUN,
		PT_EAFOG,
		PT_EAFOGLOW,
		// area
		// autonomous
		PT_AUTONOMOUSTYPE,
		PT_AUTONOMOUSBEHAVIOUR,

		// another.
		PT_TEXTURE,

		// water
		PT_GRIDSIZE,

		//策划的巡逻路经类型
		PT_LINETYPE,

		PT_UNKNOWN,
	};

	enum TerrainMaskType
	{
		TERRAIN_WATER_MASK = 0x00000001,
		TERRAIN_EARTH_MASK = 0x00000002,
		TERRAIN_SNOW_MASK  = 0x00000004,
		TERRAIN_STONE_MASK = 0x00000008,
		TERRIAN_MASK_MAX,			
	};

	//for editor
	enum EditorMode
	{
		EDITOR_NONE = 0,
		EDITOR_TERRAIN,
		EDITOR_TERRAINMESH,
		EDITOR_GAMEOBJ,
		EDITOR_LOGICOBJ,
		EDITOR_NAVIGATION,
		EDITOR_LIGHTMAP,
		EDITOR_CAMERAANIM,
		EDITOR_OCEAN,
		EDITOR_MAX,
	};

	// for editor
	enum ObjectSubTookit
	{
		OST_SMESH = 0,
		OST_DMESH,
		OST_EFFECT,
		OST_LIGHT,
		OST_SMESH_L,
		OST_DIRLIGHT,
		OST_SPOTLIGHT,
		OST_TERRAIN,
		OST_GROUP,
		OST_SMESH_L_NOFOG,
		OST_HAZE,
		OST_MAX,
	};

	enum GameObjectType
	{
		GOT_SMESH =  0x00000001,
		GOT_DMESH =  0x00000002,
		GOT_EFFECT = 0x00000004,
		GOT_LOGIC =  0x00000008,
		GOT_POINTLIGHT =  0X00000010,
		GOT_SMESH_L =  0X00000020,
		GOT_DIRLIGHT =  0X00000040,
		GOT_SPOTLIGHT =  0X00000080,
		GOT_TERRAIN =  0X00000100,
		GOT_SIMDPOINTLIGHT =0X00000200,
		GOT_GROUP =  0x0000000400,
		GOT_SMESH_L_NOFOG =  0X00000800,
		GOT_HAZE = 0X00001000,
		GOT_MAX,
	};

	enum LogicObjectType
	{
		LOT_MONSTER = 0x00000001,
		LOT_GROWPOINTGROUP = 0x00000002,
		LOT_GROWPOINT = 0x00000004,
		LOT_PATHING = 0x00000008,
		LOT_REGION_PK = 0x00000010,
		LOT_REGION_SCENE = 0x00000020,
		LOT_REGION_SHOP = 0x00000040,
		LOT_REGION_EXC = 0x00000080,
		LOT_MAX,
	};

	// for editor
	enum ObjectOperationSpace
	{
		OOS_WORLD = 0,
		OOS_LOCAL = 1,
	};

	// for editor
	enum ObjectOperationType
	{
		OOT_MOVE = 0,
		OOT_ROTATE,
		OOT_SCALE,
	};
	
#define CAMERA_POINT_QUERY_FLAG 0x00010000
#define NAVIGATE_POINT_QUERY_FLAG 0x0002000
}

#endif