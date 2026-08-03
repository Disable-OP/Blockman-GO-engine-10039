
#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include "Core.h"

#if (LORD_PLATFORM == LORD_PLATFORM_LINUX)
#include <arpa/inet.h>
#define htonll(x) ((1==htonl(1)) ? (x) : ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define ntohll(x) ((1==ntohl(1)) ? (x) : ((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))
#else
#include <WinSock2.h>
#endif

using namespace vcs;

Buffer::Buffer(size_t capacity) : _size(0), _capacity(capacity)
{
	_data_beg_ptr = (char*)malloc(_capacity);
	_data_end_ptr = _data_beg_ptr + _capacity;
	_cursor_beg_ptr = _data_beg_ptr;
	_cursor_end_ptr = _cursor_beg_ptr;
}

Buffer::~Buffer()
{
	free(_data_beg_ptr);
}

size_t Buffer::shrink(size_t count)
{
	if(count == 0) return 0;
	if(count > size())
		count = size();

	if(count < (size_t)(_data_end_ptr - _cursor_beg_ptr)){
		_cursor_beg_ptr += count;
		if(_cursor_beg_ptr == _data_end_ptr)
			_cursor_beg_ptr = _data_beg_ptr;
	}
	else{
		size_t n_items_1 = _data_end_ptr - _cursor_beg_ptr;
		size_t n_items_2 = count - n_items_1;
		_cursor_beg_ptr = _data_beg_ptr + n_items_2;
	}
	_size -= count;
	return count;
}

size_t Buffer::read(const char* data, size_t count)
{
	if(count == 0) return 0;
	if(count > size())
		count = size();

	if(count < (size_t)(_data_end_ptr - _cursor_beg_ptr)){
		memcpy((void *)data, _cursor_beg_ptr, count);
		_cursor_beg_ptr += count;
		if(_cursor_beg_ptr == _data_end_ptr)
			_cursor_beg_ptr = _data_beg_ptr;
	}
	else{
		size_t n_items_1 = _data_end_ptr - _cursor_beg_ptr;
		memcpy((void *)data, _cursor_beg_ptr, n_items_1);

		size_t n_items_2 = count - n_items_1;
		memcpy((void *)(data + n_items_1), _data_beg_ptr, n_items_2);

		_cursor_beg_ptr = _data_beg_ptr + n_items_2;
	}
	_size -= count;
	return count;
}

size_t Buffer::peek(char* data, size_t count) const
{
	if(count == 0) return 0;
	if(count > size())
		count = size();

	if(count < (size_t)(_data_end_ptr - _cursor_beg_ptr)){
		memcpy(data, _cursor_beg_ptr, count);
	}
	else{
		size_t n_items_1 = _data_end_ptr - _cursor_beg_ptr;
		memcpy(data, _cursor_beg_ptr, n_items_1);

		size_t n_items_2 = count - n_items_1;
		memcpy(data + n_items_1, _data_beg_ptr, n_items_2);
	}
	return count;
}

void Buffer::write(const char* data, size_t count)
{
	if(count == 0) return;
	while(count > (capacity() - size()))
		this->_inflate();

	if(_cursor_end_ptr + count <= _data_end_ptr){
		memcpy(_cursor_end_ptr, data, count);
		_cursor_end_ptr += count;
		if (_cursor_end_ptr == _data_end_ptr)
			_cursor_end_ptr = _data_beg_ptr;
	}
	else{
		size_t n_items_1 = _data_end_ptr - _cursor_end_ptr;
		memcpy(_cursor_end_ptr, data, n_items_1);

		size_t n_items_2 = count - n_items_1;
		memcpy(_data_beg_ptr, data + n_items_1, n_items_2);

		_cursor_end_ptr = _data_beg_ptr + n_items_2;
	}
	_size += count;
	return;
}

void Buffer::append(const Buffer* buffer)
{
	size_t count = buffer->size();
	while(count > (capacity() - size()))
		this->_inflate();

	if(count < (size_t)(buffer->_data_end_ptr - buffer->_cursor_beg_ptr)){
		this->write(buffer->_cursor_beg_ptr, count);
	}
	else{
		size_t n_items_1 = buffer->_data_end_ptr - buffer->_cursor_beg_ptr;
		this->write(buffer->_cursor_beg_ptr, n_items_1);

		size_t n_items_2 = count - n_items_1;
		this->write(buffer->_data_beg_ptr, n_items_2);
	}
	return;
}

void Buffer::shrinkByte()
{
	this->shrink(sizeof(char));
}

char Buffer::peekByte() const
{
	char val = 0;
	this->peek((char *)&val, sizeof(val));
	return val;
}

char Buffer::readByte()
{
	char val = 0;
	this->read((char *)&val, sizeof(val));
	return val;
}

void Buffer::writeByte(const char data)
{
	char val = data;
	this->write((char *)&val, sizeof(val));
}

void Buffer::shrinkUnsignedByte()
{
	this->shrink(sizeof(unsigned char));
}

unsigned char Buffer::peekUnsignedByte() const
{
	unsigned char val;
	this->peek((char *)&val, sizeof(val));
	return val;
}

unsigned char Buffer::readUnsignedByte()
{
	unsigned char val;
	this->read((char *)&val, sizeof(val));
	return val;
}

void Buffer::writeUnsignedByte(const unsigned char data)
{
	unsigned char val = data;
	this->write((char *)&val, sizeof(val));
}

void Buffer::shrinkShort()
{
	this->shrink(sizeof(short));
}

short Buffer::peekShort() const
{
	short val;
	this->peek((char *)&val, sizeof(val));
	return ntohs(val);
}

short Buffer::readShort()
{
	short val;
	this->read((char *)&val, sizeof(val));
	return ntohs(val);
}

void Buffer::writeShort(const short data)
{
	short val = htons(data);
	this->write((char *)&val, sizeof(val));
}

void Buffer::shrinkUnsignedShort()
{
	this->shrink(sizeof(unsigned short));
}

unsigned short Buffer::peekUnsignedShort() const
{
	unsigned short val;
	this->peek((char *)&val, sizeof(val));
	return ntohs(val);
}

unsigned short Buffer::readUnsignedShort()
{
	unsigned short val;
	this->read((char *)&val, sizeof(val));
	return ntohs(val);
}

void Buffer::writeUnsignedShort(const unsigned short data)
{
	unsigned short val = htons(data);
	this->write((char *)&val, sizeof(val));
}

void Buffer::shrinkInt()
{
	this->shrink(sizeof(int32_t));
}

int32_t Buffer::peekInt() const
{
	int32_t val;
	this->peek((char *)&val, sizeof(val));
	return ntohl(val);
}

int32_t Buffer::readInt()
{
	int32_t val;
	this->read((char *)&val, sizeof(val));
	return ntohl(val);
}

void Buffer::writeInt(const int32_t data)
{
	int val = htonl(data);
	this->write((char *)&val, sizeof(val));
}

void Buffer::shrinkUnsignedInt()
{
	this->shrink(sizeof(uint32_t));
}

uint32_t Buffer::peekUnsignedInt() const
{
	uint32_t val;
	this->peek((char *)&val, sizeof(val));
	return ntohl(val);
}

uint32_t Buffer::readUnsignedInt()
{
	uint32_t val;
	this->read((char *)&val, sizeof(val));
	return ntohl(val);
}

void Buffer::writeUnsignedInt(const uint32_t data)
{
	unsigned int val = htonl(data);
	this->write((char *)&val, sizeof(val));
}

void Buffer::shrinkInt64()
{
	this->shrink(sizeof(int64_t));
}

int64_t Buffer::peekInt64() const 
{
	int64_t val;
	this->peek((char *)&val, sizeof(val));
	return ntohll(val);
}

int64_t Buffer::readInt64() {
    int64_t val;
    this->read((char *)&val, sizeof(val));
    return ntohll(val);
}

void Buffer::writeInt64(const int64_t data) {
	int64_t val = htonll(data);
    this->write((char *)&val, sizeof(val));
}

///////////////////////////////////////////////

void Buffer::_inflate()
{
	size_t newCapacity = capacity() * 2;
	size_t cursor_beg_ofset = _cursor_beg_ptr - _data_beg_ptr;

	size_t sizeToMove = 0;
	if(size() > (size_t)(_data_end_ptr - _cursor_beg_ptr)){
		sizeToMove = size() - (_data_end_ptr - _cursor_beg_ptr);
	}

	_data_beg_ptr = (char*)realloc(_data_beg_ptr, newCapacity);
	if (!_data_beg_ptr) {
		std::abort();
	}
	_data_end_ptr = _data_beg_ptr + newCapacity;
	_cursor_beg_ptr = _data_beg_ptr + cursor_beg_ofset;
	_cursor_end_ptr = _cursor_beg_ptr + size();
	_capacity = newCapacity;

	if(sizeToMove > 0)
		memcpy(_cursor_beg_ptr + (size() - sizeToMove), _data_beg_ptr, sizeToMove);
}
