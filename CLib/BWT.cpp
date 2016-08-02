#include "Stdafx.h"
#include "BWT.h"

//Преобразование Барроуза Уилера
// https://ru.wikipedia.org/wiki/%D0%9F%D1%80%D0%B5%D0%BE%D0%B1%D1%80%D0%B0%D0%B7%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D0%B5_%D0%91%D0%B0%D1%80%D1%80%D0%BE%D1%83%D0%B7%D0%B0_%E2%80%94_%D0%A3%D0%B8%D0%BB%D0%B5%D1%80%D0%B0
// использует оптимизации "на хранить все переставленные строки" и "не хранить отдельным кодом символ конец файла"


charbuf BWT::EncodeBuf(charbuf &source, vector<int> &SortedStrings)
{
	size_t _len = source.Length()+1;
	const unsigned char *_buf = source.GetBuf();

	//Пишем в файл отсортированный массив (для отладки)
	//ofstream log("BWT_sortedenc.log");
	//ofstream log2("BWT_sortstrings.log");
	//for(auto it=SortedStrings.begin(); it != SortedStrings.end(); ++it)
	{
		//log<< *it <<endl;
		/*size_t off = *it;
		for(size_t i=0;i<len;i++)
		{
			if(off == len-1)
				log2<<'^';
			else
				log2<<buf[off];
			off++;
			if(off>=len)
				off-=len;
		}
		log2<<endl;*/
	}

	unsigned char *nb = new unsigned char[_len+3];
	int nbp = 4;
	for(size_t i=0;i<_len;i++)
	{
		size_t off = SortedStrings[i] + _len-1;
		if (off > _len-1) off -= _len;
		if(off != _len-1)
			nb[nbp++] = _buf[off];
		else
			*((int *)nb) = (int)i;
	}

	return charbuf(nb,_len+3);
}

charbuf BWT::DecodeBuf(charbuf &source, vector<int> &Order)
{
	int len = source.Length() -3;
	const unsigned char *buf = source.GetBuf();
	int eof = *((int*)buf);
	buf+=4;

	vector<int> moves(len);
	for(int i=0;i<len;i++)
		moves[Order[i]]=i;

	unsigned char *newbuf = new unsigned char [len-1];
	int next = moves[eof];
	for(int i=len-2;i>=0;i--)
	{
		newbuf[i] = buf[next>=eof?next-1:next];
		next = moves[next];
	}

	return charbuf(newbuf,len-1);
}

charbuf BWT::Do(charbuf &source)
{
	vector<int> strings;
	int len = source.Length()+1;
	const unsigned char *buf = source.GetBuf();

	//в массиве хранятся номера перестановок
	//номер означает смещение строки относительно начала
	strings.reserve(len);
	for(int i=0;i<len;i++)
		strings.push_back(i);

	sort(strings.begin(),strings.end(), [&buf,&len](int x, int y)
	{
		if(x==y)
			return false;
		for(int i = 0;i<len;i++)
		{
			int xo = x+i,yo=y+i;
			if(xo > len) xo -= len;	//текущий символ строки выходит за границу буфера, то "заворачиваем" его на начало
			if(yo > len) yo -= len;

				return false;
			if(yo == len-1)
				return true;
			if(buf[xo] != buf[yo])
				return buf[xo]< buf[yo];

		}
		return false;
	});

	//charbuf newbuf(len+3);
	return move(EncodeBuf(source,strings));
}

charbuf BWT::UnDo(charbuf &source)
{
	int len = source.Length() -3;
	const unsigned char *buf = source.GetBuf();
	int eof = *((int*)buf);
	buf+=4;

	vector<int> order;
	order.reserve(len);
	for(int i=0;i<len;i++)
		order.push_back(i);

	sort(order.begin(),order.end(),[&eof,&buf](int x,int y)
	{
		if(x == eof)
			return false;
		if(y == eof)
			return true;
		if(x>eof) x--;
		if(y>eof) y--;
		if(buf[x] == buf[y])
			return x<y;
		return buf[x]<buf[y];
	});

	//return unique_ptr<charbuf>(new charbuf(*source));
	return DecodeBuf(source,order);
}