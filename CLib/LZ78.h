#pragma once

#include "Stdafx.h"
#include "Base.h"
#include <utility>

//На данный момент этот алгоритм "заброшен", все что тут написано - это черновые наброски

//"строка" для нужд кодировщика, может содержать нулевые символы
/*class CoderString
{
private:
	uchar *buf;
	size_t _length;
public:
	CoderString();
	CoderString(uchar c);
	~CoderString();
	uchar operator[](size_t index) const;
	bool operator<(const CoderString &s2) const;
	bool operator==(const CoderString &s2) const;
};*/

class LZ78StringTree;
class LZ78StringTree;
typedef std::pair<std::string, LZ78StringTree*> LZ78tn;

class LZ78StringTreeIndex
{
private:
	std::vector<LZ78StringTree*> Strings;
public:
	LZ78StringTreeIndex(LZ78StringTree *Root);
	uint Insert(LZ78StringTree* elem);
	LZ78StringTree& operator[](uint index);
	uint Size();
};

class LZ78StringTree
{
private:
	std::map<uchar,LZ78StringTree*> SubStrings;
	LZ78StringTree *parent;
	LZ78StringTreeIndex *index;
	std::string str;
	uint StringNum;	//0 - пустая строка(по умолчанию), 1 - reset, 2 - eof, 3 и выше - доступные строки
	uint StringLength;

	LZ78StringTree(LZ78StringTreeIndex *ind, LZ78StringTree *p);
public:
	LZ78StringTree();
	~LZ78StringTree();

	void AddChar(uint strnum,uchar c);
	uint FindLongestString(uchar *buf, size_t buflen);
	uint Length();
	LZ78StringTree& operator[](uint i);
	uint NumBits();
};

class LZ78 : public CLib::Base::Act
{
private:
	LZ78StringTree Tree;
public:
	virtual CLib::Base::charbuf Do(CLib::Base::charbuf &source) override;
	virtual CLib::Base::charbuf UnDo(CLib::Base::charbuf &source) override;
};