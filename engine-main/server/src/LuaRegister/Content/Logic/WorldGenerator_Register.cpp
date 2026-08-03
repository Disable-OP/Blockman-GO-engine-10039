#include "WorldGenerator/Anvil.h"
#include "WorldGenerator/AnvilManager.h"
#include "WorldGenerator/BiomeCache.h"
#include "WorldGenerator/BiomeDecorator.h"
#include "WorldGenerator/BiomeGen.h"
#include "WorldGenerator/FlatGeneratorInfo.h"
#include "WorldGenerator/FlatLayerInfo.h"
#include "WorldGenerator/GenLayer.h"
#include "WorldGenerator/MapGenerate.h"
#include "WorldGenerator/NoiseGenerator.h"
#include "WorldGenerator/NoiseGeneratorOctaves.h"
#include "WorldGenerator/NoiseGeneratorPerlin.h"
#include "WorldGenerator/StructureComponent.h"
#include "WorldGenerator/StructureNetherBridgePieceWeight.h"
#include "WorldGenerator/StructurePieceBlockSelector.h"
#include "WorldGenerator/StructurePieces.h"
#include "WorldGenerator/StructureStart.h"
#include "WorldGenerator/WorldGenerator.h"
#include "LuaRegister/Template/LuaRegister.h"
using namespace BLOCKMAN;

using namespace LUA_REGISTER;


/*
// Register Class ChunkWithMeta
_BEGIN_REGISTER_CLASS(ChunkWithMeta)
_END_REGISTER_CLASS()


// Register Class AnvilManager
_BEGIN_REGISTER_CLASS(AnvilManager)
_CLASSREGISTER_AddMember(anvilFileExists, AnvilManager::anvilFileExists)
_CLASSREGISTER_AddMember(createNewAnvilFile, AnvilManager::createNewAnvilFile)
_CLASSREGISTER_AddMember(getAllAnvilsInCache, AnvilManager::getAllAnvilsInCache)
_CLASSREGISTER_AddMember(getAnvil, AnvilManager::getAnvil)
_CLASSREGISTER_AddMember(getAnvilFilePath, AnvilManager::getAnvilFilePath)
_CLASSREGISTER_AddMember(getRegionDir, AnvilManager::getRegionDir)
_CLASSREGISTER_AddMember(loadAllAnvilsInDirectory, AnvilManager::loadAllAnvilsInDirectory)
_CLASSREGISTER_AddMember_Override(loadAnvil1, AnvilManager::loadAnvil,void ,int ,int )
_CLASSREGISTER_AddMember(saveAllAnvilsInCache, AnvilManager::saveAllAnvilsInCache)
_CLASSREGISTER_AddMember(saveAnvil, AnvilManager::saveAnvil)
_END_REGISTER_CLASS()


// Register Class BiomeCache
_BEGIN_REGISTER_CLASS(BiomeCache)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(WorldChunkManager * )
_CLASSREGISTER_AddMember(cleanupCache, BiomeCache::cleanupCache)
_CLASSREGISTER_AddMember(getBiomeCacheBlock, BiomeCache::getBiomeCacheBlock)
_CLASSREGISTER_AddMember(getBiomeGenAt, BiomeCache::getBiomeGenAt)
_CLASSREGISTER_AddMember(getCachedBiomes, BiomeCache::getCachedBiomes)
_CLASSREGISTER_AddStaticMember(getChunkManager, BiomeCache::getChunkManager)
_END_REGISTER_CLASS()




// Register Class BiomeCacheBlock
_BEGIN_REGISTER_CLASS(BiomeCacheBlock)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(BiomeCache * , int , int )
_CLASSREGISTER_AddMember(getBiomeGenAt, BiomeCacheBlock::getBiomeGenAt)
_END_REGISTER_CLASS()




// Register Class BiomeDecorator
_BEGIN_REGISTER_CLASS(BiomeDecorator)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(BiomeGenBase * )
_CLASSREGISTER_AddMember_Override(decorate, BiomeDecorator::decorate,void ,World * ,Random & ,int ,int )
_END_REGISTER_CLASS()




// Register Class BiomeEndDecorator
_BEGIN_REGISTER_CLASS(BiomeEndDecorator)
_CLASSREGISTER_AddBaseClass(BiomeDecorator)
_CLASSREGISTER_AddCtor(BiomeGenBase * )
_CLASSREGISTER_AddMember(decorate, BiomeEndDecorator::decorate)
_END_REGISTER_CLASS()




// Register Class BiomeGenBase
_BEGIN_REGISTER_CLASS(BiomeGenBase)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(canSpawnLightningBolt, BiomeGenBase::canSpawnLightningBolt)
_CLASSREGISTER_AddMember(decorate, BiomeGenBase::decorate)
_CLASSREGISTER_AddMember(getColorMultiplierType, BiomeGenBase::getColorMultiplierType)
_CLASSREGISTER_AddMember(getEnableSnow, BiomeGenBase::getEnableSnow)
_CLASSREGISTER_AddMember(getFloatRainfall, BiomeGenBase::getFloatRainfall)
_CLASSREGISTER_AddMember(getFloatTemperature, BiomeGenBase::getFloatTemperature)
_CLASSREGISTER_AddMember(getIntRainfall, BiomeGenBase::getIntRainfall)
_CLASSREGISTER_AddMember(getIntTemperature, BiomeGenBase::getIntTemperature)
_CLASSREGISTER_AddMember(getRandomWorldGenForGrass, BiomeGenBase::getRandomWorldGenForGrass)
_CLASSREGISTER_AddMember(getRandomWorldGenForTrees, BiomeGenBase::getRandomWorldGenForTrees)
_CLASSREGISTER_AddMember(getSkyColorByTemp, BiomeGenBase::getSkyColorByTemp)
_CLASSREGISTER_AddMember(getSpawningChance, BiomeGenBase::getSpawningChance)
_CLASSREGISTER_AddStaticMember(initialize, BiomeGenBase::initialize)
_CLASSREGISTER_AddMember(isHighHumidity, BiomeGenBase::isHighHumidity)
_CLASSREGISTER_AddStaticMember(uninitialize, BiomeGenBase::uninitialize)
_END_REGISTER_CLASS()




// Register Class BiomeGenBeach
_BEGIN_REGISTER_CLASS(BiomeGenBeach)
_CLASSREGISTER_AddBaseClass(BiomeGenBase)
_CLASSREGISTER_AddCtor(int )
_END_REGISTER_CLASS()




// Register Class BiomeGenDesert
_BEGIN_REGISTER_CLASS(BiomeGenDesert)
_CLASSREGISTER_AddBaseClass(BiomeGenBase)
_CLASSREGISTER_AddCtor(int )
_CLASSREGISTER_AddMember(decorate, BiomeGenDesert::decorate)
_END_REGISTER_CLASS()




// Register Class BiomeGenEnd
_BEGIN_REGISTER_CLASS(BiomeGenEnd)
_CLASSREGISTER_AddBaseClass(BiomeGenBase)
_CLASSREGISTER_AddCtor(int )
_CLASSREGISTER_AddMember(getSkyColorByTemp, BiomeGenEnd::getSkyColorByTemp)
_END_REGISTER_CLASS()




// Register Class BiomeGenForest
_BEGIN_REGISTER_CLASS(BiomeGenForest)
_CLASSREGISTER_AddBaseClass(BiomeGenBase)
_CLASSREGISTER_AddCtor(int )
_CLASSREGISTER_AddMember(getRandomWorldGenForTrees, BiomeGenForest::getRandomWorldGenForTrees)
_END_REGISTER_CLASS()




// Register Class BiomeGenHell
_BEGIN_REGISTER_CLASS(BiomeGenHell)
_CLASSREGISTER_AddBaseClass(BiomeGenBase)
_CLASSREGISTER_AddCtor(int )
_END_REGISTER_CLASS()




// Register Class BiomeGenHills
_BEGIN_REGISTER_CLASS(BiomeGenHills)
_CLASSREGISTER_AddBaseClass(BiomeGenBase)
_CLASSREGISTER_AddCtor(int )
_CLASSREGISTER_AddMember(decorate, BiomeGenHills::decorate)
_END_REGISTER_CLASS()




// Register Class BiomeGenJungle
_BEGIN_REGISTER_CLASS(BiomeGenJungle)
_CLASSREGISTER_AddBaseClass(BiomeGenBase)
_CLASSREGISTER_AddCtor(int )
_CLASSREGISTER_AddMember(decorate, BiomeGenJungle::decorate)
_CLASSREGISTER_AddMember(getRandomWorldGenForGrass, BiomeGenJungle::getRandomWorldGenForGrass)
_CLASSREGISTER_AddMember(getRandomWorldGenForTrees, BiomeGenJungle::getRandomWorldGenForTrees)
_END_REGISTER_CLASS()




// Register Class BiomeGenMushroomIsland
_BEGIN_REGISTER_CLASS(BiomeGenMushroomIsland)
_CLASSREGISTER_AddBaseClass(BiomeGenBase)
_CLASSREGISTER_AddCtor(int )
_END_REGISTER_CLASS()




// Register Class BiomeGenOcean
_BEGIN_REGISTER_CLASS(BiomeGenOcean)
_CLASSREGISTER_AddBaseClass(BiomeGenBase)
_CLASSREGISTER_AddCtor(int )
_END_REGISTER_CLASS()




// Register Class BiomeGenPlains
_BEGIN_REGISTER_CLASS(BiomeGenPlains)
_CLASSREGISTER_AddBaseClass(BiomeGenBase)
_CLASSREGISTER_AddCtor(int )
_END_REGISTER_CLASS()




// Register Class BiomeGenRiver
_BEGIN_REGISTER_CLASS(BiomeGenRiver)
_CLASSREGISTER_AddBaseClass(BiomeGenBase)
_CLASSREGISTER_AddCtor(int )
_END_REGISTER_CLASS()




// Register Class BiomeGenSnow
_BEGIN_REGISTER_CLASS(BiomeGenSnow)
_CLASSREGISTER_AddBaseClass(BiomeGenBase)
_CLASSREGISTER_AddCtor(int )
_END_REGISTER_CLASS()




// Register Class BiomeGenSwamp
_BEGIN_REGISTER_CLASS(BiomeGenSwamp)
_CLASSREGISTER_AddBaseClass(BiomeGenBase)
_CLASSREGISTER_AddCtor(int )
_CLASSREGISTER_AddMember(getColorMultiplierType, BiomeGenSwamp::getColorMultiplierType)
_CLASSREGISTER_AddMember(getRandomWorldGenForTrees, BiomeGenSwamp::getRandomWorldGenForTrees)
_END_REGISTER_CLASS()




// Register Class BiomeGenTaiga
_BEGIN_REGISTER_CLASS(BiomeGenTaiga)
_CLASSREGISTER_AddBaseClass(BiomeGenBase)
_CLASSREGISTER_AddCtor(int )
_CLASSREGISTER_AddMember(getRandomWorldGenForTrees, BiomeGenTaiga::getRandomWorldGenForTrees)
_END_REGISTER_CLASS()




// Register Class ChunkProviderEnd
_BEGIN_REGISTER_CLASS(ChunkProviderEnd)
_CLASSREGISTER_AddBaseClass(IChunkProvider)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(World * , i64 )
_CLASSREGISTER_AddMember(canSave, ChunkProviderEnd::canSave)
_CLASSREGISTER_AddMember(chunkExists, ChunkProviderEnd::chunkExists)
_CLASSREGISTER_AddMember(findClosestStructure, ChunkProviderEnd::findClosestStructure)
_CLASSREGISTER_AddMember(generateTerrain, ChunkProviderEnd::generateTerrain)
_CLASSREGISTER_AddMember(getLoadedChunkCount, ChunkProviderEnd::getLoadedChunkCount)
_CLASSREGISTER_AddMember(initializeNoiseField, ChunkProviderEnd::initializeNoiseField)
_CLASSREGISTER_AddMember(loadChunk, ChunkProviderEnd::loadChunk)
_CLASSREGISTER_AddMember(makeString, ChunkProviderEnd::makeString)
_CLASSREGISTER_AddMember(populate, ChunkProviderEnd::populate)
_CLASSREGISTER_AddMember(provideChunk, ChunkProviderEnd::provideChunk)
_CLASSREGISTER_AddMember(recreateStructures, ChunkProviderEnd::recreateStructures)
_CLASSREGISTER_AddMember(replaceBlocksForBiome, ChunkProviderEnd::replaceBlocksForBiome)
_CLASSREGISTER_AddMember(saveChunks, ChunkProviderEnd::saveChunks)
_CLASSREGISTER_AddMember(unloadQueuedChunks, ChunkProviderEnd::unloadQueuedChunks)
_END_REGISTER_CLASS()




// Register Class ChunkProviderFile
_BEGIN_REGISTER_CLASS(ChunkProviderFile)
_CLASSREGISTER_AddBaseClass(IChunkProvider)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(World * , size_t )
_CLASSREGISTER_AddMember(canSave, ChunkProviderFile::canSave)
_CLASSREGISTER_AddMember(chunkExists, ChunkProviderFile::chunkExists)
_CLASSREGISTER_AddMember(findClosestStructure, ChunkProviderFile::findClosestStructure)
_CLASSREGISTER_AddMember(getLoadedChunkCount, ChunkProviderFile::getLoadedChunkCount)
_CLASSREGISTER_AddMember(loadAllChunks, ChunkProviderFile::loadAllChunks)
_CLASSREGISTER_AddMember(loadChunk, ChunkProviderFile::loadChunk)
_CLASSREGISTER_AddMember(makeString, ChunkProviderFile::makeString)
_CLASSREGISTER_AddMember(populate, ChunkProviderFile::populate)
_CLASSREGISTER_AddMember(provideChunk, ChunkProviderFile::provideChunk)
_CLASSREGISTER_AddMember(recreateStructures, ChunkProviderFile::recreateStructures)
_CLASSREGISTER_AddMember(saveChunks, ChunkProviderFile::saveChunks)
_CLASSREGISTER_AddMember(scheduleLoadChunk, ChunkProviderFile::scheduleLoadChunk)
_END_REGISTER_CLASS()




// Register Class ChunkProviderFlat
_BEGIN_REGISTER_CLASS(ChunkProviderFlat)
_CLASSREGISTER_AddBaseClass(IChunkProvider)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(World * , i64 , bool , const String & )
_CLASSREGISTER_AddMember(canSave, ChunkProviderFlat::canSave)
_CLASSREGISTER_AddMember(chunkExists, ChunkProviderFlat::chunkExists)
_CLASSREGISTER_AddMember(findClosestStructure, ChunkProviderFlat::findClosestStructure)
_CLASSREGISTER_AddMember(getLoadedChunkCount, ChunkProviderFlat::getLoadedChunkCount)
_CLASSREGISTER_AddMember(loadChunk, ChunkProviderFlat::loadChunk)
_CLASSREGISTER_AddMember(makeString, ChunkProviderFlat::makeString)
_CLASSREGISTER_AddMember(populate, ChunkProviderFlat::populate)
_CLASSREGISTER_AddMember(provideChunk, ChunkProviderFlat::provideChunk)
_CLASSREGISTER_AddMember(recreateStructures, ChunkProviderFlat::recreateStructures)
_CLASSREGISTER_AddMember(saveChunks, ChunkProviderFlat::saveChunks)
_CLASSREGISTER_AddMember(unloadQueuedChunks, ChunkProviderFlat::unloadQueuedChunks)
_END_REGISTER_CLASS()




// Register Class ChunkProviderGenerate
_BEGIN_REGISTER_CLASS(ChunkProviderGenerate)
_CLASSREGISTER_AddBaseClass(IChunkProvider)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(World * , i64 , bool )
_CLASSREGISTER_AddMember(canSave, ChunkProviderGenerate::canSave)
_CLASSREGISTER_AddMember(chunkExists, ChunkProviderGenerate::chunkExists)
_CLASSREGISTER_AddMember(findClosestStructure, ChunkProviderGenerate::findClosestStructure)
_CLASSREGISTER_AddMember(generateTerrain, ChunkProviderGenerate::generateTerrain)
_CLASSREGISTER_AddMember(getLoadedChunkCount, ChunkProviderGenerate::getLoadedChunkCount)
_CLASSREGISTER_AddMember(initializeNoiseField, ChunkProviderGenerate::initializeNoiseField)
_CLASSREGISTER_AddMember(loadChunk, ChunkProviderGenerate::loadChunk)
_CLASSREGISTER_AddMember(makeString, ChunkProviderGenerate::makeString)
_CLASSREGISTER_AddMember(populate, ChunkProviderGenerate::populate)
_CLASSREGISTER_AddMember(provideChunk, ChunkProviderGenerate::provideChunk)
_CLASSREGISTER_AddMember(recreateStructures, ChunkProviderGenerate::recreateStructures)
_CLASSREGISTER_AddMember(replaceBlocksForBiome, ChunkProviderGenerate::replaceBlocksForBiome)
_CLASSREGISTER_AddMember(saveChunks, ChunkProviderGenerate::saveChunks)
_CLASSREGISTER_AddMember(unloadQueuedChunks, ChunkProviderGenerate::unloadQueuedChunks)
_END_REGISTER_CLASS()




// Register Class ChunkProviderHell
_BEGIN_REGISTER_CLASS(ChunkProviderHell)
_CLASSREGISTER_AddBaseClass(IChunkProvider)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(World * , i64 )
_CLASSREGISTER_AddMember(canSave, ChunkProviderHell::canSave)
_CLASSREGISTER_AddMember(chunkExists, ChunkProviderHell::chunkExists)
_CLASSREGISTER_AddMember(findClosestStructure, ChunkProviderHell::findClosestStructure)
_CLASSREGISTER_AddMember(generateNetherTerrain, ChunkProviderHell::generateNetherTerrain)
_CLASSREGISTER_AddMember(getLoadedChunkCount, ChunkProviderHell::getLoadedChunkCount)
_CLASSREGISTER_AddMember(initializeNoiseField, ChunkProviderHell::initializeNoiseField)
_CLASSREGISTER_AddMember(loadChunk, ChunkProviderHell::loadChunk)
_CLASSREGISTER_AddMember(makeString, ChunkProviderHell::makeString)
_CLASSREGISTER_AddMember(populate, ChunkProviderHell::populate)
_CLASSREGISTER_AddMember(provideChunk, ChunkProviderHell::provideChunk)
_CLASSREGISTER_AddMember(recreateStructures, ChunkProviderHell::recreateStructures)
_CLASSREGISTER_AddMember(replaceBlocksForBiome, ChunkProviderHell::replaceBlocksForBiome)
_CLASSREGISTER_AddMember(saveChunks, ChunkProviderHell::saveChunks)
_CLASSREGISTER_AddMember(unloadQueuedChunks, ChunkProviderHell::unloadQueuedChunks)
_END_REGISTER_CLASS()






// Register Class FlatGeneratorInfo
_BEGIN_REGISTER_CLASS(FlatGeneratorInfo)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddStaticMember(createFlatGeneratorFromString, FlatGeneratorInfo::createFlatGeneratorFromString)
_CLASSREGISTER_AddMember(getBiome, FlatGeneratorInfo::getBiome)
_CLASSREGISTER_AddStaticMember_Override(getDefaultFlatGenerator, FlatGeneratorInfo::getDefaultFlatGenerator,FlatGeneratorInfo * )
_CLASSREGISTER_AddMember(getFlatLayers, FlatGeneratorInfo::getFlatLayers)
_CLASSREGISTER_AddMember(getWorldFeatures, FlatGeneratorInfo::getWorldFeatures)
_CLASSREGISTER_AddMember(setBiome, FlatGeneratorInfo::setBiome)
_CLASSREGISTER_AddMember(toString, FlatGeneratorInfo::toString)
_CLASSREGISTER_AddMember(updateLayers, FlatGeneratorInfo::updateLayers)
_END_REGISTER_CLASS()




// Register Class FlatLayerInfo
_BEGIN_REGISTER_CLASS(FlatLayerInfo)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(int , int )
_CLASSREGISTER_AddCtor(int , int , int )
_CLASSREGISTER_AddMember(getFillBlock, FlatLayerInfo::getFillBlock)
_CLASSREGISTER_AddMember(getFillBlockMeta, FlatLayerInfo::getFillBlockMeta)
_CLASSREGISTER_AddMember(getLayerCount, FlatLayerInfo::getLayerCount)
_CLASSREGISTER_AddMember(getMinY, FlatLayerInfo::getMinY)
_CLASSREGISTER_AddMember(setMinY, FlatLayerInfo::setMinY)
_CLASSREGISTER_AddMember(toString, FlatLayerInfo::toString)
_END_REGISTER_CLASS()




// Register Class GenLayer
_BEGIN_REGISTER_CLASS(GenLayer)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(addRef, GenLayer::addRef)
_CLASSREGISTER_AddMember(initChunkSeed, GenLayer::initChunkSeed)
_CLASSREGISTER_AddMember(initWorldGenSeed, GenLayer::initWorldGenSeed)
_CLASSREGISTER_AddStaticMember(initializeAllBiomeGenerators, GenLayer::initializeAllBiomeGenerators)
_CLASSREGISTER_AddMember(nextInt, GenLayer::nextInt)
_CLASSREGISTER_AddMember(setParent, GenLayer::setParent)
_CLASSREGISTER_AddMember(subRef, GenLayer::subRef)
_END_REGISTER_CLASS()




// Register Class GenLayerAddIsland
_BEGIN_REGISTER_CLASS(GenLayerAddIsland)
_CLASSREGISTER_AddBaseClass(GenLayer)
_CLASSREGISTER_AddCtor(i64 , GenLayer * )
_CLASSREGISTER_AddMember(getInts, GenLayerAddIsland::getInts)
_END_REGISTER_CLASS()




// Register Class GenLayerAddMushroomIsland
_BEGIN_REGISTER_CLASS(GenLayerAddMushroomIsland)
_CLASSREGISTER_AddBaseClass(GenLayer)
_CLASSREGISTER_AddCtor(i64 , GenLayer * )
_CLASSREGISTER_AddMember(getInts, GenLayerAddMushroomIsland::getInts)
_END_REGISTER_CLASS()




// Register Class GenLayerAddSnow
_BEGIN_REGISTER_CLASS(GenLayerAddSnow)
_CLASSREGISTER_AddBaseClass(GenLayer)
_CLASSREGISTER_AddCtor(i64 , GenLayer * )
_CLASSREGISTER_AddMember(getInts, GenLayerAddSnow::getInts)
_END_REGISTER_CLASS()




// Register Class GenLayerBiome
_BEGIN_REGISTER_CLASS(GenLayerBiome)
_CLASSREGISTER_AddBaseClass(GenLayer)
_CLASSREGISTER_AddCtor(i64 , GenLayer * , TERRAIN_TYPE )
_CLASSREGISTER_AddMember(getInts, GenLayerBiome::getInts)
_END_REGISTER_CLASS()




// Register Class GenLayerFuzzyZoom
_BEGIN_REGISTER_CLASS(GenLayerFuzzyZoom)
_CLASSREGISTER_AddBaseClass(GenLayer)
_CLASSREGISTER_AddCtor(i64 , GenLayer * )
_CLASSREGISTER_AddMember(getInts, GenLayerFuzzyZoom::getInts)
_END_REGISTER_CLASS()




// Register Class GenLayerHills
_BEGIN_REGISTER_CLASS(GenLayerHills)
_CLASSREGISTER_AddBaseClass(GenLayer)
_CLASSREGISTER_AddCtor(i64 , GenLayer * )
_CLASSREGISTER_AddMember(getInts, GenLayerHills::getInts)
_END_REGISTER_CLASS()




// Register Class GenLayerIsland
_BEGIN_REGISTER_CLASS(GenLayerIsland)
_CLASSREGISTER_AddBaseClass(GenLayer)
_CLASSREGISTER_AddCtor(i64 )
_CLASSREGISTER_AddMember(getInts, GenLayerIsland::getInts)
_END_REGISTER_CLASS()




// Register Class GenLayerRiver
_BEGIN_REGISTER_CLASS(GenLayerRiver)
_CLASSREGISTER_AddBaseClass(GenLayer)
_CLASSREGISTER_AddCtor(i64 , GenLayer * )
_CLASSREGISTER_AddMember(getInts, GenLayerRiver::getInts)
_END_REGISTER_CLASS()




// Register Class GenLayerRiverInit
_BEGIN_REGISTER_CLASS(GenLayerRiverInit)
_CLASSREGISTER_AddBaseClass(GenLayer)
_CLASSREGISTER_AddCtor(i64 , GenLayer * )
_CLASSREGISTER_AddMember(getInts, GenLayerRiverInit::getInts)
_END_REGISTER_CLASS()




// Register Class GenLayerRiverMix
_BEGIN_REGISTER_CLASS(GenLayerRiverMix)
_CLASSREGISTER_AddBaseClass(GenLayer)
_CLASSREGISTER_AddCtor(i64 , GenLayer * , GenLayer * )
_CLASSREGISTER_AddMember(getInts, GenLayerRiverMix::getInts)
_CLASSREGISTER_AddMember(initWorldGenSeed, GenLayerRiverMix::initWorldGenSeed)
_END_REGISTER_CLASS()




// Register Class GenLayerShore
_BEGIN_REGISTER_CLASS(GenLayerShore)
_CLASSREGISTER_AddBaseClass(GenLayer)
_CLASSREGISTER_AddCtor(i64 , GenLayer * )
_CLASSREGISTER_AddMember(getInts, GenLayerShore::getInts)
_END_REGISTER_CLASS()




// Register Class GenLayerSmooth
_BEGIN_REGISTER_CLASS(GenLayerSmooth)
_CLASSREGISTER_AddBaseClass(GenLayer)
_CLASSREGISTER_AddCtor(i64 , GenLayer * )
_CLASSREGISTER_AddMember(getInts, GenLayerSmooth::getInts)
_END_REGISTER_CLASS()




// Register Class GenLayerSwampRivers
_BEGIN_REGISTER_CLASS(GenLayerSwampRivers)
_CLASSREGISTER_AddBaseClass(GenLayer)
_CLASSREGISTER_AddCtor(i64 , GenLayer * )
_CLASSREGISTER_AddMember(getInts, GenLayerSwampRivers::getInts)
_END_REGISTER_CLASS()




// Register Class GenLayerVoronoiZoom
_BEGIN_REGISTER_CLASS(GenLayerVoronoiZoom)
_CLASSREGISTER_AddBaseClass(GenLayer)
_CLASSREGISTER_AddCtor(i64 , GenLayer * )
_CLASSREGISTER_AddMember(getInts, GenLayerVoronoiZoom::getInts)
_END_REGISTER_CLASS()




// Register Class GenLayerZoom
_BEGIN_REGISTER_CLASS(GenLayerZoom)
_CLASSREGISTER_AddBaseClass(GenLayer)
_CLASSREGISTER_AddCtor(i64 , GenLayer * )
_CLASSREGISTER_AddMember(getInts, GenLayerZoom::getInts)
_CLASSREGISTER_AddStaticMember(magnify, GenLayerZoom::magnify)
_END_REGISTER_CLASS()




// Register Class MapGenBase
_BEGIN_REGISTER_CLASS(MapGenBase)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(generate, MapGenBase::generate)
_CLASSREGISTER_AddMember(recursiveGenerate, MapGenBase::recursiveGenerate)
_END_REGISTER_CLASS()




// Register Class MapGenCaves
_BEGIN_REGISTER_CLASS(MapGenCaves)
_CLASSREGISTER_AddBaseClass(MapGenBase)
_CLASSREGISTER_AddMember(generateCaveNode, MapGenCaves::generateCaveNode)
_CLASSREGISTER_AddMember(generateLargeCaveNode, MapGenCaves::generateLargeCaveNode)
_CLASSREGISTER_AddMember(recursiveGenerate, MapGenCaves::recursiveGenerate)
_END_REGISTER_CLASS()




// Register Class MapGenCavesHell
_BEGIN_REGISTER_CLASS(MapGenCavesHell)
_CLASSREGISTER_AddBaseClass(MapGenBase)
_CLASSREGISTER_AddMember(generateCaveNode, MapGenCavesHell::generateCaveNode)
_CLASSREGISTER_AddMember(generateLargeCaveNode, MapGenCavesHell::generateLargeCaveNode)
_CLASSREGISTER_AddMember(recursiveGenerate, MapGenCavesHell::recursiveGenerate)
_END_REGISTER_CLASS()




// Register Class MapGenMineshaft
_BEGIN_REGISTER_CLASS(MapGenMineshaft)
_CLASSREGISTER_AddBaseClass(MapGenStructure)
_CLASSREGISTER_AddCtor()
_END_REGISTER_CLASS()




// Register Class MapGenNetherBridge
_BEGIN_REGISTER_CLASS(MapGenNetherBridge)
_CLASSREGISTER_AddBaseClass(MapGenStructure)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(canSpawnStructureAtCoords, MapGenNetherBridge::canSpawnStructureAtCoords)
_CLASSREGISTER_AddMember(getSpawnList, MapGenNetherBridge::getSpawnList)
_CLASSREGISTER_AddMember(getStructureStart, MapGenNetherBridge::getStructureStart)
_END_REGISTER_CLASS()




// Register Class MapGenRavine
_BEGIN_REGISTER_CLASS(MapGenRavine)
_CLASSREGISTER_AddBaseClass(MapGenBase)
//_CLASSREGISTER_AddMember(generateRavine, MapGenRavine::generateRavine)
_CLASSREGISTER_AddMember(recursiveGenerate, MapGenRavine::recursiveGenerate)
_END_REGISTER_CLASS()




// Register Class MapGenStructure
_BEGIN_REGISTER_CLASS(MapGenStructure)
_CLASSREGISTER_AddBaseClass(MapGenBase)
_CLASSREGISTER_AddMember(func_142038_b, MapGenStructure::func_142038_b)
_CLASSREGISTER_AddMember(generateStructuresInChunk, MapGenStructure::generateStructuresInChunk)
_CLASSREGISTER_AddMember(getNearestInstance, MapGenStructure::getNearestInstance)
_CLASSREGISTER_AddMember(hasStructureAt, MapGenStructure::hasStructureAt)
_CLASSREGISTER_AddMember(recursiveGenerate, MapGenStructure::recursiveGenerate)
_END_REGISTER_CLASS()






// Register Class NoiseGeneratorOctaves
_BEGIN_REGISTER_CLASS(NoiseGeneratorOctaves)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(Random * , int )
_CLASSREGISTER_AddMember_Override(generateNoiseOctaves, NoiseGeneratorOctaves::generateNoiseOctaves,double * ,double * ,int ,int ,int ,int ,int ,int ,double ,double ,double )
_CLASSREGISTER_AddMember_Override(generateNoiseOctaves1, NoiseGeneratorOctaves::generateNoiseOctaves,double * ,double * ,int ,int ,int ,int ,double ,double )
_END_REGISTER_CLASS()




// Register Class NoiseGeneratorPerlin
_BEGIN_REGISTER_CLASS(NoiseGeneratorPerlin)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(Random * )
_END_REGISTER_CLASS()




































// Register Class ComponentMineshaftCorridor
_BEGIN_REGISTER_CLASS(ComponentMineshaftCorridor)
_CLASSREGISTER_AddBaseClass(StructureComponent)
_CLASSREGISTER_AddCtor(int , Random & , const StructureBB & , int )
_CLASSREGISTER_AddMember(addComponentParts, ComponentMineshaftCorridor::addComponentParts)
_CLASSREGISTER_AddMember(buildComponent, ComponentMineshaftCorridor::buildComponent)
_CLASSREGISTER_AddStaticMember(findValidPlacement, ComponentMineshaftCorridor::findValidPlacement)
_CLASSREGISTER_AddMember(generateStructureChestContents, ComponentMineshaftCorridor::generateStructureChestContents)
_END_REGISTER_CLASS()




// Register Class ComponentMineshaftCross
_BEGIN_REGISTER_CLASS(ComponentMineshaftCross)
_CLASSREGISTER_AddBaseClass(StructureComponent)
_CLASSREGISTER_AddCtor(int , Random , const StructureBB & , int )
_CLASSREGISTER_AddMember(addComponentParts, ComponentMineshaftCross::addComponentParts)
_CLASSREGISTER_AddMember(buildComponent, ComponentMineshaftCross::buildComponent)
_CLASSREGISTER_AddStaticMember(findValidPlacement, ComponentMineshaftCross::findValidPlacement)
_END_REGISTER_CLASS()




// Register Class ComponentMineshaftRoom
_BEGIN_REGISTER_CLASS(ComponentMineshaftRoom)
_CLASSREGISTER_AddBaseClass(StructureComponent)
_CLASSREGISTER_AddCtor(int , Random & , int , int )
_CLASSREGISTER_AddMember(addComponentParts, ComponentMineshaftRoom::addComponentParts)
_CLASSREGISTER_AddMember(buildComponent, ComponentMineshaftRoom::buildComponent)
_END_REGISTER_CLASS()




// Register Class ComponentMineshaftStairs
_BEGIN_REGISTER_CLASS(ComponentMineshaftStairs)
_CLASSREGISTER_AddBaseClass(StructureComponent)
_CLASSREGISTER_AddCtor(int , Random & , const StructureBB & , int )
_CLASSREGISTER_AddMember(addComponentParts, ComponentMineshaftStairs::addComponentParts)
_CLASSREGISTER_AddMember(buildComponent, ComponentMineshaftStairs::buildComponent)
_CLASSREGISTER_AddStaticMember(findValidPlacement, ComponentMineshaftStairs::findValidPlacement)
_END_REGISTER_CLASS()




// Register Class ComponentNetherBridgeCorridor
_BEGIN_REGISTER_CLASS(ComponentNetherBridgeCorridor)
_CLASSREGISTER_AddBaseClass(ComponentNetherBridgePiece)
_CLASSREGISTER_AddCtor(int , Random & , const StructureBB & , int )
_CLASSREGISTER_AddMember(addComponentParts, ComponentNetherBridgeCorridor::addComponentParts)
_CLASSREGISTER_AddMember(buildComponent, ComponentNetherBridgeCorridor::buildComponent)
_CLASSREGISTER_AddStaticMember(createValidComponent, ComponentNetherBridgeCorridor::createValidComponent)
_END_REGISTER_CLASS()




// Register Class ComponentNetherBridgeCorridor2
_BEGIN_REGISTER_CLASS(ComponentNetherBridgeCorridor2)
_CLASSREGISTER_AddBaseClass(ComponentNetherBridgePiece)
_CLASSREGISTER_AddCtor(int , Random & , const StructureBB & , int )
_CLASSREGISTER_AddMember(addComponentParts, ComponentNetherBridgeCorridor2::addComponentParts)
_CLASSREGISTER_AddMember(buildComponent, ComponentNetherBridgeCorridor2::buildComponent)
_CLASSREGISTER_AddStaticMember(createValidComponent, ComponentNetherBridgeCorridor2::createValidComponent)
_END_REGISTER_CLASS()




// Register Class ComponentNetherBridgeCorridor3
_BEGIN_REGISTER_CLASS(ComponentNetherBridgeCorridor3)
_CLASSREGISTER_AddBaseClass(ComponentNetherBridgePiece)
_CLASSREGISTER_AddCtor(int , Random & , const StructureBB & , int )
_CLASSREGISTER_AddMember(addComponentParts, ComponentNetherBridgeCorridor3::addComponentParts)
_CLASSREGISTER_AddMember(buildComponent, ComponentNetherBridgeCorridor3::buildComponent)
_CLASSREGISTER_AddStaticMember(createValidComponent, ComponentNetherBridgeCorridor3::createValidComponent)
_END_REGISTER_CLASS()




// Register Class ComponentNetherBridgeCorridor4
_BEGIN_REGISTER_CLASS(ComponentNetherBridgeCorridor4)
_CLASSREGISTER_AddBaseClass(ComponentNetherBridgePiece)
_CLASSREGISTER_AddCtor(int , Random & , const StructureBB & , int )
_CLASSREGISTER_AddMember(addComponentParts, ComponentNetherBridgeCorridor4::addComponentParts)
_CLASSREGISTER_AddMember(buildComponent, ComponentNetherBridgeCorridor4::buildComponent)
_CLASSREGISTER_AddStaticMember(createValidComponent, ComponentNetherBridgeCorridor4::createValidComponent)
_END_REGISTER_CLASS()




// Register Class ComponentNetherBridgeCorridor5
_BEGIN_REGISTER_CLASS(ComponentNetherBridgeCorridor5)
_CLASSREGISTER_AddBaseClass(ComponentNetherBridgePiece)
_CLASSREGISTER_AddCtor(int , Random & , const StructureBB & , int )
_CLASSREGISTER_AddMember(addComponentParts, ComponentNetherBridgeCorridor5::addComponentParts)
_CLASSREGISTER_AddMember(buildComponent, ComponentNetherBridgeCorridor5::buildComponent)
_CLASSREGISTER_AddStaticMember(createValidComponent, ComponentNetherBridgeCorridor5::createValidComponent)
_END_REGISTER_CLASS()




// Register Class ComponentNetherBridgeCrossing
_BEGIN_REGISTER_CLASS(ComponentNetherBridgeCrossing)
_CLASSREGISTER_AddBaseClass(ComponentNetherBridgePiece)
_CLASSREGISTER_AddCtor(int , Random & , const StructureBB & , int )
_CLASSREGISTER_AddMember(addComponentParts, ComponentNetherBridgeCrossing::addComponentParts)
_CLASSREGISTER_AddMember(buildComponent, ComponentNetherBridgeCrossing::buildComponent)
_CLASSREGISTER_AddStaticMember(createValidComponent, ComponentNetherBridgeCrossing::createValidComponent)
_END_REGISTER_CLASS()




// Register Class ComponentNetherBridgeCrossing2
_BEGIN_REGISTER_CLASS(ComponentNetherBridgeCrossing2)
_CLASSREGISTER_AddBaseClass(ComponentNetherBridgePiece)
_CLASSREGISTER_AddCtor(int , Random & , const StructureBB & , int )
_CLASSREGISTER_AddMember(addComponentParts, ComponentNetherBridgeCrossing2::addComponentParts)
_CLASSREGISTER_AddMember(buildComponent, ComponentNetherBridgeCrossing2::buildComponent)
_CLASSREGISTER_AddStaticMember(createValidComponent, ComponentNetherBridgeCrossing2::createValidComponent)
_END_REGISTER_CLASS()




// Register Class ComponentNetherBridgeCrossing3
_BEGIN_REGISTER_CLASS(ComponentNetherBridgeCrossing3)
_CLASSREGISTER_AddBaseClass(ComponentNetherBridgePiece)
_CLASSREGISTER_AddCtor(int , Random & , const StructureBB & , int )
_CLASSREGISTER_AddCtor(Random & , int , int )
_CLASSREGISTER_AddMember(addComponentParts, ComponentNetherBridgeCrossing3::addComponentParts)
_CLASSREGISTER_AddMember(buildComponent, ComponentNetherBridgeCrossing3::buildComponent)
_CLASSREGISTER_AddStaticMember(createValidComponent, ComponentNetherBridgeCrossing3::createValidComponent)
_END_REGISTER_CLASS()




// Register Class ComponentNetherBridgeEntrance
_BEGIN_REGISTER_CLASS(ComponentNetherBridgeEntrance)
_CLASSREGISTER_AddBaseClass(ComponentNetherBridgePiece)
_CLASSREGISTER_AddCtor(int , Random & , const StructureBB & , int )
_CLASSREGISTER_AddMember(addComponentParts, ComponentNetherBridgeEntrance::addComponentParts)
_CLASSREGISTER_AddMember(buildComponent, ComponentNetherBridgeEntrance::buildComponent)
_CLASSREGISTER_AddStaticMember(createValidComponent, ComponentNetherBridgeEntrance::createValidComponent)
_END_REGISTER_CLASS()




// Register Class ComponentNetherBridgeNetherStalkRoom
_BEGIN_REGISTER_CLASS(ComponentNetherBridgeNetherStalkRoom)
_CLASSREGISTER_AddBaseClass(ComponentNetherBridgePiece)
_CLASSREGISTER_AddCtor(int , Random & , const StructureBB & , int )
_CLASSREGISTER_AddMember(addComponentParts, ComponentNetherBridgeNetherStalkRoom::addComponentParts)
_CLASSREGISTER_AddMember(buildComponent, ComponentNetherBridgeNetherStalkRoom::buildComponent)
_CLASSREGISTER_AddStaticMember(createValidComponent, ComponentNetherBridgeNetherStalkRoom::createValidComponent)
_END_REGISTER_CLASS()




// Register Class ComponentNetherBridgePiece
_BEGIN_REGISTER_CLASS(ComponentNetherBridgePiece)
_CLASSREGISTER_AddBaseClass(StructureComponent)
_END_REGISTER_CLASS()




// Register Class ComponentNetherBridgeStairs
_BEGIN_REGISTER_CLASS(ComponentNetherBridgeStairs)
_CLASSREGISTER_AddBaseClass(ComponentNetherBridgePiece)
_CLASSREGISTER_AddCtor(int , Random & , const StructureBB & , int )
_CLASSREGISTER_AddMember(addComponentParts, ComponentNetherBridgeStairs::addComponentParts)
_CLASSREGISTER_AddMember(buildComponent, ComponentNetherBridgeStairs::buildComponent)
_CLASSREGISTER_AddStaticMember(createValidComponent, ComponentNetherBridgeStairs::createValidComponent)
_END_REGISTER_CLASS()




// Register Class ComponentNetherBridgeStartPiece
_BEGIN_REGISTER_CLASS(ComponentNetherBridgeStartPiece)
_CLASSREGISTER_AddBaseClass(ComponentNetherBridgeCrossing3)
_CLASSREGISTER_AddCtor(Random & , int , int )
_END_REGISTER_CLASS()




// Register Class ComponentNetherBridgeStraight
_BEGIN_REGISTER_CLASS(ComponentNetherBridgeStraight)
_CLASSREGISTER_AddBaseClass(ComponentNetherBridgePiece)
_CLASSREGISTER_AddCtor(int , Random & , const StructureBB & , int )
_CLASSREGISTER_AddMember(addComponentParts, ComponentNetherBridgeStraight::addComponentParts)
_CLASSREGISTER_AddMember(buildComponent, ComponentNetherBridgeStraight::buildComponent)
_CLASSREGISTER_AddStaticMember(createValidComponent, ComponentNetherBridgeStraight::createValidComponent)
_END_REGISTER_CLASS()




// Register Class ComponentNetherBridgeThrone
_BEGIN_REGISTER_CLASS(ComponentNetherBridgeThrone)
_CLASSREGISTER_AddBaseClass(ComponentNetherBridgePiece)
_CLASSREGISTER_AddCtor(int , Random & , const StructureBB & , int )
_CLASSREGISTER_AddMember(addComponentParts, ComponentNetherBridgeThrone::addComponentParts)
_CLASSREGISTER_AddStaticMember(createValidComponent, ComponentNetherBridgeThrone::createValidComponent)
_END_REGISTER_CLASS()




// Register Class StructureComponent
_BEGIN_REGISTER_CLASS(StructureComponent)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(buildComponent, StructureComponent::buildComponent)
_CLASSREGISTER_AddMember(clearCurrentPositionBlocksUpwards, StructureComponent::clearCurrentPositionBlocksUpwards)
_CLASSREGISTER_AddMember(fillCurrentPositionBlocksDownwards, StructureComponent::fillCurrentPositionBlocksDownwards)
_CLASSREGISTER_AddMember(fillWithAir, StructureComponent::fillWithAir)
//_CLASSREGISTER_AddMember(fillWithBlocks, StructureComponent::fillWithBlocks)
//_CLASSREGISTER_AddMember(fillWithMetadataBlocks, StructureComponent::fillWithMetadataBlocks)
//_CLASSREGISTER_AddMember(fillWithRandomizedBlocks, StructureComponent::fillWithRandomizedBlocks)
_CLASSREGISTER_AddStaticMember(findIntersecting, StructureComponent::findIntersecting)
_CLASSREGISTER_AddMember(generateStructureChestContents, StructureComponent::generateStructureChestContents)
_CLASSREGISTER_AddMember(getBlockIdAtCurrentPosition, StructureComponent::getBlockIdAtCurrentPosition)
_CLASSREGISTER_AddMember_Override(getBoundingBox, StructureComponent::getBoundingBox,const StructureBB & )
_CLASSREGISTER_AddMember_Override(getBoundingBox1, StructureComponent::getBoundingBox,StructureBB & )
_CLASSREGISTER_AddMember(getCenter, StructureComponent::getCenter)
_CLASSREGISTER_AddMember(getComponentType, StructureComponent::getComponentType)
_CLASSREGISTER_AddMember(getMetadataWithOffset, StructureComponent::getMetadataWithOffset)
_CLASSREGISTER_AddMember(getXWithOffset, StructureComponent::getXWithOffset)
_CLASSREGISTER_AddMember(getYWithOffset, StructureComponent::getYWithOffset)
_CLASSREGISTER_AddMember(getZWithOffset, StructureComponent::getZWithOffset)
_CLASSREGISTER_AddMember(isLiquidInStructureBoundingBox, StructureComponent::isLiquidInStructureBoundingBox)
_CLASSREGISTER_AddMember(placeBlockAtCurrentPosition, StructureComponent::placeBlockAtCurrentPosition)
_CLASSREGISTER_AddMember(placeDoorAtCurrentPosition, StructureComponent::placeDoorAtCurrentPosition)
//_CLASSREGISTER_AddMember(randomlyFillWithBlocks, StructureComponent::randomlyFillWithBlocks)
_CLASSREGISTER_AddMember(randomlyPlaceBlock, StructureComponent::randomlyPlaceBlock)
_CLASSREGISTER_AddMember(randomlyRareFillWithBlocks, StructureComponent::randomlyRareFillWithBlocks)
_END_REGISTER_CLASS()




// Register Class StructureNetherBridgePieceWeight
_BEGIN_REGISTER_CLASS(StructureNetherBridgePieceWeight)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(int , int , int , bool )
_CLASSREGISTER_AddCtor(int , int , int )
_CLASSREGISTER_AddMember(func_78822_a, StructureNetherBridgePieceWeight::func_78822_a)
_CLASSREGISTER_AddMember(func_78823_a, StructureNetherBridgePieceWeight::func_78823_a)
_END_REGISTER_CLASS()




// Register Class StructurePieceBlockSelector
_BEGIN_REGISTER_CLASS(StructurePieceBlockSelector)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(getSelectedBlockId, StructurePieceBlockSelector::getSelectedBlockId)
_CLASSREGISTER_AddMember(getSelectedBlockMetaData, StructurePieceBlockSelector::getSelectedBlockMetaData)
_END_REGISTER_CLASS()




// Register Class StructureScatteredFeatureStones
_BEGIN_REGISTER_CLASS(StructureScatteredFeatureStones)
_CLASSREGISTER_AddBaseClass(StructurePieceBlockSelector)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(selectBlocks, StructureScatteredFeatureStones::selectBlocks)
_END_REGISTER_CLASS()




// Register Class StructureStrongholdStones
_BEGIN_REGISTER_CLASS(StructureStrongholdStones)
_CLASSREGISTER_AddBaseClass(StructurePieceBlockSelector)
_CLASSREGISTER_AddMember(selectBlocks, StructureStrongholdStones::selectBlocks)
_END_REGISTER_CLASS()




// Register Class StructureMineshaftPieces
_BEGIN_REGISTER_CLASS(StructureMineshaftPieces)
_CLASSREGISTER_AddStaticMember(getChestContext, StructureMineshaftPieces::getChestContext)
_CLASSREGISTER_AddStaticMember(getNextComponent, StructureMineshaftPieces::getNextComponent)
_CLASSREGISTER_AddStaticMember(getNextMineShaftComponent, StructureMineshaftPieces::getNextMineShaftComponent)
_CLASSREGISTER_AddStaticMember(getRandomComponent, StructureMineshaftPieces::getRandomComponent)
_CLASSREGISTER_AddStaticMember(initialize, StructureMineshaftPieces::initialize)
_CLASSREGISTER_AddStaticMember(uninitialize, StructureMineshaftPieces::uninitialize)
_END_REGISTER_CLASS()




// Register Class StructureNetherBridgePieces
_BEGIN_REGISTER_CLASS(StructureNetherBridgePieces)
_CLASSREGISTER_AddStaticMember(createNextComponent, StructureNetherBridgePieces::createNextComponent)
_CLASSREGISTER_AddStaticMember(createNextComponentRandom, StructureNetherBridgePieces::createNextComponentRandom)
_CLASSREGISTER_AddStaticMember(getPrimaryComponents, StructureNetherBridgePieces::getPrimaryComponents)
_CLASSREGISTER_AddStaticMember(getSecondaryComponents, StructureNetherBridgePieces::getSecondaryComponents)
_CLASSREGISTER_AddStaticMember(initialize, StructureNetherBridgePieces::initialize)
_CLASSREGISTER_AddStaticMember(uninitialize, StructureNetherBridgePieces::uninitialize)
_END_REGISTER_CLASS()




// Register Class StructureMineshaftStart
_BEGIN_REGISTER_CLASS(StructureMineshaftStart)
_CLASSREGISTER_AddBaseClass(StructureStart)
_CLASSREGISTER_AddCtor(World * , Random & , int , int )
_END_REGISTER_CLASS()




// Register Class StructureNetherBridgeStart
_BEGIN_REGISTER_CLASS(StructureNetherBridgeStart)
_CLASSREGISTER_AddBaseClass(StructureStart)
_CLASSREGISTER_AddCtor(World * , Random & , int , int )
_END_REGISTER_CLASS()




// Register Class StructureStart
_BEGIN_REGISTER_CLASS(StructureStart)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(generateStructure, StructureStart::generateStructure)
_CLASSREGISTER_AddMember(getBoundingBox, StructureStart::getBoundingBox)
_CLASSREGISTER_AddMember(getComponents, StructureStart::getComponents)
_CLASSREGISTER_AddMember(isSizeableStructure, StructureStart::isSizeableStructure)
_CLASSREGISTER_AddMember(markAvailableHeight, StructureStart::markAvailableHeight)
_CLASSREGISTER_AddMember(setRandomHeight, StructureStart::setRandomHeight)
_CLASSREGISTER_AddMember(updateBoundingBox, StructureStart::updateBoundingBox)
_END_REGISTER_CLASS()




// Register Class WorldGenBigMushroom
_BEGIN_REGISTER_CLASS(WorldGenBigMushroom)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddCtor(int )
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(generate, WorldGenBigMushroom::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenBigTree
_BEGIN_REGISTER_CLASS(WorldGenBigTree)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddCtor(bool )
_CLASSREGISTER_AddMember(checkBlockLine, WorldGenBigTree::checkBlockLine)
_CLASSREGISTER_AddMember(genTreeLayer, WorldGenBigTree::genTreeLayer)
_CLASSREGISTER_AddMember(generate, WorldGenBigTree::generate)
_CLASSREGISTER_AddMember(generateFinish, WorldGenBigTree::generateFinish)
_CLASSREGISTER_AddMember(generateLeafNode, WorldGenBigTree::generateLeafNode)
_CLASSREGISTER_AddMember(generateLeafNodeBases, WorldGenBigTree::generateLeafNodeBases)
_CLASSREGISTER_AddMember(generateLeafNodeList, WorldGenBigTree::generateLeafNodeList)
_CLASSREGISTER_AddMember(generateLeaves, WorldGenBigTree::generateLeaves)
_CLASSREGISTER_AddMember(generateTrunk, WorldGenBigTree::generateTrunk)
_CLASSREGISTER_AddMember(layerSize, WorldGenBigTree::layerSize)
_CLASSREGISTER_AddMember(leafNodeNeedsBase, WorldGenBigTree::leafNodeNeedsBase)
_CLASSREGISTER_AddMember(leafSize, WorldGenBigTree::leafSize)
_CLASSREGISTER_AddMember(placeBlockLine, WorldGenBigTree::placeBlockLine)
_CLASSREGISTER_AddMember(setScale, WorldGenBigTree::setScale)
_CLASSREGISTER_AddMember(validTreeLocation, WorldGenBigTree::validTreeLocation)
_END_REGISTER_CLASS()




// Register Class WorldGenCactus
_BEGIN_REGISTER_CLASS(WorldGenCactus)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddMember(generate, WorldGenCactus::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenClay
_BEGIN_REGISTER_CLASS(WorldGenClay)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddCtor(int )
_CLASSREGISTER_AddMember(generate, WorldGenClay::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenDeadBush
_BEGIN_REGISTER_CLASS(WorldGenDeadBush)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddCtor(int )
_CLASSREGISTER_AddMember(generate, WorldGenDeadBush::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenDesertWells
_BEGIN_REGISTER_CLASS(WorldGenDesertWells)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddMember(generate, WorldGenDesertWells::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenDungeons
_BEGIN_REGISTER_CLASS(WorldGenDungeons)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddMember(generate, WorldGenDungeons::generate)
_CLASSREGISTER_AddStaticMember(initialize, WorldGenDungeons::initialize)
_CLASSREGISTER_AddMember(simulateCreateChest, WorldGenDungeons::simulateCreateChest)
_CLASSREGISTER_AddMember(simulateCreateMob, WorldGenDungeons::simulateCreateMob)
_CLASSREGISTER_AddStaticMember(uninitialize, WorldGenDungeons::uninitialize)
_END_REGISTER_CLASS()




// Register Class WorldGenFire
_BEGIN_REGISTER_CLASS(WorldGenFire)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddMember(generate, WorldGenFire::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenFlowers
_BEGIN_REGISTER_CLASS(WorldGenFlowers)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddCtor(int )
_CLASSREGISTER_AddMember(generate, WorldGenFlowers::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenForest
_BEGIN_REGISTER_CLASS(WorldGenForest)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddCtor(bool )
_CLASSREGISTER_AddMember(generate, WorldGenForest::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenGlowStone1
_BEGIN_REGISTER_CLASS(WorldGenGlowStone1)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddMember(generate, WorldGenGlowStone1::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenGlowStone2
_BEGIN_REGISTER_CLASS(WorldGenGlowStone2)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddMember(generate, WorldGenGlowStone2::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenHellLava
_BEGIN_REGISTER_CLASS(WorldGenHellLava)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddCtor(int , bool )
_CLASSREGISTER_AddMember(generate, WorldGenHellLava::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenHugeTrees
_BEGIN_REGISTER_CLASS(WorldGenHugeTrees)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddCtor(bool , int , int , int )
_CLASSREGISTER_AddMember(generate, WorldGenHugeTrees::generate)
_CLASSREGISTER_AddMember(growLeaves, WorldGenHugeTrees::growLeaves)
_CLASSREGISTER_AddMember(setParam, WorldGenHugeTrees::setParam)
_END_REGISTER_CLASS()




// Register Class WorldGenLakes
_BEGIN_REGISTER_CLASS(WorldGenLakes)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddCtor(int )
_CLASSREGISTER_AddMember(generate, WorldGenLakes::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenLiquids
_BEGIN_REGISTER_CLASS(WorldGenLiquids)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddCtor(int )
_CLASSREGISTER_AddMember(generate, WorldGenLiquids::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenMinable
_BEGIN_REGISTER_CLASS(WorldGenMinable)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddCtor(int , int )
_CLASSREGISTER_AddCtor(int , int , int )
_CLASSREGISTER_AddMember(generate, WorldGenMinable::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenPumpkin
_BEGIN_REGISTER_CLASS(WorldGenPumpkin)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddMember(generate, WorldGenPumpkin::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenReed
_BEGIN_REGISTER_CLASS(WorldGenReed)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddMember(generate, WorldGenReed::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenSand
_BEGIN_REGISTER_CLASS(WorldGenSand)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddCtor(int , int )
_CLASSREGISTER_AddMember(generate, WorldGenSand::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenShrub
_BEGIN_REGISTER_CLASS(WorldGenShrub)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddCtor(int , int )
_CLASSREGISTER_AddMember(generate, WorldGenShrub::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenSpikes
_BEGIN_REGISTER_CLASS(WorldGenSpikes)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddCtor(int )
_CLASSREGISTER_AddMember(generate, WorldGenSpikes::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenSwamp
_BEGIN_REGISTER_CLASS(WorldGenSwamp)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddMember(generate, WorldGenSwamp::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenTaiga1
_BEGIN_REGISTER_CLASS(WorldGenTaiga1)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddMember(generate, WorldGenTaiga1::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenTaiga2
_BEGIN_REGISTER_CLASS(WorldGenTaiga2)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddCtor(bool )
_CLASSREGISTER_AddMember(generate, WorldGenTaiga2::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenTallGrass
_BEGIN_REGISTER_CLASS(WorldGenTallGrass)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddCtor(int , int )
_CLASSREGISTER_AddMember(generate, WorldGenTallGrass::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenTrees
_BEGIN_REGISTER_CLASS(WorldGenTrees)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddCtor(bool )
_CLASSREGISTER_AddCtor(bool , int , int , int , bool )
_CLASSREGISTER_AddMember(generate, WorldGenTrees::generate)
_CLASSREGISTER_AddMember(setParam, WorldGenTrees::setParam)
_END_REGISTER_CLASS()




// Register Class WorldGenVines
_BEGIN_REGISTER_CLASS(WorldGenVines)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddMember(generate, WorldGenVines::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenWaterlily
_BEGIN_REGISTER_CLASS(WorldGenWaterlily)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddMember(generate, WorldGenWaterlily::generate)
_END_REGISTER_CLASS()




// Register Class WorldGenerator
_BEGIN_REGISTER_CLASS(WorldGenerator)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(setBlock, WorldGenerator::setBlock)
_CLASSREGISTER_AddMember(setBlockAndMetadata, WorldGenerator::setBlockAndMetadata)
_CLASSREGISTER_AddMember(setScale, WorldGenerator::setScale)
_END_REGISTER_CLASS()




// Register Class WorldGeneratorBonusChest
_BEGIN_REGISTER_CLASS(WorldGeneratorBonusChest)
_CLASSREGISTER_AddBaseClass(WorldGenerator)
_CLASSREGISTER_AddCtor(WeightedRandomChestContent *  * , int )
_CLASSREGISTER_AddMember(generate, WorldGeneratorBonusChest::generate)
_END_REGISTER_CLASS()

*/


