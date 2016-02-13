#ifndef VIOLET_TRANSFORM_H
#define VIOLET_TRANSFORM_H

#include "violet/math/m4.h"

namespace vlt
{
	struct VIOLET_API transform
	{
		static v2 get_position(const m4 & mat);
		static void set_position(m4 & mat, const v2 & position);

	private:

		transform() = delete;
	};
}

#endif