#ifndef VIOLET_LINE_H
#define VIOLET_LINE_H

#include "violet/math/v2f.h"

void line_extrapolate(const v2f * a, const v2f * b, r32 t, v2f * p);
b8 ray_intersect_coords(const v2f * a, const v2f * adir,
                        const v2f * b, const v2f * bdir, r32 * t, r32 * u);
b8 line_intersect_coords(const v2f * a0, const v2f * a1,
                         const v2f * b0, const v2f * b1, r32 * t, r32 * u);
b8 line_intersect(const v2f * a0, const v2f * a1,
                  const v2f * b0, const v2f * b1, v2f * isec);
b8 segment_intersect(const v2f * a0, const v2f * a1,
                     const v2f * b0, const v2f * b1, v2f * isec);
void nearest_point_on_segment(const v2f * a, const v2f * b, const v2f * p,
                              v2f * result);
void nearest_point_on_line(const v2f * a, const v2f * b, const v2f * p,
                           v2f * result);
r32 point_to_segment_dist(const v2f * a, const v2f * b, const v2f * p);
r32 point_to_line_dist(const v2f * a, const v2f * b, const v2f * p);

#endif
