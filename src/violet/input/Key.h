#ifndef VIOLET_Key_H
#define VIOLET_Key_H

#include "violet/Defines.h"

namespace Violet
{
	class VIOLET_API Key
	{
	public:

		enum SpecialKey : unsigned char
		{
			Null = 0,
			Backspace = 8,
			Return = 13,
			Escape = 27,
			Delete = 127
		};

	public:

		static bool isNumeric(unsigned char key);
		static bool isNumericInput(unsigned char key);
	};
}

#endif