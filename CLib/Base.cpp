#include "Stdafx.h"
#include "Base.h"
#include "BWT.h"
#include "ParallelBWT.h"
#include "Delta.h"
#include "RLE12.h"
#include "RLE.h"
#include "MTF.h"
#include "AdHuffman.h"
#include "Huffman.h"
#include "LZSS.h"
#include "LZSSRAW.h"
#include "LZSSAdHuf.h"
#include "Probability.h"
#include "LZB.h"
#include "LZ78.h"
#include "Arythmetic.h"

void charbuf::ExpandCapacity()
{
	size_t newcap = _capacity + 65536;
	unsigned char *newbuf = new unsigned char [newcap];
	memcpy(newbuf,buf,_length);
	delete [] buf;
	buf = newbuf;
	_capacity = newcap;
}

charbuf::charbuf(int capacity): _capacity(capacity),_length(0)
{
	buf = new unsigned char [_capacity];
}

charbuf::charbuf(const wstring &file)
{
	ifstream in(file,ios_base::binary);
	unsigned long long size = tr2::sys::file_size( tr2::sys::wpath(file) );
	_capacity = _length = static_cast<size_t>(size);
	buf = new unsigned char [_capacity];
	in.read((char *)buf,_capacity);
}

charbuf::charbuf(const charbuf &src): _capacity(src._capacity), _length(src._length)
{
	//_capacity = src._capacity;
	//_length = src._length;
	buf = new unsigned char [_capacity];
	memcpy(buf,src.buf,_length);
}

charbuf::charbuf(charbuf &&src): _capacity(src._capacity), _length(src._length), buf(src.buf)
{
	//_capacity = src._capacity;
	//_length = src._length;
	//buf = src.buf;
	src.buf = nullptr;
}

charbuf::charbuf(unsigned char *b, int len): _capacity(len),_length(len), buf(b)
{
	//buf = b;
}

charbuf::~charbuf()
{
	if(buf != nullptr) delete [] buf;
}

charbuf& charbuf::operator=(charbuf &&src)
{
	if( buf != src.buf)
	{
		if(buf != nullptr) delete [] buf;
		buf = src.buf;
		src.buf = nullptr;
		_length = src._length;
		_capacity = src._capacity;
	}
	return *this;
}

void charbuf::SaveTo(const wstring &file)
{
	ofstream out(file,ios_base::binary);
	out.write((char*)buf,_length);
}

size_t charbuf::Length()
{
	return _length;
}

unsigned char * charbuf::GetBuf()
{
	return buf;
}

void charbuf::AppendChar(unsigned char c)
{

	if(_length +1 >=_capacity)
		ExpandCapacity();

	buf[_length++] = c;

}

void charbuf::AppendCopyedChars(size_t shift, size_t count)
{
	while( _length + count > _capacity)
		ExpandCapacity();

	memcpy( buf+_length, buf + _length - shift, count);
	_length += count;
}

OutBitStream::OutBitStream(): buf(65535), bitsleft(8), curbyte(0)
{
}

OutBitStream::OutBitStream(int bufsize): buf(bufsize), bitsleft(8), curbyte(0)
{
}

void OutBitStream::WriteBits(unsigned int b, unsigned int count)
{
	int btw = count;
	unsigned int bl=b;
	while(btw > 0)
	{
		unsigned int tb = bl;
		int bw;
		if(btw > bitsleft)
		{
			tb >>= btw-bitsleft;
			bw = bitsleft;
		}
		else
		{
			bw = btw;
			tb <<= bitsleft - bw;
		}

		curbyte |= tb;
		bitsleft -= bw;
		btw -= bw;
		bl <<= 32 - btw;
		bl >>= 32 - btw;
		if(bitsleft == 0)
		{
			buf.AppendChar(curbyte);
			curbyte = 0;
			bitsleft = 8;
		}
	}
}

void OutBitStream::Flush()
{
	if(bitsleft <8)
		buf.AppendChar(curbyte);
}

charbuf& OutBitStream::GetCharbuf()
{
	return buf;
}

void OutBitStream::WriteEliaseCode(uint num)
{
	uint nb=NeedBits(num);
	if(nb>1)
		WriteBits(0,nb-1);
	WriteBits(num,nb);
}

BitReader::BitReader(unsigned char *c, size_t len) : buf(c), buf_len(len), cur_char(0), cur_bit(8), bytes_after_end(0)
{
}

BitReader::BitReader(unsigned char *c, size_t len, unsigned int after_end) : buf(c), buf_len(len), cur_char(0), cur_bit(8), bytes_after_end(after_end)
{
}

unsigned int BitReader::GetBits(unsigned int count)
{
	unsigned int result=0, cur_count = count;
	while(cur_count >0)
	{
		unsigned int tempc = cur_count > cur_bit? cur_bit: cur_count;
		unsigned char c = buf[cur_char] << (8-cur_bit);
		unsigned int t = c>>(8-tempc);
		cur_bit -= tempc;
		if(cur_bit == 0)
		{
			cur_char++;
			cur_bit = 8;
		}
		result |= t << (cur_count-tempc);

		cur_count -= tempc;
	}

	return result;
}

unsigned char BitReader::GetBit()
{
	assert(cur_char < buf_len + bytes_after_end);
	unsigned char res;

	if(cur_char < buf_len)
		res = (buf[cur_char] >> (cur_bit-1)) & 0x01;
	else
		res = 0;

	cur_bit--;
	if(cur_bit == 0)
	{
		cur_char ++;
		cur_bit = 8;
	}
	return res;
}

uint BitReader::ReadEliaseCode()
{
	uint count=0,bit = GetBit();
	while(bit == 0)
	{
		bit = GetBit();
		count++;
	}
	uint num=0;
	if(count>0)
		num = GetBits(count);
	num = (bit << count)+num;
	return num;
}

//Говорит сколько бит нужно для кодирования такого числа
int NeedBits(unsigned int num)
{
	unsigned int b=1, n=1;
	while(num>=b)
	{
		b<<=1;
		n++;
	}
	return n-1;
}

void _Compress(const wstring &infile, const wstring &outname,const vector<int> &actions, bool Dump)
{
	charbuf buf(infile);
	unique_ptr<Act> action;

	for(size_t i =0; i< actions.size();++i)
		if(actions[i] != 0)
		{
			action = GetAction(actions[i]);
			buf = action->Do(buf);
			if(Dump)
			{
				wostringstream dumpname;
				dumpname << infile << L'.' << i << L"_dump";
				buf.SaveTo( dumpname.str() );
			}
		}

	buf.SaveTo(outname);
}

void _Decompress(const wstring &infile, const wstring &outname,const vector<int> &actions, bool Dump)
{
	charbuf buf(infile);
	unique_ptr<Act> action;

	for(int i = actions.size()-1; i>=0 ;--i)
		if(actions[i] != 0)
		{
			action = GetAction(actions[i]);
			buf = action->UnDo(buf);
			if(Dump)
			{
				wostringstream dumpname;
				dumpname << infile << L'.' << i << L"_dump";
				buf.SaveTo( dumpname.str() );
			}
		}

	buf.SaveTo(outname);
}

unique_ptr<Act> GetAction(int actnum)
{
	switch(actnum)
	{
		case 1:	return unique_ptr<Act>(new BWT());
		case 2:	return unique_ptr<Act>(new ParallelBWT());
		case 3: return unique_ptr<Act>(new Delta());
		case 4: return unique_ptr<Act>(new RLE12());
		case 5: return unique_ptr<Act>(new RLE());
		case 6: return unique_ptr<Act>(new MTF());
		case 7: return unique_ptr<Act>(new Huffman());
		case 8: return unique_ptr<Act>(new AdHuffman());
		case 9: return unique_ptr<Act>(new LZSS());
		case 10: return unique_ptr<Act>(new LZSSRAW());
		case 11: return unique_ptr<Act>(new LZSSAdHuf());
		case 12: return unique_ptr<Act>(new Probability());
		case 13: return unique_ptr<Act>(new LZB());
		case 14: return unique_ptr<Act>(new LZ78());
		case 15: return unique_ptr<Act>(new Arythmetic());
	};
	return unique_ptr<Act>(new Act());
}

void GetActions(vector<wstring> &list)
{
	list.push_back(L"Ничего");
	list.push_back(L"Barrows Wheeler Transform");
	list.push_back(L"Barrows Wheeler Transform (parallel)");
	list.push_back(L"Delta");
	list.push_back(L"RLE 1+2 bytes");
	list.push_back(L"RLE");
	list.push_back(L"Move To Front");
	list.push_back(L"Huffman");
	list.push_back(L"Adaptive Huffman");
	list.push_back(L"LZSS");
	list.push_back(L"LZSS + RAW");
	list.push_back(L"LZSS + Adaptive Huffman for single chars");
	list.push_back(L"Вероятностное сжатие");
	list.push_back(L"LZB");
	list.push_back(L"LZ78");
	list.push_back(L"Адаптивное арифметическое кодирование");
}


charbuf Act::Do(charbuf &source)
{
	return move(source);
	//return unique_ptr<charbuf>(new charbuf(*source));
}

charbuf Act::UnDo(charbuf &source)
{
	return move(source);
	//return unique_ptr<charbuf>(new charbuf(*source));
}