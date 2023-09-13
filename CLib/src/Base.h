#pragma once

#include "Stdafx.h"
#include "Base\Act.h"
#include <memory>

//Говорит сколько бит нужно для кодирования такого числа (для кодов Элиаса)
int NeedBits(unsigned int num);

std::unique_ptr<CLib::Base::Act> GetAction(int actnum);
