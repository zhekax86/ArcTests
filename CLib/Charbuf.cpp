#include "stdafx.h"
#include "Charbuf.h"

#include <filesystem>

namespace CLib::Base
{
	void charbuf::ExpandCapacity()
	{
		size_t newcap = _capacity + 65536;
		unsigned char* newbuf = new unsigned char[newcap];
		memcpy(newbuf, buf, _length);
		delete[] buf;
		buf = newbuf;
		_capacity = newcap;
	}

	charbuf::charbuf(int capacity) : _capacity(capacity), _length(0)
	{
		buf = new unsigned char[_capacity];
	}

	charbuf::charbuf(const std::wstring& file)
	{
		std::ifstream in(file, std::ios_base::binary);
		unsigned long long size = std::filesystem::file_size(std::filesystem::path(file));
		_capacity = _length = static_cast<size_t>(size);
		buf = new unsigned char[_capacity];
		in.read((char*)buf, _capacity);
	}

	charbuf::charbuf(const charbuf& src) : _capacity(src._capacity), _length(src._length)
	{
		buf = new unsigned char[_capacity];
		memcpy(buf, src.buf, _length);
	}

	charbuf::charbuf(charbuf&& src) noexcept
		: _capacity(src._capacity), _length(src._length), buf(src.buf)
	{
		src.buf = nullptr;
	}

	charbuf::charbuf(unsigned char* b, int len) : _capacity(len), _length(len), buf(b)
	{
		//buf = b;
	}

	charbuf::~charbuf()
	{
		if (buf != nullptr) delete[] buf;
	}

	charbuf& charbuf::operator=(charbuf&& src) noexcept
	{
		if (buf != src.buf)
		{
			if (buf != nullptr) delete[] buf;
			buf = src.buf;
			src.buf = nullptr;
			_length = src._length;
			_capacity = src._capacity;
		}
		return *this;
	}

	void charbuf::SaveTo(const std::wstring& file)
	{
		std::ofstream out(file, std::ios_base::binary);
		out.write((char*)buf, _length);
	}

	size_t charbuf::Length()
	{
		return _length;
	}

	unsigned char* charbuf::GetBuf()
	{
		return buf;
	}

	void charbuf::AppendChar(unsigned char c)
	{

		if (_length + 1 >= _capacity)
			ExpandCapacity();

		buf[_length++] = c;

	}

	void charbuf::AppendCopyedChars(size_t shift, size_t count)
	{
		while (_length + count > _capacity)
			ExpandCapacity();

		memcpy(buf + _length, buf + _length - shift, count);
		_length += count;
	}
}