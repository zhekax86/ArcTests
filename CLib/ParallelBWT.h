#include "Stdafx.h"
#include "Base.h"
#include "BWT.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

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
	std::vector<std::thread> ThreadPool;
	unsigned int Threads;

	void DropStrings(std::string filename);	//отладка

	size_t len;
	const unsigned char *buf;
	int eof;	//для обратного преобразования

	std::vector<int> Strings;	//строки для прямой сортировки

	std::vector<std::vector<uint>> CharCounts, CharOffsets;
	std::vector<uint> TotalCount;
	//vector<mutex> Mutexes;
	std::mutex mtxd,mtxo,mtxq;	//для доступа к флагам и очереди
	std::condition_variable Cond, CondQ;
	std::vector<bool> Flags;
	bool Order,Order2;
	//vector< condition_variable > Cond;

	//Очередь заданий для quick sort'а
	std::priority_queue<QuickSortQueueElem,std::vector<QuickSortQueueElem>,QuickSortQueueElemComparer> SortingQueue;
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
	virtual CLib::Base::charbuf Do(CLib::Base::charbuf &source) override;
	virtual CLib::Base::charbuf UnDo(CLib::Base::charbuf &source) override;
};