#include "Stdafx.h"
#include "LZB.h"

using namespace CLib::Base;
using namespace std;

void LZB::AddInChain(unsigned int c, size_t pos)
{
	int newpos = pos % DICTLEN;
	if( FirstEntry[c] == -1)
		FirstEntry[c] = pos;
	else
	{
		int old_pos = FirstEntry[c], shift = pos-old_pos;
		FirstEntry[c] = pos;
		Chain[newpos] = shift;
		ForwardChain[newpos-shift >= 0? newpos-shift: newpos - shift + DICTLEN] = shift;
	}
}

void LZB::DeleteLastFromChain(unsigned int c, size_t pos)
{
	int newpos = pos % DICTLEN;
	int shift = ForwardChain[newpos];
	if (shift == 0)
		FirstEntry[c] = -1;
	else
	{
		ForwardChain[newpos] = 0;
		Chain[newpos + shift < DICTLEN ?newpos+shift:newpos+shift-DICTLEN] = 0;
	}
}

void LZB::CalcCodes4Tree(map<uint,uint> &stat)
{
	HTreeNode newnode;
	newnode.left = -1;
	newnode.right = -1;
	newnode.parent = -1;
	newnode.count = 0;

	tree.reserve(512);
	for(auto s:stat)
	{
		newnode.c = s.first;
		newnode.count = s.second;
		tree.push_back(newnode);
	}
	/*for(size_t i=0;i<256;i++)
	{
		newnode.c = i;
		tree.push_back(newnode);
		codelen[i] = 0;
	}
	for(size_t i=0; i< len; i++)
		tree[buf[i]].count++;*/

	tree.erase( remove_if(tree.begin(),tree.end(), [](const HTreeNode &n) { return n.count == 0;} ), tree.end());
	sort(tree.begin(),tree.end(), [](const HTreeNode &l,const HTreeNode &r) { return l.count < r.count; } );
	
	for(size_t i=0; i+1 < tree.size() ; i+=2)
	{
		newnode.left = i;
		newnode.right = i+1;
		newnode.count = tree[i].count + tree[i+1].count;
		newnode.c = 0;

		size_t nni = i +2;
		while( nni < tree.size() )
		{
			if( tree[nni].count >= newnode.count+1)
				break;
			nni++;
		}

		tree[i].parent = nni;
		tree[i+1].parent = nni;
		tree.insert( tree.begin() + nni,newnode);
	}

	

	for(size_t i=0; i< tree.size(); i++)
	{
		if(tree[i].left == -1 && tree[i].right == -1)
		{
			unsigned char cl =0;
			unsigned int cc = 0;
			int inode = i;

			while(tree[inode].parent != -1)
			{
				if(tree[tree[inode].parent].right == inode)
					cc |= 1<<cl;
				cl++;
				inode = tree[inode].parent;
			}

			assert(cl <= 32);

			auto cit = treecodes.insert( pair<uint,pair<int,int>>(tree[i].c,pair<int,int>(cc,cl)));
			/*codes[tree[i].c] = cc;
			codelen[tree[i].c] = cl;*/
		}
	}
}

charbuf LZB::Do(charbuf &source)
{
	size_t len = source.Length();
	const unsigned char *buf = source.GetBuf();
	OutBitStream out(len/2);

	//статистика по длинне повторов
	map<uint,uint> stat;
	ofstream statlogfile("LZB_stat.log");

	for(size_t i=0;i<65536;i++)
		FirstEntry[i] = -1;
	for(size_t i=0;i<DICTLEN;i++)
		Chain[i] = ForwardChain[i]=0;

	out.WriteBits(buf[0],8);
	if(len>=2)
		AddInChain(buf[0]+(buf[1]<<8),0);
	//out.WriteBits(buf[1],8);
	size_t current=1;
	ofstream logfile("LZB.log");
	while(current<len)
	{
		int start = current+1 < len? FirstEntry[buf[current] + (buf[current+1]<<8)] : -1;
		int moffset=0, mlen=0;
		uint offb = NeedBits(current<DICTLEN? current : DICTLEN), lenb=0;
		//Если длина смещения назад будет 6 бит или меньше (63), то есть шанс закодировать один символ
		//парой (смещение,1) так что длинна этой пары будет короче самого символа!
		//но для этого нужен другой алгоритм поиска совпадания, т.к. обычный ищет совпадения от 2-х символов
		//второй алгоритм будет простым перебором
		if(current <=63)
		{
			int i;
			for(start = 0; start < current; start++)
			{
				for(i=0; start+i< current && current+i < len && buf[start+i] == buf[current+i]; i++) ;
				if(i>mlen)
				{
					moffset = current-start;
					mlen = i;
				}
			}
		}
		else
			while( start >=0 )
			{
				int i;			
				{
					//Обычный поиск, ищет совпадания от 2-х символов
					for(i=0; start+i< current && current+i < len && buf[start+i] == buf[current+i]; i++) ;
					if(i>mlen)
					{
						moffset = current-start;
						mlen = i;
					}
					if(Chain[start%DICTLEN] != 0)
						start -= Chain[ start%DICTLEN ];
					else
						start = -1;
				}
			}

		if(mlen>0)
			lenb=(NeedBits(mlen)*2)-1;

		//если пара будет короче, чем все совпадения одиночными символами (при условии что совпадение есть)
		if(mlen>0 && offb+lenb+1 < mlen*9)
		{
			out.WriteBits(1,1);
			out.WriteBits(moffset,offb);
			out.WriteEliaseCode(mlen);
			logfile<<current<<':'<<'('<<moffset<<'/'<<offb<<','<<mlen<<')'<<':';
			for(int i=0;i<mlen;i++)
				logfile<<buf[current-moffset+i];
			logfile << endl;

			//обновляем статистику
			auto sit = stat.find(mlen);
			if(sit != stat.end())
				sit->second++;
			else
				stat.insert( pair<uint,uint>(mlen,1));
		}
		else
		{
			mlen=1;
			out.WriteBits(0,1);
			out.WriteBits(buf[current],8);
			logfile<<current<<':'<<buf[current]<<endl;
		}

		while(mlen--)
		{
			if(current >= DICTLEN)
				DeleteLastFromChain(buf[current-DICTLEN]+(buf[current+1-DICTLEN]<<8),current-DICTLEN);
			if(current+1 < len)
				AddInChain(buf[current]+(buf[current+1]<<8),current);
			current++;
		}
	}

	out.WriteBits(1,1);
	out.WriteBits(0,NeedBits(current));
	out.Flush();

	//подсчет и вывод статистики
	CalcCodes4Tree(stat);
	for(auto s : stat)
	{
		auto tit = treecodes.find(s.first);
		statlogfile<<s.first<<' '<<s.second<<' '<<((NeedBits(s.first)*2) -1);
		statlogfile<<' '<<tit->second.second<<endl;
	}

	return move(out.GetCharbuf());
}

charbuf LZB::UnDo(charbuf &source)
{
	BitReader input(source.GetBuf(),source.Length());
	charbuf newbuf(source.Length() * 2);

	newbuf.AppendChar(input.GetBits(8));
	size_t current = 1;
	while(true)
	{
		uint b = input.GetBit();
		if(b)
		{
			uint offset = input.GetBits( NeedBits(current<DICTLEN? current : DICTLEN) );
			if(offset == 0)
				break;
			uint count = input.ReadEliaseCode();
			newbuf.AppendCopyedChars(offset,count);
			current += count;
		}
		else
		{
			newbuf.AppendChar(input.GetBits(8));
			current++;
		}
	}

	return move(newbuf);
}