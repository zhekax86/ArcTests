#include "Stdafx.h"
#include "AdHuffman.h"

using namespace CLib::Base;
using namespace std;

AdHuffman::AdHuffman()
{
	for(int i=0;i<256;i++)
		CharPosition[i] = -1;

	tree.reserve(256+255);

	HTreeNode node;
	node.count=1;
	node.left=-1;
	node.right=-1;
	node.parent = 2;
	node.c = -1;
	tree.push_back(node);

	node.c = -2;
	tree.push_back(node);

	node.parent = -1;
	node.count = 2;
	node.left = 0;
	node.right = 1;
	node.c  = 0;
	tree.push_back(node);

	SpecCharPosition[0] = 0;
	SpecCharPosition[1] = 1;
}

void AdHuffman::_DumpTree(size_t i, bool Do)
{
	ostringstream filename;
	filename << (Do ? "code_" : "decode_") << i << ".txt";
	ofstream ofs(filename.str());
	for(size_t ind=0; ind < tree.size(); ind++)
	{
		ofs << ind << ":" << "{ count = "<<tree[ind].count << " char = " << tree[ind].c << " parent = " 
			<< tree[ind].parent ;
		if(tree[ind].left == -1 && tree[ind].right == -1)
			ofs << " leaf" << endl;
		else
			ofs << " left = " << tree[ind].left << " right = " << tree[ind].right << endl;
	}
}

int AdHuffman::GetStartNode(int ch)
{
	if(ch >= 0 )
		return CharPosition[ch];
	else
		return SpecCharPosition[ -ch -1];
}

void AdHuffman::AddChar(int nc)
{
	HTreeNode newnode;
	newnode.left = -1;
	newnode.right = -1;
	newnode.parent = -1;
	newnode.c = nc;
	newnode.count = 1;
	tree.insert(tree.begin(),newnode);

	tree.erase( remove_if(tree.begin(),tree.end(), [](HTreeNode &n) { return n.left != -1 && n.right != -1;} ), tree.end());
	for_each(tree.begin(),tree.end(),[](HTreeNode &n){ n.parent = -1;});

	//перестраиваем дерево
	//int i = 0;
	//while(i+1 < tree.size())
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
		//newnode.parent = nni;
		//i+=2;
	}

	//очистка и обновление указателей на символы
	for(int i=0;i<256;i++)
		CharPosition[i] = -1;

	for(size_t i=0;i<tree.size();i++)
		if(tree[i].left ==-1 && tree[i].right == -1)
		{
			if(tree[i].c >=0)
				CharPosition[tree[i].c]=i;
			else
				SpecCharPosition[ -tree[i].c -1] = i;
		}
}

void AdHuffman::UpdateChar(int nc)
{
	//находим узел по таблице символов
	int inode = GetStartNode(nc);
	/*if(nc >= 0 )
		inode = CharPosition[nc];
	else
		inode = SpecCharPosition[ -nc -1];*/

	while(true)
	{
		tree[inode].count++;
		if(tree[inode].parent == -1)
			return;

		//делаем обмен узлов, если вес текущего стал больше чем у следующего
		if(tree[inode].count > tree[inode+1].count)
		{
			int ni=inode+2;
			while(tree[inode].count > tree[ni].count)
				ni++;
			ni--;

			swap(tree[inode],tree[ni]);
			swap(inode,ni);
			if(tree[ni].left != -1 && tree[ni].right != -1)
			{
				tree[ tree[ni].left  ].parent = ni;
				tree[ tree[ni].right ].parent = ni;
			}
			else
			{
				if(tree[ni].c >= 0)
					CharPosition[tree[ni].c]=ni;
				else
					SpecCharPosition[ -tree[ni].c -1] = ni;
			}

			if(tree[inode].left != -1 && tree[inode].right != -1)
			{
				tree[ tree[inode].left  ].parent = inode;
				tree[ tree[inode].right ].parent = inode;
			}
			else
			{
				if(tree[inode].c >= 0)
					CharPosition[tree[inode].c]=inode;
				else
					SpecCharPosition[ -tree[inode].c -1] = inode;
			}
			swap(tree[inode].parent,tree[ni].parent);
		}

		inode = tree[inode].parent;
	}
}

void AdHuffman::UpdateCharLite(int nc)
{
	tree[GetStartNode(nc)].count++;
}

void AdHuffman::GetCode(int ch, unsigned int &code, unsigned int &len)
{
	unsigned int cc=0,cl=0;
	int inode = GetStartNode(ch);
	
	while(tree[inode].parent != -1)
	{
		if(tree[tree[inode].parent].right == inode)
			cc |= 1<<cl;
		cl++;
		inode = tree[inode].parent;
	}
	code = cc; len = cl;
}

int AdHuffman::NextSym(BitReader &input)
{
	int inode = tree.size()-1;
	while( tree[inode].left != -1 && tree[inode].right != -1)
		inode = input.GetBit() ? tree[inode].right : tree[inode].left;
	return tree[inode].c;
}

charbuf AdHuffman::Do(charbuf &source)
{
	size_t len = source.Length();
	const unsigned char *buf = source.GetBuf();
	//unsigned char * newbuf = new unsigned char[len];
	OutBitStream out(len/2);
	//unsigned int code, codelen;

	for(size_t i=0;i<len;i++)
	{
		//if(i >= 32 && i <= 36)
			//_DumpTree(i,true);
		//unsigned char ch = buf[i];
		EncodeChar(buf[i],out);
	}

	/*GetCode(-1,code,codelen);
	out.WriteBits(code,codelen);*/
	EncodeChar(-1,out);
	out.Flush();

	return charbuf( move(out.GetCharbuf()) );
}

charbuf AdHuffman::UnDo(charbuf &source)
{
	BitReader input(source.GetBuf(), source.Length() );
	charbuf newbuf(source.Length() * 2);
	int sym = DecodeChar(input);//NextSym(input);
	size_t i =0;

	while(sym != -1)
	{
		//if(i >= 32 && i <= 36)
			//_DumpTree(i,false);
		newbuf.AppendChar(sym);
		sym = DecodeChar(input);
		/*if(sym == -2)
		{
			sym = input.GetBits(8);
			UpdateChar(-2);
			AddChar(sym);
			newbuf.AppendChar( (unsigned int) sym);
		}
		else
		{
			newbuf.AppendChar( (unsigned char) sym);
			UpdateChar(sym);
		}
		i++;
		sym = NextSym(input);*/
	}

	return move(newbuf);
}

void AdHuffman::EncodeChar(int ch,OutBitStream &stream)
{
	unsigned int code, codelen;
	if (CharPosition[ch] == -1)
	{
		GetCode(-2,code,codelen);
		stream.WriteBits(code,codelen);
		UpdateCharLite(-2);
		AddChar(ch);
		stream.WriteBits(ch,8);
	}
	else
	{
		GetCode(ch,code,codelen);
		stream.WriteBits(code,codelen);
		UpdateChar(ch);
	}
}

int AdHuffman::DecodeChar(BitReader &stream)
{
	int sym = NextSym(stream);
	if(sym == -2)
	{
		sym = stream.GetBits(8);
		UpdateCharLite(-2);
		AddChar(sym);
	}
	else
	{
		UpdateChar(sym);
	}
	return sym;
}