
#include "relwarb_utils.h"
#include "relwarb_math.h"

bool32 StrEqual(const char* str1, const char* str2, int length)
{
	bool32 matched = true;

	for (int idx = 0; idx < length; ++idx)
	{
		if (*str1 != *str2)
		{
			matched = false;
			break;
		}

		++str1;
		++str2;
	}

	return matched;
}

uint32 StrLength(const char* str)
{
	uint32 result = 0;
	for (; *str; ++str, ++result);

	return result;
}
