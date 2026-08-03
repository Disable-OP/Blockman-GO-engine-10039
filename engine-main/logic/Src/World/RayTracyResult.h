/********************************************************************
filename: 	RayTracyResult.h
file path:	dev\client\Src\Blockman\World

version:	1
author:		ajohn
company:	supernano
date:		2017-02-09
*********************************************************************/
#ifndef __RAYTRACY_RESULT_HEADER__
#define __RAYTRACY_RESULT_HEADER__

#include "BM_TypeDef.h"
#include "Util/Facing.h"
#include "Entity/Entity.h"

namespace BLOCKMAN
{

class RayTraceResult : public ObjectAlloc
{
public:
	bool result = false;
	/** The type of hit that occured, see {@link RayTraceResult#Type} for possibilities. */
	RAYTRACE_TYPE typeOfHit = RAYTRACE_TYPE_INVALID;
	Facing* sideHit = nullptr;
	Vector3i blockPos;
	/** The vector position of the hit */
	Vector3 hitVec;
	/** The hit entity */
	Entity* entityHit = nullptr;

public:
	RayTraceResult()
		: result(false)
        , typeOfHit(RAYTRACE_TYPE_INVALID)
		, sideHit(NULL)
		, entityHit(NULL)
	{}

	void reset()
	{
		result = false;
		sideHit = NULL;
		entityHit = NULL;
		blockPos = Vector3i::ZERO;
		hitVec = Vector3::ZERO;
	}

	void set(const Vector3& hitVecIn, Facing* sideHitIn, const Vector3i& blockPosIn)
	{
		result = true;
		typeOfHit = RAYTRACE_TYPE_BLOCK;
		blockPos = blockPosIn;
		sideHit = sideHitIn;
		hitVec = hitVecIn;
		entityHit = NULL;
	}

	void set(const Vector3& hitVecIn, Facing* sideHitIn)
	{
		result = true;
		typeOfHit = RAYTRACE_TYPE_BLOCK;
		blockPos = Vector3i::ZERO;
		sideHit = sideHitIn;
		hitVec = hitVecIn;
		entityHit = NULL;
	}

	void set(Entity* entityIn)
	{
		result = true;
		typeOfHit = RAYTRACE_TYPE_ENTITY;
		blockPos = Vector3i::ZERO;
		sideHit = NULL;
		hitVec = entityIn->position;
		entityHit = entityIn;
	}

	void set(RAYTRACE_TYPE typeIn, const Vector3& hitVecIn, Facing* sideHitIn, const Vector3i& blockPosIn)
	{
		result = true;
		typeOfHit = typeIn;
		blockPos = blockPosIn;
		sideHit = sideHitIn;
		hitVec = hitVecIn;
		entityHit = NULL;
	}

	void set(Entity* entityHitIn, const Vector3& hitVecIn)
	{
		result = true;
		typeOfHit = RAYTRACE_TYPE_ENTITY;
		blockPos = Vector3i::ZERO;
		sideHit = NULL;
		hitVec = hitVecIn;
		entityHit = entityHitIn;
	}

	const Vector3i& getBlockPos()
	{
		return blockPos;
	}

	static RayTraceResult calculateIntercept(const Box& aabb, const Vector3& begin, const Vector3& end)
	{
		RayTraceResult result;
		Vector3 var3 = begin.getIntermediateWithXValue(end, aabb.vMin.x);
		Vector3 var4 = begin.getIntermediateWithXValue(end, aabb.vMax.x);
		Vector3 var5 = begin.getIntermediateWithYValue(end, aabb.vMin.y);
		Vector3 var6 = begin.getIntermediateWithYValue(end, aabb.vMax.y);
		Vector3 var7 = begin.getIntermediateWithZValue(end, aabb.vMin.z);
		Vector3 var8 = begin.getIntermediateWithZValue(end, aabb.vMax.z);

		if (!aabb.isVecInYZ(var3))
			var3 = Vector3::INVALID;

		if (!aabb.isVecInYZ(var4))
			var4 = Vector3::INVALID;

		if (!aabb.isVecInXZ(var5))
			var5 = Vector3::INVALID;

		if (!aabb.isVecInXZ(var6))
			var6 = Vector3::INVALID;

		if (!aabb.isVecInXY(var7))
			var7 = Vector3::INVALID;

		if (!aabb.isVecInXY(var8))
			var8 = Vector3::INVALID;

		Vector3 var9 = Vector3::INVALID;

		if (var3 != Vector3::INVALID && (var9 == Vector3::INVALID || begin.squareDistanceTo(var3) < begin.squareDistanceTo(var9)))
		{
			var9 = var3;
		}

		if (var4 != Vector3::INVALID && (var9 == Vector3::INVALID || begin.squareDistanceTo(var4) < begin.squareDistanceTo(var9)))
		{
			var9 = var4;
		}

		if (var5 != Vector3::INVALID && (var9 == Vector3::INVALID || begin.squareDistanceTo(var5) < begin.squareDistanceTo(var9)))
		{
			var9 = var5;
		}

		if (var6 != Vector3::INVALID && (var9 == Vector3::INVALID || begin.squareDistanceTo(var6) < begin.squareDistanceTo(var9)))
		{
			var9 = var6;
		}

		if (var7 != Vector3::INVALID && (var9 == Vector3::INVALID || begin.squareDistanceTo(var7) < begin.squareDistanceTo(var9)))
		{
			var9 = var7;
		}

		if (var8 != Vector3::INVALID && (var9 == Vector3::INVALID || begin.squareDistanceTo(var8) < begin.squareDistanceTo(var9)))
		{
			var9 = var8;
		}

		if (var9 == Vector3::INVALID)
		{
			return result;
		}
		else
		{
			Facing* pFacing = Facing::DOWN;
			
			if (var9 == var3)
			{
				pFacing = Facing::WEST;
			}

			if (var9 == var4)
			{
				pFacing = Facing::EAST;
			}

			if (var9 == var5)
			{
				pFacing = Facing::DOWN;
			}

			if (var9 == var6)
			{
				pFacing = Facing::UP;
			}

			if (var9 == var7)
			{
				pFacing = Facing::NORTH;
			}

			if (var9 == var8)
			{
				pFacing = Facing::SOUTH;
			}

			result.set(var9, pFacing, Vector3i(0, 0, 0));
			return result;
		}
	}

	String toString()
	{
		return "HitResult";
	}
};

}
#endif
