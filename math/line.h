#ifndef VIOLET_LINE_H
#define VIOLET_LINE_H

#include "violet/math/v2.h"

b8 line_intersect_coords(const v2 * a0, const v2 * a1,
                         const v2 * b0, const v2 * b1, r32 * t, r32 * u);
b8 line_intersect(const v2 * a0, const v2 * a1,
                  const v2 * b0, const v2 * b1, v2 * isec);
b8 segment_intersect(const v2 * a0, const v2 * a1,
                     const v2 * b0, const v2 * b1, v2 * isec);

#endif
