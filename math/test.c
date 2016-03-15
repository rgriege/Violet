#include "interval.h"
#include "m2.h"
#include "v2.h"

int main()
{
	interval i1, i2;
	r32 x;

	interval_contains(&i1, x);
	interval_contains(&i1, &i2);

	v2 p1, p2, p3;
	v2_add(&p1, &p2, &p3);

	m2 mat1, mat2;
	if (!m2_inverse(mat1, mat2))
		m2_init_rot(mat2, 0);
}
