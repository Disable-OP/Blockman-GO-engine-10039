#ifndef __NBT_BINDING_MACROS_HEADER__
#define __NBT_BINDING_MACROS_HEADER__

#include "Nbt/NbtBinding.h"
#include "Nbt/NbtTagReader.h"
#include "Nbt/NbtTagWriter.h"

#define NBTB_BEGIN(_className, _baseClass, ...) \
template<> \
class NbtBinding<_className> : public NbtBindingBase<_className, _baseClass> \
{ \
	using BindedClass = _className; \
	template<typename T, typename U> \
	friend class BLOCKMAN::NbtBindingBase; \
private: \
	NbtBinding() {

#define NBTB_END }};

#define NBTB_CUSTOM_HANDLERS(_tagName, _reader, _writer) registerCustomHandler(_tagName, _reader, _writer);
#define NBTB_DECLARE_FRIEND(_className) friend class NbtBinding<_className>;
#define NBTB_ALIAS(_propertyName, _tagName, _tagOperator, ...) \
registerBinding<_tagOperator::Reader, _tagOperator::Writer>(&BindedClass::_propertyName, _tagName, ##__VA_ARGS__);

#define NBTB_CONVERTERS(_assignOp, _convertOp) std::make_pair(_assignOp, _convertOp)
#define NBTB_HALF(_tagType) TagOperators<HalfNbtTagReader<NbtTagType::_tagType>, HalfNbtTagWriter<NbtTagType::_tagType>>
#define NBTB_SIMPLE(_tagType) TagOperators<SimpleNbtTagReader<NbtTagType::_tagType>, SimpleNbtTagWriter<NbtTagType::_tagType>>
#define NBTB_LIST(_tagOperator) TagOperators<ListNbtTagReader<typename _tagOperator::Reader>, ListNbtTagWriter<typename _tagOperator::Writer>>
#define NBTB_COMPOUND(_className) TagOperators<CompoundNbtTagReader<_className>, CompoundNbtTagWriter<_className>>
#define NBTB_PROPERTY(_name, _tagOperator, ...) NBTB_ALIAS(_name, #_name, _tagOperator, ##__VA_ARGS__)


#endif // !__NBT_BINDING_MACROS_HEADER__

