#include "Stdafx.h"
#include "Huffman.h"

using namespace CLib::Base;
using namespace std;

struct CCode
{
	unsigned int code;
	unsigned char c;
};

class SymTree
{
private:
	unsigned char ch;
	SymTree *left,*right;

	SymTree(unsigned char c);
public:
	SymTree();
	~SymTree();

	void InsertCode(unsigned int code, unsigned char codelen, unsigned char c);
	unsigned char DecodeSymbol(BitReader &input);
};

SymTree::SymTree(): left(nullptr), right(nullptr), ch(0)
{
}

SymTree::SymTree(unsigned char c) :left(nullptr), right(nullptr), ch(c)
{
}

SymTree::~SymTree()
{
	if(left != nullptr)
		delete left;
	if(right != nullptr)
		delete right;
}

void SymTree::InsertCode(unsigned int code, unsigned char codelen, unsigned char c)
{
	unsigned int b = (code >> (codelen-1)) & 1;
	if(codelen > 1)
	{
		if(b)
		{
			if(right == nullptr)
				right = new SymTree();
			right->InsertCode(code,codelen-1,c);
		}
		else
		{
			if(left == nullptr)
				left = new SymTree();
			left->InsertCode(code,codelen-1,c);
		}
	}
	else
	{
		if(b)
		{
			assert(right == nullptr);
			right = new SymTree(c);
		}
		else
		{
			assert(left == nullptr);
			left = new SymTree(c);
		}
	}
}

unsigned char SymTree::DecodeSymbol(BitReader &input)
{
	unsigned int b = input.GetBit();
	if(b)
	{
		assert(right != nullptr);
		if(right->right == nullptr && right->left == nullptr)
			return right->ch;
		else
			return right->DecodeSymbol(input);
	}
	else
	{
		assert(left != nullptr);
		if(left->right == nullptr && left->left == nullptr)
			return left->ch;
		else
			return left->DecodeSymbol(input);
	}
}

void Huffman::MakeCodes(charbuf &source)
{

	size_t len = source.Length();
	const unsigned char *buf = source.GetBuf();

	HTreeNode newnode;
	newnode.left = -1;
	newnode.right = -1;
	newnode.parent = -1;
	newnode.count = 0;

	tree.reserve(512);
	for(size_t i=0;i<256;i++)
	{
		newnode.c = i;
		tree.push_back(newnode);
		codelen[i] = 0;
	}
	for(size_t i=0; i< len; i++)
		tree[buf[i]].count++;

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

			codes[tree[i].c] = cc;
			codelen[tree[i].c] = cl;
		}
	}

}

charbuf Huffman::Do(charbuf &source)
{
	//unsigned int freq[256];
	//vector<FR> freq;
	size_t len = source.Length();
	const unsigned char *buf = source.GetBuf();
	OutBitStream out(len/2);

	MakeCodes(source);
	out.WriteBits( len, 32);
	for(int i=0;i<256;i++)
		out.WriteBits(codelen[i],6);
	for(int i=0;i<256;i++)
		if(codelen[i] > 0)
			out.WriteBits(codes[i],codelen[i]);

	for(size_t i=0;i<len;i++)
		out.WriteBits(codes[buf[i]],codelen[buf[i]]);

	out.Flush();
	return charbuf( move(out.GetCharbuf()) );
}

charbuf Huffman::UnDo(charbuf &source)
{
	BitReader input(source.GetBuf(), source.Length() );
	charbuf newbuf(source.Length() * 2);
	//int /*maxlen = 0,*/ symbols = 0;
	SymTree dectree;
	//vector<CCode> codesv;
	//codesv.reserve(256);
	//CCode newcode;

	size_t len = input.GetBits(32);
	for(int i=0;i<256;i++)
	{
		codelen[i] = input.GetBits(6);
		//codes[i] = 0;
		/*if(codelen[i] > maxlen)
			maxlen = codelen[i];*/
	}
	for(int i=0;i<256;i++)
		if(codelen[i] > 0)
		{
			//codes[i] = input.GetBits(codelen[i]);
			dectree.InsertCode(input.GetBits(codelen[i]),codelen[i],i);
			//newcode.c = i;
			//newcode.code = codes[i] << (maxlen - codelen[i]);
			//codesv.push_back( newcode );
			//symbols++;
		}

	//sort(codesv.begin(),codesv.end(),[](const CCode &l, const CCode &r){return l.code < r.code;});


	for(size_t i=0;i<len;i++)
	{
		/*int curbit = maxlen;
		unsigned char ch;
		auto beg = codesv.begin(), end = codesv.end();
		while(true)
		{
			unsigned int b = input.GetBit();
			beg = lower_bound(beg,end,newcode, [&curbit,&b](const CCode &c, CCode b2) { return  ((c.code>>(curbit-1)) & 0x1) < b;} );
			end = upper_bound(beg,end,newcode, [&curbit,&b](const CCode &c, CCode b2) { return  ((c.code>>(curbit-1)) & 0x1) < b;} );
			if(maxlen-curbit == codelen[beg->c] )
			{
				ch = beg->c;
				break;
			}
		}*/
		newbuf.AppendChar(dectree.DecodeSymbol(input));
	}

	return move(newbuf);
}