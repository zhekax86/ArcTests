﻿#include "Stdafx.h"
#include "Base.h"
#include "BWT.h"
#include <thread>
#include <mutex>
#include <condition_variable>

struct QuickSortQueueElem
{
	size_t start, stop;

	bool operator==(const QuickSortQueueElem &r)
	{
		return start == r.start && stop &&r.stop;
	};
};

class QuickSortQueueElemComparer
{
public:
	bool operator()(const QuickSortQueueElem &l, const QuickSortQueueElem &r)
	{
		//return (l.stop - l.start) < (r.stop- r.start);	//так вышла огроменная очередь и тормозища...
		return l.start > r.start;
	}
};

class ParallelBWT : public BWT
{
private:
	vector<thread> ThreadPool;
	uint Threads;

	void DropStrings(string filename);	//отладка

	size_t len;
	const unsigned char *buf;
	int eof;	//для обратного преобразования

	vector<int> Strings;	//строки для прямой сортировки

	vector<vector<uint>> CharCounts, CharOffsets;
	vector<uint> TotalCount;
	//vector<mutex> Mutexes;
	mutex mtxd,mtxo,mtxq;	//для доступа к флагам и очереди
	condition_variable Cond, CondQ;
	vector<bool> Flags;
	bool Order,Order2;
	//vector< condition_variable > Cond;

	//Очередь заданий для quick sort'а
	priority_queue<QuickSortQueueElem,vector<QuickSortQueueElem>,QuickSortQueueElemComparer> SortingQueue;
	//vector<QuickSortQueueElem> SortingDebug;	//отладка
	void Enqueue(size_t start, size_t stop);
	QuickSortQueueElem Dequeue();

	//Управление синхронизацией основного потока с рабочими
	void TellDone(uint Num);
	void WaitForOrder();
	void WaitForOrder2();
	void WaitUntilDone();
	void OrderToContinue();
	void OrderToContinue2();
	void JoinThreads();

	void EncoderSort(uint Num);
	void QuickSort(QuickSortQueueElem range);
	void ShellSort(QuickSortQueueElem range);
	void DecoderSort(uint Num);

	uint Symbol(uint shift, uint n);
	bool CompareStrings(uint left, uint right);	//true если left < right
public:
	ParallelBWT();
	virtual charbuf Do(charbuf &source) override;
	virtual charbuf UnDo(charbuf &source) override;
};