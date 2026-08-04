/********************************************************************
filename: 	CloudSerializer.h
file path:	dev\client\Src\Blockman

version:	1
creator:	ajohn
company:	supernano
date:		2016-12-07
*********************************************************************/
#ifndef __CLOUD_SERIALIZER_HEADER__
#define __CLOUD_SERIALIZER_HEADER__

#include "Core.h"
using namespace LORD;

class TessObject;

namespace BLOCKMAN
{


struct CloudStripe
{
	Vector2i bias2D;
	ui8 length;

	CloudStripe()
	{
		bias2D.x = 999;
		bias2D.y = 999;
		length = 0;
	}

	void Reset(const Vector2i& i_bias2D)
	{
		bias2D = i_bias2D;
		length = 0;
	}

	void Reset()
	{
		bias2D = Vector2i(999, 999);
		length = 0;
	}

	bool CanMerge(const CloudStripe& other)
	{
		return length == other.length;
	}
};


class CloudSerializer : public ObjectAlloc
{
public:
	typedef vector<Vector4i>::type ClosureEdges;
	typedef vector<Vector4i>::type CloudQuadArr;

	//for edge--add by maxicheng
	enum Direction { UP, DOWN, LEFT, RIGHT};

	struct Node
	{
		Vector2i position;
		Direction movingDirection;
		Direction alongDirection;

		bool operator !=(const Node& another)
		{
			return position != another.position 
				|| movingDirection != another.movingDirection
				|| alongDirection != another.alongDirection;					
		}
	};

public:
	CloudSerializer(const String& imageName);
	~CloudSerializer();

	bool generate();

	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }
	const ClosureEdges& getEdges() const { return m_edges; }
	const CloudQuadArr& getQuads() const { return m_quads; }

protected:
	Node getStartDirectioin(ui8* maskData, const Vector2i& position, int alpha);
	bool isValidPixel(const Vector2i& position);	
	bool isThePixel(ui8* maskData, const Vector2i& position, int alpha);
	bool isAnotherPixel(ui8* maskData, const Vector2i& position, int alpha);
	Direction reverseDirection(Direction direction);	
	Vector2i nextPosition(Vector2i positon, Direction direction);		
	bool canMove(ui8* maskData, const Vector2i& position, Direction direction, int alpha);
	int anotherPixelCount(ui8* maskData, const Vector2i& position, int alpha);	

protected:
	Image*			m_pImage = nullptr;
	ClosureEdges	m_edges;
	CloudQuadArr	m_quads;
	int				m_width = 0;
	int				m_height = 0;

	//for render--add by maxicheng
	Vector2i m_loadCoordinate;		//云的坐标
	Vector2i m_loadSize;			//云的大小
	Vector2 m_shift;				//云当前的偏移量
	Vector3 m_prevPosition;			//相机前一刻的位置
	float m_speed = 0.f;			//云的漂浮速度

public:
	Vector2i getLoadCoordinate() { return m_loadCoordinate; }
	Vector2i getLoadSize() { return m_loadSize; }
	Vector2 getCloudShift() { return m_shift; }
	Vector3 getCameraPrevPosition() { return m_prevPosition; }
	float getCloudSpeed() { return m_speed; }

	void setLoadCoordinate(const Vector2i& coordinate);
	void setLoadSize(const Vector2i& size);
	void setCloudShift(const Vector2& shift);
	void setCameraPrevPosition(const Vector3& prevPosition);
	void setCloudSpeed(float speed);
	
};

}

#endif