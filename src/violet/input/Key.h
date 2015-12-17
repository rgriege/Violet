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

		enum Modifier
		{
			M_NONE = 0x00,
			M_LSHIFT = 0x01,
			M_RSHIFT = 0x02,
			M_SHIFT = M_LSHIFT | M_RSHIFT,
			M_LCTRL = 0x04,
			M_RCTRL = 0x08,
			M_CTRL = M_LCTRL | M_RCTRL,
			M_LALT = 0x10,
			M_RALT = 0x20,
			M_ALT = M_LALT | M_RALT,
			M_NUM = 0x40,
			M_CAPS = 0x80
		};

	public:

		static bool isNumeric(unsigned char key);
		static bool isNumericInput(unsigned char key);
	};
}

#endif