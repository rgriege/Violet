#ifndef VIOLET_LINE_H
#define VIOLET_LINE_H

#include "violet/math/v2.h"

void line_extrapolate(const v2 * a, const v2 * b, r32 t, v2 * p);
b8 ray_intersect_coords(const v2 * a, const v2 * adir,
                        const v2 * b, const v2 * bdir, r32 * t, r32 * u);
b8 line_intersect_coords(const v2 * a0, const v2 * a1,
                         const v2 * b0, const v2 * b1, r32 * t, r32 * u);
b8 line_intersect(const v2 * a0, const v2 * a1,
                  const v2 * b0, const v2 * b1, v2 * isec);
b8 segment_intersect(const v2 * a0, const v2 * a1,
                     const v2 * b0, const v2 * b1, v2 * isec);
void nearest_point_on_segment(const v2 * a, const v2 * b, const v2 * p,
                              v2 * result);
void nearest_point_on_line(const v2 * a, const v2 * b, const v2 * p,
                           v2 * result);
r32 point_to_segment_dist(const v2 * a, const v2 * b, const v2 * p);
r32 point_to_line_dist(const v2 * a, const v2 * b, const v2 * p);

#endif
