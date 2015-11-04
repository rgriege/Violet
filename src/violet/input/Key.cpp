// ============================================================================

#include "violet/input/Key.h"

using namespace Violet;

// ============================================================================

bool Key::isNumeric(const unsigned char key)
{
	switch (key)
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return true;
	default:
		return false;
	}
}

// ----------------------------------------------------------------------------

bool Key::isNumericInput(const unsigned char key)
{
	return isNumeric(key) || key == '-' || key == '.';
}

// ============================================================================
