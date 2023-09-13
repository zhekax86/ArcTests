#include "Stdafx.h"
#include "Arythmetic.h"
#include <vector>

using namespace CLib::Base;
using namespace std;


Prob BaseModel::GetProbByCode(int code)
{
	Prob p = {0,0,0,0};
	return p;
}

Prob BaseModel::GetCodeByProb(uint ScaledValue)
{
	Prob p = {0,0,0,0};
	return p;
}

void BaseModel::UpdateCode(int code)
{
}

bool BaseModel::HasCode(int code)
{
	return false;
}

uint BaseModel::GetCount()
{
	return 0;
}

class AdaptiveOneCharModel : public BaseModel
{
	short CodeToIndex[257], IndexToCode[257];	//Код (256) : escape
	uint Probs[258], CharsInModel;

public:

	AdaptiveOneCharModel(): CharsInModel(1)
	{
		CodeToIndex[256] = 0;
		IndexToCode[0] = 256;
		//CharsInModel = 1;
		Probs[0] = 0;
		Probs[1] = 1;
		for(int i=1;i<257;i++)
			IndexToCode[i] = -1;
		for(int i=0;i<256;i++)
			CodeToIndex[i] = -1;
	}

	void GetSpace()
	{
		assert(Probs[CharsInModel] == ArMaxStat );

		uint tprobs[257];
		for(uint i=0;i<CharsInModel;i++)
		{
			tprobs[i] = (Probs[i+1] - Probs[i])/2;
			if(tprobs[i] == 0)
				tprobs[i] = 1;
		}
		for(uint i=0;i<CharsInModel;i++)
			Probs[i+1] = Probs[i] + tprobs[i];

		/*for(uint i=0;i<CharsInModel-1;i++)
			if(Probs[i+1] > Probs[i]+1)
			{
				for(uint j = i+1;j<=CharsInModel; j++)
					Probs[j]--;
				return;
			}*/
	}

	virtual Prob GetProbByCode(int code) override
	{
		Prob p;
		p.code = code;
		p.count = Probs[CharsInModel];

		if( CodeToIndex[code] == -1)
		{
			p.min = p.max =0;
			return p;
		}

		p.min = Probs[ CodeToIndex[code] ];
		p.max = Probs[ CodeToIndex[code] + 1 ];
		return p;
	}

	virtual Prob GetCodeByProb(uint ScaledValue) override
	{
		Prob p;
		p.count = Probs[CharsInModel];
		uint i;
		for(i=0; i < CharsInModel; i++) 
			if(ScaledValue < Probs[i+1])
			{
				p.max = Probs[i+1];
				p.min = Probs[i];
				p.code = IndexToCode[i];
				return p;
			}
		
		assert(false);
		return p;
	}

	virtual void UpdateCode(int code) override
	{
		assert(code <257);

		if(Probs[CharsInModel] == ArMaxStat)
			GetSpace();
		for(uint i = CodeToIndex[code]+1; i< CharsInModel+1;i++)
			Probs[i]++;
	}

	virtual bool HasCode(int code)
	{
		if(code <= 256 && CodeToIndex[code] != -1)
			return true;
		else 
			return false;
	}

	virtual uint GetCount() override
	{
		return Probs[CharsInModel];
	}

	void NewCode(int code)
	{
		assert(code < 256);
		assert(CodeToIndex[code] == -1 );

		if( Probs[ CharsInModel ] == ArMaxStat)
			GetSpace();

		CharsInModel++;
		for(int i = CharsInModel; i>=1; i--)
		{
			Probs[i] = Probs[i-1]+1;
			if(i>=2)
				IndexToCode[i-1] = IndexToCode[i-2];
		}
		IndexToCode[0] = code;
		for(uint i=0;i<CharsInModel;i++)
			CodeToIndex[IndexToCode[i]] = i;
	}
};

class NewCharSimpleModel : public BaseModel
{
private:
	vector<short> chars;
public:
	NewCharSimpleModel()
	{
		chars.reserve(256);	//пишем в вектор 256 символов
		for(short i=0;i<257;i++)
			chars.push_back(i);
	}

	virtual Prob GetProbByCode(int code) override
	{
		Prob p;
		p.count = chars.size();
		p.code = code;
		for(size_t i=0;i<chars.size();i++)
			if(chars[i] == code)
			{
				p.min = i;
				p.max = i+1;
				return p;
			}

		assert(false);
		return p;
	}

	virtual Prob GetCodeByProb(uint ScaledValue) override
	{
		assert(ScaledValue < chars.size() );

		Prob p;
		p.count = chars.size();
		p.code = chars[ScaledValue];
		p.min = ScaledValue;
		p.max = ScaledValue+1;
		return p;
	}

	virtual void UpdateCode(int code) override
	{
		//обновили код - значит второй раз он уже не попадется - удаляем его из модели
		chars.erase( find(chars.begin(), chars.end(),code) );
	}

	virtual bool HasCode(int code) override
	{
		return binary_search(chars.begin(),chars.end(),code);
	}

	virtual uint GetCount() override
	{
		return chars.size();
	}
};

void Arythmetic::PutBitPlusPending(uint bit, OutBitStream &stream)
{
	stream.WriteBits(bit,1);
	for(uint i=0;i<PendingBits;i++)
		stream.WriteBits( !bit,1);
	PendingBits = 0;
}

void Arythmetic::DumpValues(ofstream &file, uint position, int code)
{
	file << position <<": "<<Low << ' '<< High;
	if(code != -1)
		file << '('<<code<<')';
	file<<endl;
}

Arythmetic::Arythmetic() : Low(0),High(ArMaxVal),PendingBits(0)
{
}

void Arythmetic::EncodeProb(const Prob &prob, OutBitStream &stream)
{
	uint Range = High - Low + 1;
	High = Low + (Range*prob.max/prob.count) - 1;
	Low = Low + (Range*prob.min/prob.count);

	while(true)
	{
		if(High < ArHalf)
			PutBitPlusPending(0,stream);
		else if(Low >= ArHalf)
			PutBitPlusPending(1,stream);
		else if(Low >= ArOneForth && High < ArThreeForth )
		{
			PendingBits++;
			Low -= ArOneForth;
			High -= ArOneForth;
		}
		else
			break;

		High = ((High<<1)+1) & ArMaxVal;
		Low = (Low<<1) & ArMaxVal;
	}
}

int Arythmetic::DecodeCode(BaseModel &model, BitReader &stream)
{
	uint Range = High - Low + 1;
	uint ScaledValue = ((Value - Low +1) * model.GetCount() -1)/Range;
	Prob prob = model.GetCodeByProb(ScaledValue);

	High = Low + (Range*prob.max/prob.count) - 1;
	Low = Low + (Range*prob.min/prob.count);

	while(true)
	{
		if(High < ArHalf)
			;
		else if(Low >= ArHalf)
		{
			Value -= ArHalf;
			Low -= ArHalf;
			High -= ArHalf;
		}
		else if(Low >= ArOneForth && High < ArThreeForth)
		{
			Value -= ArOneForth;
			Low -= ArOneForth;
			High -= ArOneForth;
		}
		else 
			break;

		Low <<=1;
		High = (High<<1)+1;
		Value = (Value<<1)+stream.GetBit();

		assert(Value <= ArMaxVal);
	}

	return prob.code;
}

void Arythmetic::FinishEncoding(OutBitStream &stream)
{
	PendingBits++;
	if(Low < ArOneForth)
		PutBitPlusPending(0,stream);
	else
		PutBitPlusPending(1,stream);
	stream.Flush();
}

void Arythmetic::StartDecoding(BitReader &stream)
{
	Value = 0;
	for(int i = 0; i< ArValBits; i++)
		Value = (Value<<1) + stream.GetBit();
}

charbuf Arythmetic::Do(charbuf &source)
{
	size_t len = source.Length();
	const unsigned char *buf = source.GetBuf();
	OutBitStream out(len/2);

	AdaptiveOneCharModel cmodel;
	NewCharSimpleModel ncmodel;
	Prob prob;

	ofstream log("ari_enc.log");

	for(size_t i=0;i< len; i++)
	{
		if(cmodel.HasCode(buf[i]))
		{
			prob = cmodel.GetProbByCode(buf[i]);
			DumpValues(log,i,buf[i]);
			EncodeProb(prob,out);
			cmodel.UpdateCode(buf[i]);
		}
		else
		{
			prob = cmodel.GetProbByCode(256);
			DumpValues(log,i,256);
			EncodeProb(prob,out);
			cmodel.UpdateCode(256);
			cmodel.NewCode(buf[i]);
			prob = ncmodel.GetProbByCode(buf[i]);
			DumpValues(log,i,buf[i]);
			EncodeProb(prob,out);
			ncmodel.UpdateCode(buf[i]);
		}
	}
	
	prob = cmodel.GetProbByCode(256);
	EncodeProb(prob,out);
	prob = ncmodel.GetProbByCode(256);
	EncodeProb(prob,out);
	FinishEncoding(out);

	return move(out.GetCharbuf());
}

charbuf Arythmetic::UnDo(charbuf &source)
{
	BitReader input(source.GetBuf(),source.Length(),3);
	charbuf newbuf(source.Length()*2);

	AdaptiveOneCharModel cmodel;
	NewCharSimpleModel ncmodel;
	int code;
	StartDecoding(input);
	size_t i=0;
	ofstream log("ari_dec.log");

	while(true)
	{
		DumpValues(log,i);
		code = DecodeCode(cmodel,input);
		cmodel.UpdateCode(code);
		if(code == 256)
		{
			DumpValues(log,i);
			code = DecodeCode(ncmodel,input);
			if(code == 256)
				break;
			ncmodel.UpdateCode(code);
			cmodel.NewCode(code);
		}
		newbuf.AppendChar(code);
		i++;
	}

	return move(newbuf);
}
