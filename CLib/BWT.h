#pragma once
#include "Stdafx.h"
#include "Base.h"


class BWT : public Act
{
protected:
	charbuf EncodeBuf(charbuf &source, vector<int> &SortedStrings);
	charbuf DecodeBuf(charbuf &source, vector<int> &Order);
public:
	virtual charbuf Do(charbuf &source) override;
	virtual charbuf UnDo(charbuf &source) override;
};