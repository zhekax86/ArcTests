#include "Stdafx.h"
#include "Base.h"
#include "Base\Act.h"
#include "Base\OutBitStream.h"
#include "Base\BitReader.h"

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

class Arythmetic : public CLib::Base::Act
{
private:
	uint Low,High,Value;
	uint PendingBits;

	void PutBitPlusPending(uint bit, CLib::Base::OutBitStream &stream);

	void DumpValues(std::ofstream &file, uint position, int code=-1);
public:
	Arythmetic();

	void EncodeProb(const Prob &prob, CLib::Base::OutBitStream &stream);
	int DecodeCode(BaseModel &model, CLib::Base::BitReader &stream);
	void FinishEncoding(CLib::Base::OutBitStream &stream);
	void StartDecoding(CLib::Base::BitReader &stream);

	virtual CLib::Base::charbuf Do(CLib::Base::charbuf &source) override;
	virtual CLib::Base::charbuf UnDo(CLib::Base::charbuf &source) override;
};