#include "Stdafx.h"
#include "Base.h"
#include <filesystem>


//Говорит сколько бит нужно для кодирования такого числа
int NeedBits(unsigned int num)
{
	unsigned int b = 1, n = 1;
	while (num >= b)
	{
		b <<= 1;
		n++;
	}
	return n - 1;
}