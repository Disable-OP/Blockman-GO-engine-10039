
#include "Util/BitSet.h"
#include "Util/ChecksumUtil.h"
#include "Util/ChunkCoordIntPair.h"
#include "Util/ChunkPosition.h"
#include "Util/ContainerUtils.h"
#include "Util/Facing.h"
#include "Util/IO.h"
#include "Util/Random.h"
#include "Util/RingQueue.h"
#include "Util/sha1.h"
#include "Util/StructureBB.h"
#include "Util/SubscriptionGuard.h"
#include "Util/UUID.h"
#include "Util/SecTimer.h"
#include "LuaRegister/Template/LuaRegister.h"
using namespace BLOCKMAN;

using namespace LUA_REGISTER;



_REGISTER_FUNC(_get_bit)


_REGISTER_FUNC(_reset_bit)


_REGISTER_FUNC(_set_bit)

// Register Class ChunkCoordIntPair
_BEGIN_REGISTER_CLASS(ChunkCoordIntPair)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddCtor(int , int )
_CLASSREGISTER_AddCtor(const ChunkCoordIntPair & )
_CLASSREGISTER_AddStaticMember(Int2chunkXZ, ChunkCoordIntPair::Int2chunkXZ)
_CLASSREGISTER_AddStaticMember(chunkXZ2Int, ChunkCoordIntPair::chunkXZ2Int)
_CLASSREGISTER_AddMember(hashCode, ChunkCoordIntPair::hashCode)
_CLASSREGISTER_AddMember(isValid, ChunkCoordIntPair::isValid)
_CLASSREGISTER_AddMember(setPos, ChunkCoordIntPair::setPos)
_END_REGISTER_CLASS()

// Register Class ChunkPosition
_BEGIN_REGISTER_CLASS(ChunkPosition)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddCtor(int , int , int )
_CLASSREGISTER_AddCtor(const Vector3i & )
_CLASSREGISTER_AddCtor(const Vector3 & )
_CLASSREGISTER_AddMember(hashCode, ChunkPosition::hashCode)
_CLASSREGISTER_AddMember(isInvalid, ChunkPosition::isInvalid)
_CLASSREGISTER_AddMember(isValid, ChunkPosition::isValid)
_END_REGISTER_CLASS()




// Register Class Facing
_BEGIN_REGISTER_CLASS(Facing)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddStaticMember(fromAngle, Facing::fromAngle)
_CLASSREGISTER_AddMember(getAxis, Facing::getAxis)
_CLASSREGISTER_AddMember(getAxisDirection, Facing::getAxisDirection)
_CLASSREGISTER_AddStaticMember(getByName, Facing::getByName)
_CLASSREGISTER_AddMember(getDirectionVec, Facing::getDirectionVec)
_CLASSREGISTER_AddStaticMember(getFacingFromVector, Facing::getFacingFromVector)
_CLASSREGISTER_AddStaticMember(getFront, Facing::getFront)
_CLASSREGISTER_AddMember(getFrontOffset, Facing::getFrontOffset)
_CLASSREGISTER_AddMember(getFrontOffsetX, Facing::getFrontOffsetX)
_CLASSREGISTER_AddMember(getFrontOffsetY, Facing::getFrontOffsetY)
_CLASSREGISTER_AddMember(getFrontOffsetZ, Facing::getFrontOffsetZ)
_CLASSREGISTER_AddStaticMember(getHorizontal, Facing::getHorizontal)
_CLASSREGISTER_AddMember(getHorizontalIndex, Facing::getHorizontalIndex)
_CLASSREGISTER_AddMember(getIndex, Facing::getIndex)
_CLASSREGISTER_AddMember(getOpposite, Facing::getOpposite)
_CLASSREGISTER_AddStaticMember(getRandom, Facing::getRandom)
_CLASSREGISTER_AddStaticMember(initialize, Facing::initialize)
_CLASSREGISTER_AddMember(rotateAround, Facing::rotateAround)
_CLASSREGISTER_AddMember(rotateYCCW, Facing::rotateYCCW)
_CLASSREGISTER_AddStaticMember(unInitialize, Facing::unInitialize)
_END_REGISTER_CLASS()

// Register Class Random
_BEGIN_REGISTER_CLASS(Random)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddCtor(i64)
_CLASSREGISTER_AddMember(initialScramble, Random::initialScramble)
_CLASSREGISTER_AddMember(internalNextInt, Random::internalNextInt)
_CLASSREGISTER_AddMember(internalNextLong, Random::internalNextLong)
_CLASSREGISTER_AddMember(next, Random::next)
_CLASSREGISTER_AddMember(nextBool, Random::nextBool)
_CLASSREGISTER_AddMember(nextDouble, Random::nextDouble)
_CLASSREGISTER_AddMember(nextFloat, Random::nextFloat)
_CLASSREGISTER_AddMember(nextGaussian, Random::nextGaussian)
_CLASSREGISTER_AddMember(nextGaussianVec, Random::nextGaussianVec)
_CLASSREGISTER_AddMember_Override(nextInt, Random::nextInt, int)
_CLASSREGISTER_AddMember_Override(nextInt1, Random::nextInt, int, int)
_CLASSREGISTER_AddMember(nextIntRange, Random::nextIntRange)
_CLASSREGISTER_AddMember(nextLong, Random::nextLong)
_CLASSREGISTER_AddMember(nextVector, Random::nextVector)
_CLASSREGISTER_AddStaticMember(seedUniquifier, Random::seedUniquifier)
_CLASSREGISTER_AddMember(setSeed, Random::setSeed)
_END_REGISTER_CLASS()


// Register Class SHA1
_BEGIN_REGISTER_CLASS(SHA1)
_CLASSREGISTER_AddCtor()
_END_REGISTER_CLASS()


// Register Class StructureBB
_BEGIN_REGISTER_CLASS(StructureBB)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddCtor(const StructureBB &)
_CLASSREGISTER_AddCtor(int, int, int, int, int, int)
_CLASSREGISTER_AddCtor(int, int, int, int)
_CLASSREGISTER_AddMember(expandTo, StructureBB::expandTo)
_CLASSREGISTER_AddMember(getCenterX, StructureBB::getCenterX)
_CLASSREGISTER_AddMember(getCenterY, StructureBB::getCenterY)
_CLASSREGISTER_AddMember(getCenterZ, StructureBB::getCenterZ)
_CLASSREGISTER_AddStaticMember(getComponentToAddBoundingBox, StructureBB::getComponentToAddBoundingBox)
_CLASSREGISTER_AddMember(getXSize, StructureBB::getXSize)
_CLASSREGISTER_AddMember(getYSize, StructureBB::getYSize)
_CLASSREGISTER_AddMember(getZSize, StructureBB::getZSize)
_CLASSREGISTER_AddMember_Override(intersectsWith, StructureBB::intersectsWith, bool, const StructureBB &)
_CLASSREGISTER_AddMember_Override(intersectsWith1, StructureBB::intersectsWith, bool, int, int, int, int)
_CLASSREGISTER_AddMember(invalid, StructureBB::invalid)
_CLASSREGISTER_AddMember(isInvalid, StructureBB::isInvalid)
_CLASSREGISTER_AddMember_Override(isVecInside, StructureBB::isVecInside, bool, int, int, int)
_CLASSREGISTER_AddMember_Override(isVecInside1, StructureBB::isVecInside, bool, const BlockPos &)
_CLASSREGISTER_AddMember(offset, StructureBB::offset)
//_CLASSREGISTER_AddMember(set, StructureBB::set)
_CLASSREGISTER_AddMember(toString, StructureBB::toString)
_END_REGISTER_CLASS()


// Register Class UUID
_BEGIN_REGISTER_CLASS(UUID)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddCtor(i64, i64)
_CLASSREGISTER_AddMember(clockSequence, UUID::clockSequence)
_CLASSREGISTER_AddStaticMember(digits, UUID::digits)
_CLASSREGISTER_AddStaticMember(fromString, UUID::fromString)
_CLASSREGISTER_AddMember(getLeastSignificantBits, UUID::getLeastSignificantBits)
_CLASSREGISTER_AddMember(getMostSignificantBits, UUID::getMostSignificantBits)
_CLASSREGISTER_AddMember(hashCode, UUID::hashCode)
_CLASSREGISTER_AddMember(node, UUID::node)
_CLASSREGISTER_AddStaticMember(randomUUID, UUID::randomUUID)
_CLASSREGISTER_AddMember(timestamp, UUID::timestamp)
_CLASSREGISTER_AddMember(toString, UUID::toString)
_CLASSREGISTER_AddMember(variant, UUID::variant)
_CLASSREGISTER_AddMember(version, UUID::version)
_END_REGISTER_CLASS()

// Register SecTimer
_BEGIN_REGISTER_CLASS(SecTimer)
_CLASSREGISTER_AddStaticMember(createTimer, SecTimer::createTimer);
_CLASSREGISTER_AddStaticMember(startTimer, SecTimer::startTimer);
_CLASSREGISTER_AddStaticMember(stopTimer, SecTimer::stopTimer);
_END_REGISTER_CLASS()




