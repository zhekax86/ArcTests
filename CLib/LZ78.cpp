#include "Stdafx.h"
#include "LZ78.h"

/*CoderString::CoderString() : _length(0)
{
}

CoderString::CoderString(uchar c) : buf((uchar*)c),_length(1)
{
}

CoderString::~CoderString()
{
	if(_length > sizeof(buf))
		delete [] buf;
}

uchar CoderString::operator[](size_t index) const
{
	assert(index<_length);
	assert(_length>0);
	if(index < sizeof(buf))
		return ((uchar*)(&buf))[index];
	else
		return buf[index];
}

bool CoderString::operator<(CoderString const &s2) const
{
	size_t maxlen = max(_length,s2._length);
	for(size_t i=0;i<maxlen;i++)
		if( operator[](i) < s2[i])
			return true;
}

bool CoderString::operator==(const CoderString &s2) const
{
	return false;
}*/

LZ78StringTreeIndex::LZ78StringTreeIndex(LZ78StringTree *Root)
{
	Strings.push_back(Root);
	Strings.push_back(nullptr);
	Strings.push_back(nullptr);
}

uint LZ78StringTreeIndex::Insert(LZ78StringTree* elem)
{
	Strings.push_back(elem);
	return Strings.size() -1;
}

LZ78StringTree& LZ78StringTreeIndex::operator[](uint index)
{
	assert(index == 0 || index >= 3);
	return *(Strings[index]);
}

uint LZ78StringTreeIndex::Size()
{
	return Strings.size();
}

LZ78StringTree::LZ78StringTree() : StringNum(0), StringLength(0), parent(nullptr)
{
	//SubStrings.push_back( LZ78tn(string(),nullptr) );
	//SubStrings.insert( pair<uchar,LZ78StringTreeIndex>(
	index = new LZ78StringTreeIndex(this);
}

LZ78StringTree::LZ78StringTree(LZ78StringTreeIndex *ind, LZ78StringTree *p): index(ind), parent(p)
{
	StringNum = index->Insert(this);
	StringLength = parent->StringLength+1;
}

LZ78StringTree::~LZ78StringTree()
{
	if(StringNum == 0)
		delete index;
}

void LZ78StringTree::AddChar(uint strnum,uchar c)
{
	LZ78StringTree &node = (*index)[strnum];
	node.SubStrings.insert( pair<uchar,LZ78StringTree*>(c,new LZ78StringTree(index,&node)) );
	/*if(StringNum == node.StringNum)
	{
		LZ78tn s(string(1,c),nullptr);
		auto it = lower_bound(node.SubStrings.begin(),node.SubStrings.end(),s,[](const LZ78tn &l, const LZ78tn &r) { return l.first< r.first;} );
		node.SubStrings.insert(
	}
	else
		node.AddChar(strnum,c);*/
}

uint LZ78StringTree::FindLongestString(uchar *buf, size_t buflen)
{
	/*LZ78tn s(string(1,buf[0]),nullptr);
	auto it=lower_bound(SubStrings.begin(),SubStrings.end(),s,[](const LZ78tn &l,const LZ78tn &r) { return l.first < r.first;} );
	if(it != SubStrings.end() && it->first == s.first && buflen >1)
		return it->second->FindLongestString(buf+1,buflen-1);
	else 
		return StringNum;*/

	if(buflen > 1)
	{
		auto it = SubStrings.find(buf[0]);
		if(it != SubStrings.end())
		{
			uint res = it->second->FindLongestString(buf+1,buflen-1);
			//if(res != 0)
			return res;
		}
	}
	return StringNum;
}

uint LZ78StringTree::Length()
{
	return StringLength;
}

LZ78StringTree& LZ78StringTree::operator[](uint i)
{
	return index->operator[](i);
}

uint LZ78StringTree::NumBits()
{
	return NeedBits(index->Size()-1);
}

charbuf LZ78::Do(charbuf &source)
{
	size_t len = source.Length();
	uchar *buf = source.GetBuf();
	OutBitStream out(len/2);
	ofstream logfile("LZ78.log");

	size_t current=0, oldc;
	while(current < len)
	{
		uint cnum = Tree.FindLongestString(buf+current,len-current);
		oldc=current;
		current += Tree[cnum].Length();
		out.WriteBits(cnum, Tree.NumBits() );

		//if(current < len)
		//{
			Tree.AddChar(cnum,buf[current]);
			out.WriteBits(buf[current],8);
			
			logfile << '('<<cnum<<','<<buf[current]<<')';
			for(;oldc<current;oldc++)
				logfile<<buf[oldc];
			logfile<<endl;
			current++;
		//}
	}
	out.WriteBits(2,8);

	out.Flush();
	return move(out.GetCharbuf());
}

charbuf LZ78::UnDo(charbuf &source)
{
	return source;
}
