#pragma once
#include "Stdafx.h"
#include "Base.h"

//Преобразование Барроуза Уилера
// https://ru.wikipedia.org/wiki/%D0%9F%D1%80%D0%B5%D0%BE%D0%B1%D1%80%D0%B0%D0%B7%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D0%B5_%D0%91%D0%B0%D1%80%D1%80%D0%BE%D1%83%D0%B7%D0%B0_%E2%80%94_%D0%A3%D0%B8%D0%BB%D0%B5%D1%80%D0%B0

class BWT : public Act
{
protected:
	charbuf EncodeBuf(charbuf &source, vector<int> &SortedStrings);
	charbuf DecodeBuf(charbuf &source, vector<int> &Order);
public:
	virtual charbuf Do(charbuf &source) override;
	virtual charbuf UnDo(charbuf &source) override;
};