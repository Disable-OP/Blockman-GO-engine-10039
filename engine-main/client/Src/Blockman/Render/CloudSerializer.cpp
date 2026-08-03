#include "CloudSerializer.h"

namespace BLOCKMAN
{


CloudSerializer::CloudSerializer(const String& imageName)
	: m_pImage(NULL)
	, m_width(0)
	, m_height(0)	
{
	DataStream* pData = ResourceGroupManager::Instance()->openResource(imageName);
	m_pImage = Image::loadFromDataStream(pData, imageName);
	LordSafeDelete(pData);
}

CloudSerializer::~CloudSerializer()
{
	LordSafeDelete(m_pImage);
}

bool CloudSerializer::generate()
{
	if (!m_pImage)
		return false;
	
	int x = m_loadCoordinate.x;
	int y = m_loadCoordinate.y;
	m_width = m_loadSize.x;
	m_height = m_loadSize.y;

	ui8* maskData = (ui8*)LordMalloc(m_width * m_height * sizeof(ui8));
	for (int h = 0; h < m_height; ++h)
	{
		for (int w = 0; w < m_width; ++w)
		{
			int ix = (w + x) % m_pImage->getWidth();
			int iy = (h + y) % m_pImage->getHeight();
			if (ix < 0) ix += m_pImage->getWidth();
			if (iy < 0) iy += m_pImage->getHeight();
			maskData[h * m_width + w] = int(Math::Floor(m_pImage->getColor(ix, iy, 0).a + 0.1f));
		}
	} 
	//assigned to MXC
	
	m_quads.clear();
	m_edges.clear();

	map<Vector2i, CloudStripe>::type stripeMap;//pos,length
	CloudStripe targetStripe;

	//1.添加矩形
	for (int h = 0; h < m_height; ++h)
	{
		for (int w = 0; w < m_width; ++w)
		{
			int index = h * m_width + w;

			bool shouldGen = maskData[index] == 1;
			/*
			if (maskData[index] == 1)
			{
				m_quads.push_back(Vector2i(w,h));

			}
			*/

			if (shouldGen == false)
			{
				if (targetStripe.length != 0 )
					stripeMap.insert(std::make_pair(targetStripe.bias2D, targetStripe));
				targetStripe.Reset();
			}
			else
			{
				if (targetStripe.length == 0)
					targetStripe.Reset({ w, h });
				
				targetStripe.length++;
				if (x == m_width - 1)
				{
					stripeMap.insert(std::make_pair(targetStripe.bias2D, targetStripe));
					targetStripe.Reset();
				}
			}
		}
	}
	
	auto it = stripeMap.begin();
	for (; it != stripeMap.end(); ++it)
	{
		CloudStripe& curStripe = it->second;
		Vector2i curBias = it->first;//for curbx curby ==> z , y
		int curLen = curStripe.length;
		int curheight = 1;

		if (curLen == 0)
			continue;

		for (int y = curBias.y + 1; y < m_height; ++y)
		{
			Vector2i searchBias(curBias.x, y);
			auto it_search = stripeMap.find(searchBias);
			if (it_search != stripeMap.end())
			{
				if (it_search->second.CanMerge(curStripe))
				{
					stripeMap[searchBias].length = 0;
					curheight++;
				}
				else
					break;
			}
			else
				break;
		}
		m_quads.push_back({ curBias.x, curBias.y, curLen, curheight });
	}

	ui8* greedDown = (ui8*)LordMalloc(m_width * m_height * sizeof(ui8));
	ui8* greedUp = (ui8*)LordMalloc(m_width * m_height * sizeof(ui8));
	ui8* greedRight = (ui8*)LordMalloc(m_width * m_height * sizeof(ui8));
	ui8* greedLeft = (ui8*)LordMalloc(m_width * m_height * sizeof(ui8));
	memset(greedDown, 0, m_width * m_height * sizeof(ui8));  // vert
	memset(greedUp, 0, m_width * m_height * sizeof(ui8));  // horz
	memset(greedRight, 0, m_width * m_height * sizeof(ui8));  // vert
	memset(greedLeft, 0, m_width * m_height * sizeof(ui8));  // horz

	// add the egde from picture.
	for (int h = 0; h < m_height; ++h)
	{
		for (int w = 0; w < m_width; ++w)
		{
			int index = h * m_width + w;
			int indexRight = h * m_width + (w + 1 == m_width ? 0 : w + 1);
			int indexBottom = (h + 1 == m_height ? 0 : h + 1) * m_width + w;
			if (maskData[index] != maskData[indexRight])
			{
				if (maskData[index] && greedRight[index] == 0)
				{
					int height = 0;
					for (int testH = h; testH < m_height; ++testH)
					{
						int testIndex = testH * m_width + w;
						int testIndexRight = testH * m_width + (w + 1 == m_width ? 0 : w + 1);
						if (maskData[testIndex] == 1 && maskData[testIndexRight] == 0 && greedRight[testIndex] == 0)
						{
							height++;
							greedRight[testIndex] = 1;
						}
						else
							break;
					}
					m_edges.push_back(Vector4i(w + 1, h, w + 1, h + height));
				}
				else if (greedLeft[index] == 0)
				{
					int height = 0;
					for (int testH = h; testH < m_height; ++testH)
					{
						int testIndex = testH * m_width + w;
						int testIndexRight = testH * m_width + (w + 1 == m_width ? 0 : w + 1);
						if (maskData[testIndex] == 0 && maskData[testIndexRight] == 1 && greedLeft[testIndex] == 0)
						{
							height++;
							greedLeft[testIndex] = 1;
						}
						else
							break;
					}
					m_edges.push_back(Vector4i(w + 1, h + height, w + 1, h));
				}
			}


			if (maskData[index] != maskData[indexBottom])
			{
				if (maskData[index] && greedDown[index] == 0)
				{
					int width = 0;
					for (int testW = w; testW < m_width; ++testW)
					{
						int testIndex = h * m_width + testW;
						int testIndexBottom = (h + 1 == m_height ? 0 : h + 1) * m_width + testW;;
						if (maskData[testIndex] == 1 && maskData[testIndexBottom] == 0 && greedDown[testIndex] == 0)
						{
							width++;
							greedDown[testIndex] = 1;
						}
						else
							break;
					}
					m_edges.push_back(Vector4i(w, h + 1, w + width, h + 1));
				}
				else if (greedUp[index] == 0)
				{
					int width = 0;
					for (int testW = w; testW < m_width; ++testW)
					{
						int testIndex = h * m_width + testW;
						int testIndexBottom = (h + 1 == m_height ? 0 : h + 1) * m_width + testW;;
						if (maskData[testIndex] == 0 && maskData[testIndexBottom] == 1 && greedUp[testIndex] == 0)
						{
							width++;
							greedUp[testIndex] = 1;
						}
						else
							break;
					}
					m_edges.push_back(Vector4i(w + width, h + 1, w, h + 1));
				}
			}
		}
	}

	// parse cloud data to create edges and quads
	//generateManually();
	LordSafeFree(maskData);
	LordSafeFree(greedDown);
	LordSafeFree(greedUp);
	LordSafeFree(greedRight);
	LordSafeFree(greedLeft);
	return true;
}

/*
	获得position位置的起始方向
*/
CloudSerializer::Node CloudSerializer::getStartDirectioin(ui8* maskData, const Vector2i& position, int alpha)
{
	//当起始点被三个透明像素包围时，只能朝开口方向移动
	if (anotherPixelCount(maskData, position, alpha) == 3)
	{
		Node node;

		Direction direction;//开口朝向
		if(isThePixel(maskData, nextPosition(position, CloudSerializer::UP), alpha)) direction = CloudSerializer::UP;
		else if (isThePixel(maskData, nextPosition(position, CloudSerializer::DOWN), alpha)) direction = CloudSerializer::DOWN;
		else if (isThePixel(maskData, nextPosition(position, CloudSerializer::LEFT), alpha)) direction = CloudSerializer::LEFT;
		else direction = CloudSerializer::RIGHT;

		switch (direction)
		{
		case CloudSerializer::UP:
			node.movingDirection = CloudSerializer::UP;
			node.alongDirection = CloudSerializer::LEFT;
			break;
		case CloudSerializer::DOWN:
			node.movingDirection = CloudSerializer::DOWN;
			node.alongDirection = CloudSerializer::LEFT;
			break;
		case CloudSerializer::LEFT:
			node.movingDirection = CloudSerializer::LEFT;
			node.alongDirection = CloudSerializer::DOWN;
			break;
		case CloudSerializer::RIGHT:
			node.movingDirection = CloudSerializer::RIGHT;
			node.alongDirection = CloudSerializer::DOWN;
			break;
		default:
			break;
		}
		node.position = position;
		return node;
	}
	else
	{
		Node node;
		node.position = position;
		node.movingDirection = CloudSerializer::DOWN;
		node.alongDirection = CloudSerializer::LEFT;
		return node;
	}
}

/*
	判断像素是否在范围内
*/
bool CloudSerializer::isValidPixel(const Vector2i& position)
{
	return position.x >= 0 && position.x < m_width
		&& position.y >= 0 && position.y < m_height;
}

/*
	判断position位置的像素的值是否等于alpha
*/
bool CloudSerializer::isThePixel(ui8* maskData, const Vector2i& position, int alpha)
{
	int index = position.y * m_width + position.x;
	return isValidPixel(position) && maskData[index] == alpha;
}

/*
	判断position位置的像素的值是否不等于alpha
*/
bool CloudSerializer::isAnotherPixel(ui8* maskData, const Vector2i& position, int alpha)
{
	int index = position.y * m_width + position.x;
	return !isValidPixel(position) || maskData[index] != alpha;
}

/*
	反转方向
*/
CloudSerializer::Direction CloudSerializer::reverseDirection(Direction direction)
{
	switch (direction)
	{
	case CloudSerializer::UP:
		return CloudSerializer::DOWN;
		break;
	case CloudSerializer::DOWN:
		return CloudSerializer::UP;
		break;
	case CloudSerializer::LEFT:
		return CloudSerializer::RIGHT;
		break;
	case CloudSerializer::RIGHT:
		return CloudSerializer::LEFT;
		break;
	default:
		return CloudSerializer::LEFT;
		break;
	}
}

/*
	能否朝指定方向移动
*/
bool CloudSerializer::canMove(ui8* maskData, const Vector2i& position, Direction direction, int alpha)
{
	Vector2i nextPosition = this->nextPosition(position, direction);	
	return isThePixel(maskData, nextPosition, alpha);

}

/*
	在position朝direction移动的下一个位置
*/
Vector2i CloudSerializer::nextPosition(Vector2i positon, Direction direction)
{
	switch (direction)
	{
	case BLOCKMAN::CloudSerializer::UP:
		--positon.y;
		break;
	case BLOCKMAN::CloudSerializer::DOWN:
		++positon.y;
		break;
	case BLOCKMAN::CloudSerializer::LEFT:
		--positon.x;
		break;
	case BLOCKMAN::CloudSerializer::RIGHT:
		++positon.x;
		break;
	default:
		return positon;
		break;
	}
	return positon;
}


/*
	点position在上下左右四个方向有多少个值不等于alpha的像素
*/
int CloudSerializer::anotherPixelCount(ui8* maskData, const Vector2i& position, int alpha)
{
	int count = 0;

	if (isAnotherPixel(maskData, nextPosition(position, CloudSerializer::UP), alpha)) ++count;
	if (isAnotherPixel(maskData, nextPosition(position, CloudSerializer::DOWN), alpha)) ++count;
	if (isAnotherPixel(maskData, nextPosition(position, CloudSerializer::LEFT), alpha)) ++count;
	if (isAnotherPixel(maskData, nextPosition(position, CloudSerializer::RIGHT), alpha)) ++count;

	return count;
}

void CloudSerializer::setLoadCoordinate(const Vector2i& coordinate)
{
	m_loadCoordinate = coordinate;
}

void CloudSerializer::setLoadSize(const Vector2i& size)
{
	m_loadSize = size;
}

void CloudSerializer::setCloudShift(const Vector2& shift)
{
	m_shift = shift;
}

void CloudSerializer::setCameraPrevPosition(const Vector3& prevPosition)
{
	m_prevPosition = prevPosition;
}

void CloudSerializer::setCloudSpeed(float speed)
{
	m_speed = speed;
}

}