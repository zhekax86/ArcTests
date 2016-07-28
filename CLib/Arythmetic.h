#include "Stdafx.h"
#include "Base.h"

// Описание того как это работает здесь
// http://marknelson.us/2014/10/19/data-compression-with-arithmetic-coding/
// я малость переделал по своему

//#define ARMAXVAL (((1)<<17)-1)
//#define ARFORTH 
static const uint ArValBits = 17;
static const uint ArStatBits = 15;
static const uint ArMaxStat = ((1)<<ArStatBits)-1;
static const uint ArMaxVal = ((1)<<ArValBits)-1;
static const uint ArOneForth = (1)<<(ArValBits-2);
static const uint ArHalf = ArOneForth*2;
static const uint ArThreeForth = ArOneForth*3;

struct Prob
{
	uint min,max,count;
	int code;
};

class BaseModel
{
public:
	virtual Prob GetProbByCode(int code);
	virtual Prob GetCodeByProb(uint ScaledValue);
	virtual void UpdateCode(int code);
	virtual bool HasCode(int code);
	virtual uint GetCount();
};

class Arythmetic : public Act
{
private:
	uint Low,High,Value;
	uint PendingBits;

	void PutBitPlusPending(uint bit, OutBitStream &stream);

	void DumpValues(ofstream &file, uint position, int code=-1);
public:
	Arythmetic();

	void EncodeProb(const Prob &prob, OutBitStream &stream);
	int DecodeCode(BaseModel &model, BitReader &stream);
	void FinishEncoding(OutBitStream &stream);
	void StartDecoding(BitReader &stream);

	virtual charbuf Do(charbuf &source) override;
	virtual charbuf UnDo(charbuf &source) override;
};