#include "StructureStart.h"
#include "StructureComponent.h"

#include "World/World.h"

namespace BLOCKMAN
{

void StructureStart::generateStructure(World* pWorld, Random& rand, const StructureBB& aabb)
{
	for (ComponentList::iterator it = m_components.begin(); it != m_components.end(); ++it)
	{
		StructureComponent* pComponent = *it;
		if (!pComponent)
			continue;

		if (pComponent->getBoundingBox().intersectsWith(aabb) &&
			pComponent->addComponentParts(pWorld, rand, aabb))
		{ 
			m_components.remove(*it); // real delete ok?
		}
	}
}

void StructureStart::updateBoundingBox()
{
	m_boundingBox.invalid();

	for (ComponentList::iterator it = m_components.begin(); it != m_components.end(); ++it)
	{
		StructureComponent* pComponent = *it;
		if (!pComponent)
			continue;

		m_boundingBox.expandTo(pComponent->getBoundingBox());
	}
}

void StructureStart::markAvailableHeight(World* pWorld, Random& rand, int offsetY)
{
	int var4 = 63 - offsetY;
	int var5 = m_boundingBox.getYSize() + 1;

	if (var5 < var4)
	{
		var5 += rand.nextInt(var4 - var5);
	}

	int var6 = var5 - m_boundingBox.m_maxY;
	m_boundingBox.offset(0, var6, 0);

	for (ComponentList::iterator it = m_components.begin(); it != m_components.end(); ++it)
	{
		StructureComponent* pComponent = *it;
		if (!pComponent)
			continue;

		pComponent->getBoundingBox().offset(0, var6, 0);
	}
}

void StructureStart::setRandomHeight(World* pWorld, Random& rand, int par3, int par4)
{
	int var5 = par4 - par3 + 1 - m_boundingBox.getYSize();
	bool var6 = true;
	int var10;

	if (var5 > 1)
	{
		var10 = par3 + rand.nextInt(var5);
	}
	else
	{
		var10 = par3;
	}

	int var7 = var10 - m_boundingBox.m_minY;
	m_boundingBox.offset(0, var7, 0);

	for (ComponentList::iterator it = m_components.begin(); it != m_components.end(); ++it)
	{
		StructureComponent* pComponent = *it;
		if (!pComponent)
			continue;

		pComponent->getBoundingBox().offset(0, var7, 0);
	}
}

StructureMineshaftStart::StructureMineshaftStart(World* pWorld, Random& rand, int x, int z)
{
	ComponentMineshaftRoom* pRoom = LordNew ComponentMineshaftRoom(0, rand, (x << 4) + 2, (z << 4) + 2);
	m_components.push_back(pRoom);
	pRoom->buildComponent(pRoom, m_components, rand);
	updateBoundingBox();
	markAvailableHeight(pWorld, rand, 10);
}

StructureNetherBridgeStart::StructureNetherBridgeStart(World* pWorld, Random& rand, int x, int z)
{
	ComponentNetherBridgeStartPiece* piece = LordNew ComponentNetherBridgeStartPiece(rand, (x << 4) + 2, (z << 4) + 2);
	m_components.push_back(piece);
	piece->buildComponent(piece, m_components, rand);
	SCList& sclst = piece->contextLst;

	while (!sclst.empty())
	{
		int var7 = rand.nextInt(sclst.size());
		StructureComponent* pComponent = NULL;
		int count = 0;
		for (SCList::iterator it = sclst.begin(); it != sclst.end(); ++it)
		{
			if (count++ == var7)
			{
				pComponent = *it;
				break;
			}
		}
		sclst.remove(pComponent);
		pComponent->buildComponent(piece, m_components, rand);
	}

	updateBoundingBox();
	setRandomHeight(pWorld, rand, 48, 70);
}

}
