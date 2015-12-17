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

		enum class Modifier
		{
			NONE = 0x00,
			LSHIFT = 0x01,
			RSHIFT = 0x02,
			SHIFT = LSHIFT | RSHIFT,
			LCTRL = 0x04,
			RCTRL = 0x08,
			CTRL = LCTRL | RCTRL,
			LALT = 0x10,
			RALT = 0x20,
			ALT = LALT | RALT,
			NUM = 0x40,
			CAPS = 0x80
		};

	public:

		static bool isNumeric(unsigned char key);
		static bool isNumericInput(unsigned char key);
	};
}

#endif