#pragma once
#ifdef __GNUC__
#ifndef min
#define min(x,y) (x) < (y) ? (x) : (y)
#endif
#endif
class RecvBuffer
{
public:
	RecvBuffer() : _data(nullptr), _capacity(0), _size(0)
	{

	}

	~RecvBuffer()
	{
		if (_data)
		{
			free(_data);
			_data = nullptr;
		}
	}

	void Reset()
	{
		_size = 0;
	}

	size_t Size() const
	{
		return _size;
	}

	size_t Read(char* s, size_t size)
	{
		size_t realSize = min(_size, size);
		memcpy(s, _data, realSize);
		_size -= realSize;

		if (_size > 0)
			memmove(_data, _data + realSize, _size);

		return realSize;
	}

	size_t Write(const char* s, size_t size)
	{
		size_t total = _size + size;
		if (total > _capacity)
			Grow(size);
		assert(_size + size <= _capacity);
		memcpy(_data + _size, s, size);
		_size += size;
		return size;
	}

	size_t Peek(char* s, size_t size) const
	{
		size_t realSize = min(_size, size);
		memcpy(s, _data, realSize);
		return realSize;
	}

	size_t Advance(size_t size)
	{
		size_t realSize = min(_size, size);
		_size -= realSize;
		if (_size > 0)
			memmove(_data, _data + realSize, _size);

		return realSize;
	}

	char* Data() const { return _data; }
	int Search(const char* s, size_t size) const
	{
		for (size_t i = 0; i < _size; i++)
		{
			size_t j = 0;
			for (; j < size; j++)
				if (_data[i + j] != s[j])
					break;
			if (j == size)
				return (int)i;
		}
		return -1;
	}
private:

	void Grow(size_t size)
	{
		size_t total = _size + size;
		if (total < _capacity * 2)
			total = _capacity * 2;
		total = (total + 1024 * 64 - 1) / (1024 * 64) * (1024 * 64);

		char* p = (char*)malloc(total);
		assert(p != nullptr);
		if (_size > 0)
			memmove(p, _data, _size);

		if (_data)
			free(_data);
		_data = p;
		_capacity = total;
	}

private:
	//char* _head, * _tail;
	char* _data;
	size_t _capacity;
	size_t _size;
};