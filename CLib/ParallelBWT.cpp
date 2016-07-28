#include "Stdafx.h"
#include "ParallelBWT.h"

void ParallelBWT::DropStrings(string filename)
{
	ofstream log(filename);
	for(auto l : Strings)
		log << l << endl;
}

//������� i-�� ������ ��� ���������� ��� ������ ��������������
void EncodingThread(uint Num, uint Count, const unsigned char *buf, size_t len, vector<vector<uint>> CountTable)
{
	//�������� i-� ������ ���-��
	for(auto it = CountTable[Num].begin() ; it != CountTable[Num].end() ;it++)
		*it = 0;

	//������� ���������� ������ ������ ������
	for(size_t i = len/Count*Num ; i<= ((Count == (Num+1))? len:(len/Count*(Num+1))) ; i++)
		CountTable[Num][ i==len ? 256: buf[i] ]++;
	

}

void ParallelBWT::Enqueue(size_t start, size_t stop)
{
	if((start !=0 || stop != 0 ) && start != stop)
	{
		lock_guard<mutex> lock(mtxq);
		QuickSortQueueElem e = {start, stop};
		SortingQueue.push(e);
		
		//�������
		/*for(auto de : SortingDebug)
		{
			if( (de.start <= start && de.stop >= start)
				||(de.start <= stop && de.stop >= stop)
				||(de.start <= start && de.stop >= stop)
				)
				assert(true == false);
		}
		SortingDebug.push_back(e);*/

	}
}

QuickSortQueueElem ParallelBWT::Dequeue()
{
	lock_guard<mutex> lock(mtxq);
	QuickSortQueueElem e ={0,0};
	if(!SortingQueue.empty())
	{
		e = SortingQueue.top();
		SortingQueue.pop();

		//�������
		//SortingDebug.erase( find(SortingDebug.begin(),SortingDebug.end(), e ) );
	}
	return e;
}

//�������� � ��� ��� ����� ������� ������
void ParallelBWT::TellDone(uint Num)
{
	{
		lock_guard<mutex> lock(mtxd);
		Flags[Num] = true;
	}
	Cond.notify_one();
}

//����� ���� �� ������ ������� �� ��������� ������
void ParallelBWT::WaitForOrder()
{
	unique_lock<mutex> QuickLock(mtxo);
	while(!Order)
		CondQ.wait(QuickLock);
}

void ParallelBWT::WaitForOrder2()
{
	unique_lock<mutex> QuickLock(mtxo);
	while(!Order2)
		CondQ.wait(QuickLock);
}


//����� ���� ������ �� ����������� �� ���������
void ParallelBWT::WaitUntilDone()
{
	for(bool b = false; b == false;)
	{
		unique_lock<mutex> Lock(mtxd);
		b = true;
		for(auto f:Flags)
			if(!f)
				b = false;
		if(b)
			break;
		this_thread::yield();
		Cond.wait(Lock);
		
	}
}

//������� ������� �������� ������
void ParallelBWT::OrderToContinue()
{
	for(auto b : Flags)
		b = false;
	{
		lock_guard<mutex> lock(mtxo);
		Order = true;
	}
	CondQ.notify_all();
}

void ParallelBWT::OrderToContinue2()
{
	for(auto b : Flags)
		b = false;
	{
		lock_guard<mutex> lock(mtxo);
		Order2 = true;
	}
	CondQ.notify_all();
}

void ParallelBWT::JoinThreads()
{
	for(size_t i=0; i< Threads; i++)
		ThreadPool[i].join();
}

//true ���� left < right
bool ParallelBWT::CompareStrings(uint left, uint right)
{
	//���� ����������� ������ ���� � �����, �� ����� false
	//� �� ����� ��� ����� ����������, � ����� - ����
	if(left==right)
		return false;

	for(size_t i = 0;i<len;i++)
	{
		size_t xo = left+i,yo=right+i;
		if(xo > len) xo -= len;
		if(yo > len) yo -= len;
		//size_t xo= (left+i)%len, yo = (right+i)%len;

		if(xo == len-1)
			return false;
		if(yo == len-1)
			return true;
		if(buf[xo] != buf[yo])
			return buf[xo]< buf[yo];
	}

	//�� ������
	return false;
}

uint inline ParallelBWT::Symbol(uint shift, uint n)
{
	uint pos = shift + n;
	if( pos == (len-1))
		return 256;
	if( pos > len)
		pos -= len;
	return buf[pos];
}

//����� ������� i-�� ������ ��� ������� ��������������
void ParallelBWT::EncoderSort(uint Num)
{
	size_t start = len/Threads*Num, stop = ((Threads == (Num+1))? (len-1):(len/Threads*(Num+1)));
	//������� ���������� ������ ������ ������
	for(size_t i = start ; i< stop ; i++)
		CharCounts[Num][ /*i==(len-1) ? 256: buf[i]*/ Symbol(i,0) ]++;

	//�������� �������� ������, ��� ��������� �������
	TellDone(Num);
	//���� ���� ������� ����� ������ ����������
	WaitForOrder();

	//������� �������� � ��������� � ������ ������� ������� ������
	{
		vector<uint> Offsets(257,0);
		/*if(Num > 0)
		{  --����������
			for(uint i = 0; i< Num; i++)
				Offsets[0] += CharCounts[i][0];	//�������� ��� ������� ������� 0, �� ����� ���������� �������� ���������� �������
		}*/
		for(size_t i=1; i< 257; i++)
			Offsets[i] = Offsets[i-1] + TotalCount[i-1];
		for(uint j=0; j < Num; j++)	//��������� �������� �� ���-�� �������� �� ���������� �������
			for(size_t i=0; i< 257; i++)
				Offsets[i] += CharCounts[j][i];

		//��������� ������ � ������, �������� ��������������
		for(size_t i = start; i <= stop; i++)
		{
			uint c = Symbol(i,0) /*i == (len-1) ? 256 : buf[i]*/;	//1-� ������ ������
			Strings[ Offsets[c]++ ] = i;
		}

		TellDone(Num);	//��������� ��������� ����������� ��������
	}

	WaitForOrder2();	//���� ������� �� �����������

	while(true)	//QuickSort'�� ���� ���-�� ���� � �������
	{
		QuickSortQueueElem e = Dequeue();
		if(e.start ==0 && e.stop == 0)	//���� ������� ���������, �� ������� �� ������� � ����� �����������
			return;
		QuickSort(e);
	}
}

void ParallelBWT::DecoderSort(uint Num)
{
	size_t start = len/Threads*Num, stop = ((Threads == (Num+1))? (len-1):(len/Threads*(Num+1)));

	//������� ���������� ������ ������ ������
	for(size_t i = start ; i< stop ; i++)
		CharCounts[Num][ i==eof?256:buf[i] ]++;

	//�������� �������� ������, ��� ��������� �������
	TellDone(Num);
	//���� ���� ������� ����� ������ ����������
	WaitForOrder();

	//������� �������� � ��������� � ������ ������� ������� ������
	{
		vector<uint> Offsets(257,0);
		for(size_t i=1; i< 257; i++)
			Offsets[i] = Offsets[i-1] + TotalCount[i-1];
		for(uint j=0; j < Num; j++)	//��������� �������� �� ���-�� �������� �� ���������� �������
			for(size_t i=0; i< 257; i++)
				Offsets[i] += CharCounts[j][i];

		//��������� ������ � ������, �������� ��������������
		for(size_t i = start; i <= stop; i++)
		{
			uint c = i==eof?256:buf[i] /*i == (len-1) ? 256 : buf[i]*/;	//1-� ������ ������
			Strings[ Offsets[c]++ ] = i;
		}

	}
}

void ParallelBWT::QuickSort(QuickSortQueueElem range)
{
	//���� � ��� ������ 32 ���������, �� �������� ���������� �� �������� (��� ��������� �������� - �������)
	//if(range.stop-range.start < 32)
	{
		ShellSort(range);
		return;
	}

	size_t i=range.start;
	size_t j = range.stop;
	uint pivot = Strings[ i + (( j-i)/2) ];

	do
	{
		while( CompareStrings(Strings[i],pivot) && i<=(range.stop-1) ) i++;
		while( CompareStrings(pivot,Strings[j]) && j>=(range.start+1) ) j--;

		if (i <= j)
		{
			swap(Strings[i], Strings[j]);
			if(i<range.stop) i++;
			if(j>range.start) j--;
		}
	} while (i<=j);

	if(j > range.start)	Enqueue(range.start, j);
	if(i < range.stop) 	Enqueue(i, range.stop);
}

void ParallelBWT::ShellSort(QuickSortQueueElem range)
{
	size_t size = range.stop-range.start +1;
	size_t gap = size/2;
	while(gap > 0)
	{
		for(size_t i=0; i< size-gap; i++)
		{
			size_t j=i+gap;
			int tmp=Strings[range.start + j];
			while( j>= gap && CompareStrings(tmp, Strings[range.start + j - gap] ) )
			{
				Strings[range.start + j] = Strings[range.start + j - gap];
				j -= gap;
			}
			Strings[range.start + j] = tmp;
		}

		if(gap == 2)
			gap = 1;
		else
			gap /= 2.2;
	}

	/**
	�������� ������� �������� �� �����
	http://www.programming-algorithms.net/article/41653/Shell-sort
 * Shell sort - sort with diminishing increment (descending)
 * @param array to be sorted
 * @param size array size
 * @return sorted array
 */
/*int * shellSort(int * array, int size) {
    int gap = size / 2;
    while (gap > 0) {
        for (int i = 0; i < size - gap; i++) { //modified insertion sort
            int j = i + gap;
            int tmp = array[j];
            while (j >= gap && tmp > array[j - gap]) {
                array[j] = array[j - gap];
                j -= gap;
            }
            array[j] = tmp;
        }
        if (gap == 2) { //change the gap size
            gap = 1;
        } else {
            gap /= 2.2;
        }
    }
    return array;
}*/


}

ParallelBWT::ParallelBWT(): Threads(thread::hardware_concurrency()), Order(false), Order2(false)
{
	ThreadPool.reserve( Threads );
	Flags.reserve( Threads );
}

charbuf ParallelBWT::Do(charbuf &source)
{
	/*
	���� �����: ������ RadixSort �� ������� ������� �����.
	��� ������������ ������ �� ��������� ������, ������ �� �����������
	����� ������ ������ �������������� ��������� � ������� QuickSort.

	RadixSort �������� ����������� � ���� �������. ����� ��� �� �������� QuickSort.
	�������������� ������ ��� RadixSort �� �����������, �.�. ��������� �������� 
	��������� ������������ "�� ����" � ����� ������� � ������ �������� ����������� �� �������
	������� �������
	*/
	len = source.Length()+1;
	buf = source.GetBuf();
	//vector<int> Strings(len+1,0);
		
	for(uint i=0;i<Threads;i++)
	{
		CharCounts.push_back(vector<uint>(257,0));
		Flags.push_back(false);
	}
	for(uint i=0;i< Threads;i++)
		ThreadPool.push_back(thread(&ParallelBWT::EncoderSort,this,i));

	//���� ��� �������, �������� ������ ������..
	Strings.insert( Strings.begin(), len, 0);
	/*Strings.reserve(len);
	for(size_t i=0;i < len; i++)
		Strings.push_back(0);*/

	//���� ���� ��� ������ ���������� �������
	WaitUntilDone();

	//������� ����� ���������� ��������
	TotalCount.reserve(257);
	for(size_t i=0;i<257;i++)
	{
		uint cnt=0;
		for(uint j=0; j< Threads; j++)
			cnt += CharCounts[j][i];
		TotalCount.push_back(cnt);
	}

	OrderToContinue();  //������� ��������� ������� ����������
	WaitUntilDone();	//� ��� ��� ���� ��
	//������ ��������� �������...
	{
		size_t offset = 0;
		for(size_t i =0; i< 257; i++)
			if(TotalCount[i] > 0)
			{
				if(TotalCount[i] > 1)
					Enqueue(offset,offset+TotalCount[i]-1);
				offset += TotalCount[i];
			}
	}

	//DropStrings("arr_after_radix.txt");	//�������
	OrderToContinue2();	//���������
	JoinThreads();	//� ���� ���� ��� �������������

	return move(EncodeBuf(source,Strings));
}

charbuf ParallelBWT::UnDo(charbuf &source)
{
	/*
	� �������� ������� ��������� ����� RadixSort, � ������� � ��� ������ ��������, �� ���� ���������� ���� �� ��������
	*/
	return BWT::UnDo(source);

	len = source.Length() -3;
	const unsigned char *buf = source.GetBuf();
	eof = *((int*)buf);
	buf+=4;
	
	for(uint i=0;i<Threads;i++)
	{
		CharCounts.push_back(vector<uint>(257,0));
		Flags.push_back(false);
	}
	for(uint i=0;i< Threads;i++)
		ThreadPool.push_back(thread(&ParallelBWT::DecoderSort,this,i));

	Strings.insert(Strings.begin(),len,0);

	//���� ���� ��� ������ ���������� �������
	WaitUntilDone();

	//������� ����� ���������� ��������
	TotalCount.reserve(257);
	for(size_t i=0;i<257;i++)
	{
		uint cnt=0;
		for(uint j=0; j< Threads; j++)
			cnt += CharCounts[j][i];
		TotalCount.push_back(cnt);
	}

	OrderToContinue();  //������� ��������� ������� ����������
	JoinThreads();	//� ���� ���� ��� �������������

	return move(DecodeBuf(source,Strings));
}